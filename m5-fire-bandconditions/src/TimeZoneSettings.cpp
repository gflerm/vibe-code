#include "TimeZoneSettings.h"

#include <AceCommon.h>
#include <AceTime.h>
#include <Preferences.h>

#include "AppConfig.h"

namespace {
using namespace ace_time;

// The 2025 extended database contains every IANA zone and link while omitting
// historical transitions before 2025, which this live clock does not need.
ExtendedZoneProcessorCache<1> zoneProcessorCache;
ExtendedZoneManager zoneManager(
    zonedbx2025::kZoneAndLinkRegistrySize,
    zonedbx2025::kZoneAndLinkRegistry,
    zoneProcessorCache);

uint16_t sortedZoneIndexes[zonedbx2025::kZoneAndLinkRegistrySize];
bool catalogueSorted = false;

constexpr uint32_t kDefaultZoneId =
    zonedbx2025::kZoneIdAfrica_Johannesburg;

uint16_t wrapPosition(uint16_t position) {
  return position < zonedbx2025::kZoneAndLinkRegistrySize ? position : 0;
}
}  // namespace

void TimeZoneSettings::begin() {
  if (!catalogueSorted) {
    ZoneSorterByName<ExtendedZoneManager> sorter(zoneManager);
    sorter.fillIndexes(sortedZoneIndexes,
                       zonedbx2025::kZoneAndLinkRegistrySize);
    sorter.sortIndexes(sortedZoneIndexes,
                       zonedbx2025::kZoneAndLinkRegistrySize);
    catalogueSorted = true;
  }

  uint32_t savedZoneId = kDefaultZoneId;
  Preferences preferences;
  if (preferences.begin(AppConfig::kPreferencesNamespace, true)) {
    savedZoneId = preferences.getUInt(AppConfig::kTimeZonePreferenceKey,
                                      kDefaultZoneId);
    preferences.end();
  }

  uint16_t registryIndex = zoneManager.indexForZoneId(savedZoneId);
  if (registryIndex == ZoneManager::kInvalidIndex) {
    registryIndex = zoneManager.indexForZoneId(kDefaultZoneId);
  }

  selectedPosition_ = 0;
  for (uint16_t i = 0; i < zoneCount(); ++i) {
    if (sortedZoneIndexes[i] == registryIndex) {
      selectedPosition_ = i;
      break;
    }
  }
}

bool TimeZoneSettings::save(uint16_t listPosition) {
  listPosition = wrapPosition(listPosition);
  const ExtendedZone zone =
      zoneManager.getZoneForIndex(sortedZoneIndexes[listPosition]);

  Preferences preferences;
  if (!preferences.begin(AppConfig::kPreferencesNamespace, false)) {
    return false;
  }
  const size_t bytesWritten = preferences.putUInt(
      AppConfig::kTimeZonePreferenceKey, zone.zoneId());
  preferences.end();
  if (bytesWritten != sizeof(uint32_t)) return false;

  selectedPosition_ = listPosition;
  return true;
}

uint16_t TimeZoneSettings::zoneCount() const {
  return zonedbx2025::kZoneAndLinkRegistrySize;
}

String TimeZoneSettings::zoneName(uint16_t listPosition) const {
  listPosition = wrapPosition(listPosition);
  ace_common::PrintStr<64> name;
  zoneManager.getZoneForIndex(sortedZoneIndexes[listPosition])
      .printNameTo(name);
  return String(name.cstr());
}

String TimeZoneSettings::selectedZoneName() const {
  return zoneName(selectedPosition_);
}

bool TimeZoneSettings::formatLocalTime(time_t unixTime, char *buffer,
                                       size_t bufferSize) const {
  if (buffer == nullptr || bufferSize == 0 || unixTime <= 0) return false;

  const TimeZone zone = zoneManager.createForZoneIndex(
      sortedZoneIndexes[selectedPosition_]);
  const ZonedDateTime dateTime =
      ZonedDateTime::forUnixSeconds64(static_cast<int64_t>(unixTime), zone);
  if (dateTime.isError()) return false;

  const ZonedExtra extra = zone.getZonedExtra(dateTime.toEpochSeconds());
  if (extra.isError()) return false;
  snprintf(buffer, bufferSize, "%s %02u:%02u:%02u", extra.abbrev(),
           dateTime.hour(), dateTime.minute(), dateTime.second());
  return true;
}
