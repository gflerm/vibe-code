#pragma once

#include <Arduino.h>

struct SolarData {
  String solarFlux = "--";
  String sunspots = "--";
  String aIndex = "--";
  String kIndex = "--";
  String updated = "No data";
  String day[4] = {"--", "--", "--", "--"};
  String night[4] = {"--", "--", "--", "--"};
  bool valid = false;
};

// Downloads and parses N0NBH conditions. Failed refreshes never replace the
// last valid dataset, keeping the dashboard useful during brief outages.
class SolarDataService {
 public:
  bool fetch();
  bool refreshDue() const;

  const SolarData &data() const { return data_; }

  static const char *bandName(size_t index);

 private:
  static String xmlValue(const String &xml, const String &tag);
  static String bandCondition(const String &xml, const char *band,
                              const char *period);
  static bool parse(const String &xml, SolarData &result);

  SolarData data_;
  uint32_t lastAttemptMs_ = 0;
};
