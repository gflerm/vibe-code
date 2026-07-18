#pragma once

#include <Arduino.h>
#include <time.h>

// Owns the station's Maidenhead locator and all location-derived calculations.
// The active locator is persisted in ESP32 NVS and survives normal reflashing.
class StationLocation {
 public:
  void begin();

  const String &locator() const { return locator_; }
  double latitude() const { return latitude_; }
  double longitude() const { return longitude_; }

  bool save(const String &locator);
  double solarElevationDegrees(time_t utcTime) const;
  bool isDaylight(time_t utcTime) const;

  static bool isValid(const String &locator);
  static void decode(const String &locator, double &latitude,
                     double &longitude);
  static void adjustCharacter(String &locator, uint8_t position,
                              int8_t direction);

 private:
  void apply(const String &locator);

  String locator_;
  double latitude_ = 0.0;
  double longitude_ = 0.0;
};
