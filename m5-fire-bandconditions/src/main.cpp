#include <Arduino.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <time.h>

#include "AppConfig.h"
#include "DisplayUi.h"
#include "NetworkTimeService.h"
#include "SolarDataService.h"
#include "StationLocation.h"
#include "TimeZoneSettings.h"
#include "secrets.h"

namespace {
enum class Screen { Dashboard, Diagnostics, LocatorEditor, TimeZoneEditor };

DisplayUi display;
NetworkTimeService networkTime;
SolarDataService solarService;
StationLocation station;
TimeZoneSettings timeZone;

Screen activeScreen = Screen::Dashboard;
String editedLocator;
uint8_t editedPosition = 0;
uint16_t editedTimeZonePosition = 0;
uint32_t timeZoneRepeatAt = 0;

// Hold/release gates prevent the press that opens a screen from also activating
// an action on that newly opened screen.
bool normalHoldHandled = false;
bool editorHoldHandled = false;
bool editorReadyForInput = false;

uint32_t lastDaylightCheckSecond = UINT32_MAX;
bool lastDaylightState = false;

void showDashboard() {
  activeScreen = Screen::Dashboard;
  lastDaylightState = station.isDaylight(time(nullptr));
  display.renderDashboard(solarService.data(), station, timeZone);
}

void openTimeZoneEditor() {
  editedTimeZonePosition = timeZone.selectedPosition();
  editorHoldHandled = false;
  editorReadyForInput = false;
  timeZoneRepeatAt = 0;
  activeScreen = Screen::TimeZoneEditor;
  display.renderTimeZoneEditor(timeZone.zoneName(editedTimeZonePosition),
                               editedTimeZonePosition, timeZone.zoneCount());
}

void moveTimeZone(int direction) {
  const int count = timeZone.zoneCount();
  editedTimeZonePosition = static_cast<uint16_t>(
      (static_cast<int>(editedTimeZonePosition) + direction + count) % count);
  display.renderTimeZoneEditor(timeZone.zoneName(editedTimeZonePosition),
                               editedTimeZonePosition, timeZone.zoneCount());
}

void handleTimeZoneEditor() {
  if (!editorReadyForInput) {
    if (!M5.BtnA.isPressed() && !M5.BtnB.isPressed() &&
        !M5.BtnC.isPressed()) {
      editorReadyForInput = true;
    }
    return;
  }

  if (M5.BtnA.pressedFor(AppConfig::kButtonHoldMs) &&
      !editorHoldHandled) {
    editorHoldHandled = true;
    showDashboard();
    return;
  }

  if (M5.BtnB.pressedFor(AppConfig::kButtonHoldMs) &&
      !editorHoldHandled) {
    editorHoldHandled = true;
    if (!timeZone.save(editedTimeZonePosition)) {
      display.showSaveFailed();
      editorHoldHandled = false;
      return;
    }
    Serial.printf("Time zone saved: %s\n",
                  timeZone.selectedZoneName().c_str());
    normalHoldHandled = true;
    showDashboard();
    return;
  }

  if (M5.BtnA.wasPressed()) {
    moveTimeZone(-1);
  } else if (M5.BtnC.wasPressed()) {
    moveTimeZone(1);
    timeZoneRepeatAt = millis() + 500;
  } else if (M5.BtnC.isPressed() && timeZoneRepeatAt != 0 &&
             static_cast<int32_t>(millis() - timeZoneRepeatAt) >= 0) {
    moveTimeZone(1);
    timeZoneRepeatAt = millis() + 90;
  }
}

void openLocatorEditor() {
  editedLocator = station.locator();
  editedPosition = 0;
  editorHoldHandled = false;
  editorReadyForInput = false;
  activeScreen = Screen::LocatorEditor;
  display.renderLocatorEditor(editedLocator, editedPosition);
}

void handleLocatorEditor() {
  // The opening B press is still down on entry. Require a complete release
  // before accepting editor controls so it cannot immediately trigger SAVE.
  if (!editorReadyForInput) {
    if (!M5.BtnA.isPressed() && !M5.BtnB.isPressed() &&
        !M5.BtnC.isPressed()) {
      editorReadyForInput = true;
    }
    return;
  }

  if (M5.BtnA.pressedFor(AppConfig::kButtonHoldMs) &&
      !editorHoldHandled) {
    editorHoldHandled = true;
    showDashboard();
    return;
  }

  if (M5.BtnB.pressedFor(AppConfig::kButtonHoldMs) &&
      !editorHoldHandled) {
    editorHoldHandled = true;
    if (!StationLocation::isValid(editedLocator) ||
        !station.save(editedLocator)) {
      display.renderLocatorEditor(editedLocator, editedPosition);
      display.showInvalidLocator();
      editorHoldHandled = false;
      return;
    }

    Serial.printf("Station saved: %s (%.4f, %.4f)\n",
                  station.locator().c_str(), station.latitude(),
                  station.longitude());
    normalHoldHandled = true;
    showDashboard();
    return;
  }

  if (M5.BtnA.wasPressed()) {
    StationLocation::adjustCharacter(editedLocator, editedPosition, -1);
    display.renderLocatorEditor(editedLocator, editedPosition);
  } else if (M5.BtnB.wasPressed()) {
    editedPosition = (editedPosition + 1) % 6;
    display.renderLocatorEditor(editedLocator, editedPosition);
  } else if (M5.BtnC.wasPressed()) {
    StationLocation::adjustCharacter(editedLocator, editedPosition, 1);
    display.renderLocatorEditor(editedLocator, editedPosition);
  }
}

void handleDiagnostics() {
  if (M5.BtnA.wasPressed()) {
    showDashboard();
  } else if (M5.BtnB.pressedFor(AppConfig::kButtonHoldMs) &&
             !normalHoldHandled) {
    normalHoldHandled = true;
    openLocatorEditor();
  } else if (M5.BtnC.wasPressed()) {
    openTimeZoneEditor();
  }

  if (!M5.BtnB.isPressed()) {
    normalHoldHandled = false;
  }
}

void handleDashboard() {
  if (M5.BtnA.wasPressed()) {
    activeScreen = Screen::Diagnostics;
    display.renderDiagnostics(station);
    return;
  }

  if (M5.BtnB.pressedFor(AppConfig::kButtonHoldMs) &&
      !normalHoldHandled) {
    normalHoldHandled = true;
    openLocatorEditor();
    return;
  }
  if (!M5.BtnB.isPressed()) {
    normalHoldHandled = false;
  }

  if (!networkTime.wifiConnected() || !networkTime.clockSynchronized()) return;

  display.updateClock(timeZone);

  // Redraw only when the Sun actually crosses the horizon, not on every clock
  // tick. This keeps the display stable and avoids visible flicker.
  const uint32_t currentSecond = static_cast<uint32_t>(time(nullptr));
  if (currentSecond != lastDaylightCheckSecond) {
    lastDaylightCheckSecond = currentSecond;
    const bool daylight = station.isDaylight(time(nullptr));
    if (daylight != lastDaylightState) {
      showDashboard();
    }
  }

  if (solarService.refreshDue()) {
    solarService.fetch();
    showDashboard();
  }
}
}  // namespace

