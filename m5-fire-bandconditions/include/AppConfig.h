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

constexpr uint16_t kLocalUdpPort = 2390;
constexpr uint16_t kNtpPort = 123;
constexpr uint32_t kNtpToUnixEpoch = 2208988800UL;

constexpr char kSolarDataUrl[] = "https://www.hamqsl.com/solarxml.php";
constexpr char kDefaultLocator[] = "JF95eu";
constexpr char kPreferencesNamespace[] = "bandclock";
constexpr char kLocatorPreferenceKey[] = "locator";
constexpr char kTimeZonePreferenceKey[] = "timezone";
}  // namespace AppConfig
