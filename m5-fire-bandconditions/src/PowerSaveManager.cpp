#include "PowerSaveManager.h"

#include <M5Stack.h>
#include <esp32-hal-cpu.h>

#include "AppConfig.h"

PowerSaveManager::PowerSaveManager()
    : leds_(AppConfig::kRgbLedCount, AppConfig::kRgbLedPin,
            NEO_GRB + NEO_KHZ800) {}

void PowerSaveManager::begin() {
  normalCpuFrequencyMhz_ = getCpuFrequencyMhz();
  lastActivityMs_ = millis();

  leds_.begin();
  // M5 recommends open-drain mode for the Fire's RGB data pin. Reapply it
  // after each transmission because LED drivers can reconfigure the GPIO.
  pinMode(AppConfig::kRgbLedPin, OUTPUT_OPEN_DRAIN);
  turnLedsOff();
}

bool PowerSaveManager::update(bool anyButtonPressed) {
  const uint32_t now = millis();

  if (powerSaving_) {
    if (anyButtonPressed) {
      leavePowerSave();
      waitForWakeButtonRelease_ = true;
    } else {
      updateLedAnimation(now);
    }
    return true;
  }

  // Consume the wake press completely. A long wake press must not open an
  // editor after the LCD comes back on.
  if (waitForWakeButtonRelease_) {
    if (!anyButtonPressed) {
      waitForWakeButtonRelease_ = false;
      lastActivityMs_ = now;
    }
    return true;
  }

  if (anyButtonPressed) {
    lastActivityMs_ = now;
  }

  const uint32_t timeoutMs =
      AppConfig::kDisplaySleepTimeoutSeconds * 1000UL;
  if (static_cast<uint32_t>(now - lastActivityMs_) >= timeoutMs) {
    enterPowerSave();
    return true;
  }

  return false;
}

void PowerSaveManager::enterPowerSave() {
  powerSaving_ = true;
  powerSaveStartedMs_ = millis();
  lastLedUpdateMs_ = 0;

  M5.Lcd.setBrightness(0);
  M5.Lcd.sleep();

  if (!setCpuFrequencyMhz(AppConfig::kPowerSaveCpuFrequencyMhz)) {
    Serial.println("Unable to lower CPU frequency");
  }
  Serial.printf("Power save: LCD off, CPU %u MHz\n",
                getCpuFrequencyMhz());
  updateLedAnimation(powerSaveStartedMs_);
}

void PowerSaveManager::leavePowerSave() {
  if (!setCpuFrequencyMhz(normalCpuFrequencyMhz_)) {
    Serial.println("Unable to restore CPU frequency");
  }

  turnLedsOff();
  M5.Lcd.wakeup();
  delay(120);  // The ILI9342 needs time to leave sleep mode.
  M5.Lcd.setBrightness(AppConfig::kDisplayBrightness);

  powerSaving_ = false;
  lastActivityMs_ = millis();
  Serial.printf("Power save ended: CPU %u MHz\n", getCpuFrequencyMhz());
}

void PowerSaveManager::updateLedAnimation(uint32_t now) {
  if (lastLedUpdateMs_ != 0 &&
      static_cast<uint32_t>(now - lastLedUpdateMs_) <
          AppConfig::kPowerSaveLedUpdateMs) {
    return;
  }
  lastLedUpdateMs_ = now;

  const uint32_t phase =
      (now - powerSaveStartedMs_) % AppConfig::kPowerSaveLedBreathPeriodMs;
  const uint32_t halfPeriod = AppConfig::kPowerSaveLedBreathPeriodMs / 2;
  const uint8_t triangle = static_cast<uint8_t>(
      phase < halfPeriod ? (phase * 255UL) / halfPeriod
                         : ((AppConfig::kPowerSaveLedBreathPeriodMs - phase) *
                            255UL) /
                               halfPeriod);

  // Smoothstep makes the change gentle at the top and bottom of each breath.
  const uint32_t t = triangle;
  const uint8_t eased = static_cast<uint8_t>(
      (t * t * (765UL - (2UL * t))) / (255UL * 255UL));
  const uint8_t maximum = static_cast<uint8_t>(
      (255UL * AppConfig::kPowerSaveLedBrightnessPercent) / 100UL);
  const uint8_t value =
      static_cast<uint8_t>((maximum * static_cast<uint16_t>(eased)) / 255U);

  // Spread a full rainbow across the ten LEDs and slowly rotate it over time.
  const uint16_t baseHue =
      static_cast<uint16_t>(((now - powerSaveStartedMs_) * 65535UL) / 12000UL);
  for (uint8_t i = 0; i < AppConfig::kRgbLedCount; ++i) {
    const uint16_t hue = static_cast<uint16_t>(
        baseHue + (static_cast<uint32_t>(i) * 65536UL) /
                      AppConfig::kRgbLedCount);
    leds_.setPixelColor(i, leds_.ColorHSV(hue, 255, value));
  }
  showLeds();
}

void PowerSaveManager::showLeds() {
  pinMode(AppConfig::kRgbLedPin, OUTPUT_OPEN_DRAIN);
  leds_.show();
  pinMode(AppConfig::kRgbLedPin, OUTPUT_OPEN_DRAIN);
}

void PowerSaveManager::turnLedsOff() {
  leds_.clear();
  showLeds();
}
