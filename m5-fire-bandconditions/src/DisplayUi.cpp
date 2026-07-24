#include "DisplayUi.h"

#include <M5Stack.h>
#include <time.h>

void DisplayUi::drawCentered(const String &text, int y, uint8_t size,
                             uint16_t color) {
  M5.Lcd.setTextSize(size);
  M5.Lcd.setTextColor(color, BLACK);
  const int x = max(0, (M5.Lcd.width() - M5.Lcd.textWidth(text)) / 2);
  M5.Lcd.setCursor(x, y);
  M5.Lcd.print(text);
}

uint16_t DisplayUi::conditionColor(const String &condition) {
  if (condition.equalsIgnoreCase("Good")) return GREEN;
  if (condition.equalsIgnoreCase("Fair")) return YELLOW;
  if (condition.equalsIgnoreCase("Poor")) return RED;
  return LIGHTGREY;
}

void DisplayUi::showStatus(const String &title, const String &detail,
                           uint8_t progressPercent) {
  M5.Lcd.fillScreen(BLACK);
  drawCentered(title, 45, 2, WHITE);
  if (!detail.isEmpty()) {
    drawCentered(detail, 80, 2, CYAN);
  }

  constexpr int kBarX = 30;
  constexpr int kBarY = 120;
  constexpr int kBarWidth = 260;
  constexpr int kBarHeight = 18;
  constexpr int kBarInset = 3;
  const uint8_t progress =
      progressPercent > 100 ? 100 : progressPercent;
  const int fillWidth =
      ((kBarWidth - (2 * kBarInset)) * progress) / 100;

  M5.Lcd.drawRect(kBarX, kBarY, kBarWidth, kBarHeight, LIGHTGREY);
  if (fillWidth > 0) {
    M5.Lcd.fillRect(kBarX + kBarInset, kBarY + kBarInset, fillWidth,
                    kBarHeight - (2 * kBarInset), CYAN);
  }
  drawCentered(String(progress) + "%", 151, 2, WHITE);
}

void DisplayUi::updateClock(const TimeZoneSettings &timeZone, bool force) {
  const uint32_t currentSecond = static_cast<uint32_t>(time(nullptr));
  if (!force && currentSecond == lastDisplayedSecond_) return;

  char timeBuffer[24];
  if (!timeZone.formatLocalTime(time(nullptr), timeBuffer,
                                sizeof(timeBuffer))) return;
  lastDisplayedSecond_ = currentSecond;
  M5.Lcd.fillRect(0, 21, 320, 27, BLACK);
  drawCentered(timeBuffer, 22, 3, WHITE);
}

void DisplayUi::renderDashboard(const SolarData &solar,
                                const StationLocation &station,
                                const TimeZoneSettings &timeZone) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextWrap(false);

  drawCentered("BAND CONDITIONS", 3, 2, CYAN);
  updateClock(timeZone, true);

  const String indices = "SFI " + solar.solarFlux + " SN " + solar.sunspots +
                         " A " + solar.aIndex + " K " + solar.kIndex;
  drawCentered(indices, 52, 2, GREEN);

  M5.Lcd.drawFastHLine(7, 73, 306, DARKGREY);
  const bool daylight = station.isDaylight(time(nullptr));
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(daylight ? YELLOW : LIGHTGREY, BLACK);
  M5.Lcd.setCursor(172, 79);
  M5.Lcd.print("DAY");
  M5.Lcd.setTextColor(daylight ? LIGHTGREY : YELLOW, BLACK);
  M5.Lcd.setCursor(250, 79);
  M5.Lcd.print("NIGHT");

  for (size_t i = 0; i < 4; ++i) {
    const int y = 96 + static_cast<int>(i) * 24;
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(8, y);
    M5.Lcd.print(SolarDataService::bandName(i));

    M5.Lcd.setTextColor(conditionColor(solar.day[i]), BLACK);
    M5.Lcd.setCursor(163, y);
    M5.Lcd.print(solar.day[i]);
    M5.Lcd.setTextColor(conditionColor(solar.night[i]), BLACK);
    M5.Lcd.setCursor(243, y);
    M5.Lcd.print(solar.night[i]);
  }

  M5.Lcd.drawFastHLine(7, 194, 306, DARKGREY);
  const String updateText =
      solar.valid ? "Updated " + solar.updated : "Band data unavailable";
  drawCentered(updateText, 201, 1, solar.valid ? LIGHTGREY : RED);
  drawCentered("QTH " + station.locator() + " | data via N0NBH", 220, 1,
               CYAN);
}

