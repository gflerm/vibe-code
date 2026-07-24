#pragma once

#include <Arduino.h>

// Central configuration for networking, refresh intervals, and the default QTH.
// Keeping these values here avoids scattering hardware/application policy across
// otherwise reusable modules.
namespace AppConfig {
constexpr uint32_t kWifiTimeoutMs = 20000;
constexpr uint32_t kNtpTimeoutMs = 3000;
constexpr uint32_t kSolarRefreshMs = 15UL * 60UL * 1000UL;
constexpr uint32_t kSolarRetryMs = 60UL * 1000UL;
constexpr uint32_t kButtonHoldMs = 1000;
constexpr uint16_t kInitializationStepDelayMs = 200;

// Power-saving behaviour. LED brightness is the maximum of the breathing
// animation, expressed as a percentage of the SK6812's full output.
constexpr uint32_t kDisplaySleepTimeoutSeconds = 120;
constexpr uint8_t kPowerSaveLedBrightnessPercent = 2;
constexpr uint16_t kPowerSaveLedBreathPeriodMs = 8000;
constexpr uint16_t kPowerSaveLedUpdateMs = 40;
constexpr uint8_t kDisplayBrightness = 80;
constexpr uint16_t kPowerSaveCpuFrequencyMhz = 80;
// Rebooting on wake avoids relying on the LCD, Wi-Fi, and CPU-frequency state
// surviving a prolonged power-save period. Set to false to wake in place.
constexpr bool kRebootOnPowerSaveWake = true;

constexpr uint8_t kRgbLedPin = 15;
constexpr uint8_t kRgbLedCount = 10;

static_assert(kPowerSaveLedBrightnessPercent <= 100,
              "LED brightness must be between 0 and 100 percent");
static_assert(kDisplaySleepTimeoutSeconds > 0,
              "Display sleep timeout must be greater than zero");

constexpr uint16_t kLocalUdpPort = 2390;
constexpr uint16_t kNtpPort = 123;
constexpr uint32_t kNtpToUnixEpoch = 2208988800UL;

constexpr char kSolarDataUrl[] = "https://www.hamqsl.com/solarxml.php";
constexpr char kDefaultLocator[] = "JF95eu";
constexpr char kPreferencesNamespace[] = "bandclock";
constexpr char kLocatorPreferenceKey[] = "locator";
constexpr char kTimeZonePreferenceKey[] = "timezone";
}  // namespace AppConfig
