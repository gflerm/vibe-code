#pragma once

#include <Arduino.h>
#include <WiFiUdp.h>

// Handles the two network prerequisites used by the application: joining Wi-Fi
// and setting the ESP32 system clock from NTP.
class NetworkTimeService {
 public:
  bool connectWifi(const char *ssid, const char *password);
  bool synchronizeClock();

  bool wifiConnected() const;
  bool clockSynchronized() const { return clockSynchronized_; }

 private:
  bool requestNtpTime(const IPAddress &server);

  WiFiUDP udp_;
  bool clockSynchronized_ = false;
};