void DisplayUi::renderDiagnostics(const StationLocation &station) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextWrap(false);
  drawCentered("DIAGNOSTICS", 5, 2, CYAN);
  drawCentered("QTH " + station.locator(), 29, 3, GREEN);

  char line[48];
  snprintf(line, sizeof(line), "LAT %+.4f", station.latitude());
  drawCentered(line, 68, 2, WHITE);
  snprintf(line, sizeof(line), "LON %+.4f", station.longitude());
  drawCentered(line, 93, 2, WHITE);

  const double elevation = station.solarElevationDegrees(time(nullptr));
  snprintf(line, sizeof(line), "SUN %+.1f deg", elevation);
  drawCentered(line, 121, 2, YELLOW);
  drawCentered(elevation > 0.0 ? "DAY" : "NIGHT", 151, 3,
               elevation > 0.0 ? YELLOW : CYAN);

  M5.Lcd.drawFastHLine(0, 204, 320, DARKGREY);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(CYAN, BLACK);
  M5.Lcd.setCursor(4, 217);
  M5.Lcd.print("A BACK");
  M5.Lcd.setCursor(124, 217);
  M5.Lcd.print("B EDIT");
  M5.Lcd.setCursor(244, 217);
  M5.Lcd.print("C TZ");
}

void DisplayUi::renderLocatorEditor(const String &locator,
                                    uint8_t selectedPosition) {
  M5.Lcd.fillScreen(BLACK);
  drawCentered("SET MAIDENHEAD", 12, 2, CYAN);
  drawCentered("A-R  A-R  0-9  0-9  a-x  a-x", 42, 1, LIGHTGREY);

  constexpr int kStartX = 43;
  constexpr int kCellWidth = 39;
  for (uint8_t i = 0; i < 6; ++i) {
    const int x = kStartX + i * kCellWidth;
    const bool selected = i == selectedPosition;
    M5.Lcd.drawRect(x, 70, 31, 43, selected ? YELLOW : DARKGREY);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(selected ? YELLOW : WHITE, BLACK);
    M5.Lcd.setCursor(x + 7, 79);
    M5.Lcd.print(locator[i]);
  }

  double latitude = 0.0;
  double longitude = 0.0;
  StationLocation::decode(locator, latitude, longitude);
  char coordinates[48];
  snprintf(coordinates, sizeof(coordinates), "Lat %.4f  Lon %.4f", latitude,
           longitude);
  drawCentered(coordinates, 132, 1, GREEN);
  drawCentered("Hold B: SAVE    Hold A: CANCEL", 158, 1, LIGHTGREY);

  M5.Lcd.drawFastHLine(0, 205, 320, DARKGREY);
  drawCentered("-             NEXT             +", 218, 1, CYAN);
}

void DisplayUi::showInvalidLocator() {
  drawCentered("INVALID MAIDENHEAD", 181, 2, RED);
}

void DisplayUi::renderTimeZoneEditor(const String &zoneName,
                                     uint16_t position, uint16_t count) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextWrap(false);
  drawCentered("SET TIME ZONE", 12, 2, CYAN);
  drawCentered(zoneName, 58, zoneName.length() > 25 ? 1 : 2, GREEN);
  drawCentered(String(position + 1) + " / " + String(count), 96, 2, WHITE);

  drawCentered("Hold C: FAST SCROLL", 136, 1, LIGHTGREY);
  drawCentered("Hold B: SAVE    Hold A: CANCEL", 158, 1, LIGHTGREY);
  M5.Lcd.drawFastHLine(0, 205, 320, DARKGREY);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(CYAN, BLACK);
  M5.Lcd.setCursor(4, 217);
  M5.Lcd.print("A PREV");
  M5.Lcd.setCursor(124, 217);
  M5.Lcd.print("B SAVE");
  M5.Lcd.setCursor(244, 217);
  M5.Lcd.print("C NEXT");
}

void DisplayUi::showSaveFailed() {
  drawCentered("SAVE FAILED", 181, 2, RED);
}