void setup() {
  M5.begin();
  Serial.begin(115200);

  station.begin();
  Serial.printf("Station: %s (%.4f, %.4f)\n", station.locator().c_str(),
                station.latitude(), station.longitude());

  timeZone.begin();
  Serial.printf("Time zone: %s\n", timeZone.selectedZoneName().c_str());

  display.showStatus("Connecting to WiFi", WIFI_SSID);
  if (!networkTime.connectWifi(WIFI_SSID, WIFI_PASSWORD)) {
    display.showStatus("WiFi failed", "Restart to retry");
    return;
  }

  display.showStatus("WiFi connected", WiFi.localIP().toString());
  delay(1000);  // Give DHCP and the gateway a moment to settle.

  display.showStatus("Syncing time", "192.168.8.10:123");
  if (!networkTime.synchronizeClock()) {
    display.showStatus("Time sync failed", "No NTP response");
    return;
  }

  display.showStatus("Loading band data", "N0NBH");
  solarService.fetch();
  showDashboard();
}

void loop() {
  M5.update();

  switch (activeScreen) {
    case Screen::LocatorEditor:
      handleLocatorEditor();
      break;
    case Screen::Diagnostics:
      handleDiagnostics();
      break;
    case Screen::TimeZoneEditor:
      handleTimeZoneEditor();
      break;
    case Screen::Dashboard:
      handleDashboard();
      break;
  }

  delay(20);
}
