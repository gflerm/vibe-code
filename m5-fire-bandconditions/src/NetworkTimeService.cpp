#include "NetworkTimeService.h"

#include <M5Stack.h>
#include <WiFi.h>
#include <sys/time.h>

#include "AppConfig.h"

namespace {
const IPAddress kPrimaryNtpServer(192, 168, 8, 10);
const char *const kFallbackServers[] = {
    "0.pool.ntp.org",
    "1.pool.ntp.org",
    "2.pool.ntp.org",
    "3.pool.ntp.org",
};
}

bool NetworkTimeService::connectWifi(const char *ssid, const char *password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  const uint32_t startedAt = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - startedAt < AppConfig::kWifiTimeoutMs) {
    delay(250);
    M5.update();
  }

  if (!wifiConnected()) {
    Serial.println("WiFi connection timed out");
    return false;
  }

  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());
  return true;
}

bool NetworkTimeService::synchronizeClock() {
  udp_.begin(AppConfig::kLocalUdpPort);

  Serial.println("Trying NTP server 192.168.8.10:123");
  if (requestNtpTime(kPrimaryNtpServer)) {
    Serial.println("Time synchronized using 192.168.8.10:123");
    clockSynchronized_ = true;
    return true;
  }

  for (const char *serverName : kFallbackServers) {
    Serial.printf("Trying NTP server %s:%u\n", serverName,
                  AppConfig::kNtpPort);
    IPAddress address;
    if (WiFi.hostByName(serverName, address) && requestNtpTime(address)) {
      Serial.printf("Time synchronized using %s:123\n", serverName);
      clockSynchronized_ = true;
      return true;
    }
  }

  Serial.println("All NTP servers failed");
  return false;
}

bool NetworkTimeService::wifiConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

bool NetworkTimeService::requestNtpTime(const IPAddress &server) {
  uint8_t packet[48] = {};
  packet[0] = 0b00100011;  // NTP v4, client mode.
  packet[2] = 6;
  packet[3] = 0xEC;

  while (udp_.parsePacket() > 0) {
    udp_.flush();
  }

  if (!udp_.beginPacket(server, AppConfig::kNtpPort)) {
    return false;
  }
  udp_.write(packet, sizeof(packet));
  if (!udp_.endPacket()) {
    return false;
  }

  const uint32_t startedAt = millis();
  while (millis() - startedAt < AppConfig::kNtpTimeoutMs) {
    if (udp_.parsePacket() >= static_cast<int>(sizeof(packet))) {
      udp_.read(packet, sizeof(packet));

      // Reject servers that explicitly report an unsynchronized clock. This
      // prevents a reachable but unhealthy local server from setting bad time.
      const uint8_t leapIndicator = packet[0] >> 6;
      const uint8_t mode = packet[0] & 0x07;
      const uint8_t stratum = packet[1];
      if (leapIndicator == 3 || (mode != 4 && mode != 5) || stratum == 0) {
        return false;
      }

      const uint32_t ntpSeconds =
          (static_cast<uint32_t>(packet[40]) << 24) |
          (static_cast<uint32_t>(packet[41]) << 16) |
          (static_cast<uint32_t>(packet[42]) << 8) |
          static_cast<uint32_t>(packet[43]);
      if (ntpSeconds <= AppConfig::kNtpToUnixEpoch) {
        return false;
      }

      timeval now = {};
      now.tv_sec =
          static_cast<time_t>(ntpSeconds - AppConfig::kNtpToUnixEpoch);
      return settimeofday(&now, nullptr) == 0;
    }
    delay(10);
  }
  return false;
}
