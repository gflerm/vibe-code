#include "StationLocation.h"

#include <Preferences.h>
#include <math.h>

#include "AppConfig.h"

namespace {
constexpr double kPi = 3.14159265358979323846;
}

void StationLocation::begin() {
  String saved = AppConfig::kDefaultLocator;

  Preferences preferences;
  if (preferences.begin(AppConfig::kPreferencesNamespace, true)) {
    saved = preferences.getString(AppConfig::kLocatorPreferenceKey,
                                  AppConfig::kDefaultLocator);
    preferences.end();
  }

  apply(isValid(saved) ? saved : String(AppConfig::kDefaultLocator));
}

bool StationLocation::save(const String &locator) {
  if (!isValid(locator)) {
    return false;
  }

  Preferences preferences;
  if (!preferences.begin(AppConfig::kPreferencesNamespace, false)) {
    return false;
  }
  const size_t bytesWritten =
      preferences.putString(AppConfig::kLocatorPreferenceKey, locator);
  preferences.end();
  if (bytesWritten == 0) {
    return false;
  }
  apply(locator);
  return true;
}

bool StationLocation::isValid(const String &locator) {
  if (locator.length() != 6) {
    return false;
  }

  const char fieldLon = toupper(locator[0]);
  const char fieldLat = toupper(locator[1]);
  const char squareLon = locator[2];
  const char squareLat = locator[3];
  const char subLon = tolower(locator[4]);
  const char subLat = tolower(locator[5]);
  return fieldLon >= 'A' && fieldLon <= 'R' && fieldLat >= 'A' &&
         fieldLat <= 'R' && squareLon >= '0' && squareLon <= '9' &&
         squareLat >= '0' && squareLat <= '9' && subLon >= 'a' &&
         subLon <= 'x' && subLat >= 'a' && subLat <= 'x';
}

void StationLocation::decode(const String &locator, double &latitude,
                             double &longitude) {
  const int fieldLon = toupper(locator[0]) - 'A';
  const int fieldLat = toupper(locator[1]) - 'A';
  const int squareLon = locator[2] - '0';
  const int squareLat = locator[3] - '0';
  const int subLon = tolower(locator[4]) - 'a';
  const int subLat = tolower(locator[5]) - 'a';

  // A six-character locator describes an area. Use its center so solar
  // calculations do not have a systematic south-west corner bias.
  longitude = -180.0 + fieldLon * 20.0 + squareLon * 2.0 +
              (subLon + 0.5) / 12.0;
  latitude = -90.0 + fieldLat * 10.0 + squareLat +
             (subLat + 0.5) / 24.0;
}

void StationLocation::adjustCharacter(String &locator, uint8_t position,
                                      int8_t direction) {
  if (locator.length() != 6 || position >= 6) {
    return;
  }

  char first;
  char last;
  if (position < 2) {
    first = 'A';
    last = 'R';
  } else if (position < 4) {
    first = '0';
    last = '9';
  } else {
    first = 'a';
    last = 'x';
  }

  char value = locator[position] + direction;
  if (value > last) value = first;
  if (value < first) value = last;
  locator.setCharAt(position, value);
}

double StationLocation::solarElevationDegrees(time_t utcTime) const {
  tm utc = {};
  gmtime_r(&utcTime, &utc);

  // NOAA's compact solar-position approximation. Accuracy is comfortably
  // inside what is needed to decide whether the Sun is above the horizon.
  const double hour =
      utc.tm_hour + utc.tm_min / 60.0 + utc.tm_sec / 3600.0;
  const double gamma = 2.0 * kPi / 365.0 *
                       (utc.tm_yday + (hour - 12.0) / 24.0);
  const double equationOfTime =
      229.18 * (0.000075 + 0.001868 * cos(gamma) -
                0.032077 * sin(gamma) - 0.014615 * cos(2.0 * gamma) -
                0.040849 * sin(2.0 * gamma));
  const double declination =
      0.006918 - 0.399912 * cos(gamma) + 0.070257 * sin(gamma) -
      0.006758 * cos(2.0 * gamma) + 0.000907 * sin(2.0 * gamma) -
      0.002697 * cos(3.0 * gamma) + 0.00148 * sin(3.0 * gamma);

  double solarMinutes = hour * 60.0 + equationOfTime + 4.0 * longitude_;
  while (solarMinutes < 0.0) solarMinutes += 1440.0;
  while (solarMinutes >= 1440.0) solarMinutes -= 1440.0;

  const double hourAngle = (solarMinutes / 4.0 - 180.0) * kPi / 180.0;
  const double latitudeRadians = latitude_ * kPi / 180.0;
  double elevationTerm = sin(latitudeRadians) * sin(declination) +
                         cos(latitudeRadians) * cos(declination) *
                             cos(hourAngle);
  if (elevationTerm > 1.0) elevationTerm = 1.0;
  if (elevationTerm < -1.0) elevationTerm = -1.0;
  return asin(elevationTerm) * 180.0 / kPi;
}

bool StationLocation::isDaylight(time_t utcTime) const {
  return solarElevationDegrees(utcTime) > 0.0;
}

void StationLocation::apply(const String &locator) {
  locator_ = locator;
  decode(locator_, latitude_, longitude_);
}
