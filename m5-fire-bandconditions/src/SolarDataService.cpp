#include "SolarDataService.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "AppConfig.h"

namespace {
const char *const kBandNames[] = {
    "80m-40m",
    "30m-20m",
    "17m-15m",
    "12m-10m",
};
}

bool SolarDataService::fetch() {
  lastAttemptMs_ = millis();
  Serial.println("Fetching band conditions from N0NBH");

  WiFiClientSecure client;
  client.setInsecure();  // The small device does not maintain a CA store.

  HTTPClient http;
  http.setConnectTimeout(10000);
  http.setTimeout(10000);
  if (!http.begin(client, AppConfig::kSolarDataUrl)) {
    Serial.println("Unable to initialize solar-data request");
    return false;
  }

  bool success = false;
  const int statusCode = http.GET();
  if (statusCode == HTTP_CODE_OK) {
    SolarData parsed;
    success = parse(http.getString(), parsed);
    if (success) {
      data_ = parsed;
    }
    Serial.println(success ? "Band conditions updated"
                           : "Unable to parse band conditions");
  } else {
    Serial.printf("Band-condition request failed: HTTP %d\n", statusCode);
  }
  http.end();
  return success;
}

bool SolarDataService::refreshDue() const {
  const uint32_t interval =
      data_.valid ? AppConfig::kSolarRefreshMs : AppConfig::kSolarRetryMs;
  return millis() - lastAttemptMs_ >= interval;
}

const char *SolarDataService::bandName(size_t index) {
  return index < 4 ? kBandNames[index] : "";
}

String SolarDataService::xmlValue(const String &xml, const String &tag) {
  const String opening = "<" + tag + ">";
  const String closing = "</" + tag + ">";
  const int start = xml.indexOf(opening);
  if (start < 0) return "";

  const int contentStart = start + opening.length();
  const int end = xml.indexOf(closing, contentStart);
  if (end < 0) return "";

  String value = xml.substring(contentStart, end);
  value.trim();
  return value;
}

String SolarDataService::bandCondition(const String &xml, const char *band,
                                       const char *period) {
  const String opening =
      String("<band name=\"") + band + "\" time=\"" + period + "\">";
  const int start = xml.indexOf(opening);
  if (start < 0) return "--";

  const int contentStart = start + opening.length();
  const int end = xml.indexOf("</band>", contentStart);
  if (end < 0) return "--";

  String value = xml.substring(contentStart, end);
  value.trim();
  return value;
}

bool SolarDataService::parse(const String &xml, SolarData &result) {
  result.solarFlux = xmlValue(xml, "solarflux");
  result.sunspots = xmlValue(xml, "sunspots");
  result.aIndex = xmlValue(xml, "aindex");
  result.kIndex = xmlValue(xml, "kindex");
  result.updated = xmlValue(xml, "updated");

  if (result.solarFlux.isEmpty() || result.sunspots.isEmpty() ||
      result.aIndex.isEmpty() || result.kIndex.isEmpty()) {
    return false;
  }

  for (size_t i = 0; i < 4; ++i) {
    result.day[i] = bandCondition(xml, kBandNames[i], "day");
    result.night[i] = bandCondition(xml, kBandNames[i], "night");
  }
  result.valid = true;
  return true;
}
