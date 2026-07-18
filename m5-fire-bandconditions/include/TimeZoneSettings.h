#pragma once

#include <Arduino.h>
#include <time.h>

// Owns the complete IANA timezone catalogue supplied by AceTime. The catalogue
// lives in program flash; only the selected stable zone ID is stored in NVS.
class TimeZoneSettings {
 public:
  void begin();
  bool save(uint16_t listPosition);

  uint16_t selectedPosition() const { return selectedPosition_; }
  uint16_t zoneCount() const;
  String zoneName(uint16_t listPosition) const;
  String selectedZoneName() const;

  // Formats Unix time using the selected zone, including its current DST-aware
  // abbreviation (for example SAST, GMT, or BST).
  bool formatLocalTime(time_t unixTime, char *buffer, size_t bufferSize) const;

 private:
  uint16_t selectedPosition_ = 0;
};
