#pragma once

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

// Manages inactivity, LCD power, CPU frequency, and the Fire's ten RGB LEDs.
// update() returns true while normal button/application processing must be
// suppressed (during power save and until the wake-up button is released).
class PowerSaveManager {
 public:
  PowerSaveManager();

  void begin();
  bool update(bool anyButtonPressed);
  bool isPowerSaving() const { return powerSaving_; }

 private:
  void enterPowerSave();
  void leavePowerSave();
  void updateLedAnimation(uint32_t now);
  void showLeds();
  void turnLedsOff();

  Adafruit_NeoPixel leds_;
  uint32_t lastActivityMs_ = 0;
  uint32_t powerSaveStartedMs_ = 0;
  uint32_t lastLedUpdateMs_ = 0;
  uint32_t normalCpuFrequencyMhz_ = 240;
  bool powerSaving_ = false;
  bool waitForWakeButtonRelease_ = false;
};
