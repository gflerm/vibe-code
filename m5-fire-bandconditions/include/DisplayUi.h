#pragma once

#include <Arduino.h>

#include "SolarDataService.h"
#include "StationLocation.h"
#include "TimeZoneSettings.h"

// Contains all LCD layout code. Application logic selects a screen; this class
// is responsible only for rendering it consistently.
class DisplayUi {
 public:
  void showStatus(const String &title, const String &detail,
                  uint8_t progressPercent);
  void renderDashboard(const SolarData &solar,
                       const StationLocation &station,
                       const TimeZoneSettings &timeZone);
  void renderDiagnostics(const StationLocation &station);
  void renderLocatorEditor(const String &locator, uint8_t selectedPosition);
  void renderTimeZoneEditor(const String &zoneName, uint16_t position,
                            uint16_t count);
  void showInvalidLocator();
  void showSaveFailed();
  void showShutdownMessage();
  void updateClock(const TimeZoneSettings &timeZone, bool force = false);

 private:
  static void drawCentered(const String &text, int y, uint8_t size,
                           uint16_t color);
  static uint16_t conditionColor(const String &condition);

  uint32_t lastDisplayedSecond_ = UINT32_MAX;
};
