# M5Stack Fire Band Conditions

A PlatformIO Arduino project that connects an M5Stack Fire to Wi-Fi and displays
local time together with live HF band conditions from the N0NBH feed. It first
requests time from `xxx.xxx.xxx.xxx:123`, then falls back to `pool.ntp.org` servers
on UDP port `123`. Band data refreshes every 15 minutes, preserving the last
successful update if a refresh fails.

The default station location is Maidenhead locator `JF95eu` (approximately
latitude -34.1458, longitude 18.3750), and the active locator is shown in the
display footer.

Press the left button from the dashboard to open diagnostics. This screen shows
the decoded coordinates, solar elevation, and current DAY/NIGHT decision. The
left button returns, holding the middle button opens the locator editor, and
the right button opens the timezone editor.

Hold the middle button to edit the locator on the device. In the editor, the
left and right buttons decrement or increment the selected character, and the
middle button advances to the next character. Hold the middle button to save
the locator in non-volatile memory, or hold the left button to cancel. The
calculated station position highlights the currently applicable DAY or NIGHT
conditions column. The editor restricts each position to the legal Maidenhead
ranges and validates the complete locator again before saving it.

The timezone editor lists all 597 IANA zones and aliases bundled with AceTime.
Use the left and right buttons to move through the alphabetical list, or hold
the right button to scroll quickly. Hold the middle button to save, or hold the
left button to cancel. The stable IANA zone ID is stored in ESP32 non-volatile
memory and restored after reboot; daylight-saving changes are applied
automatically. The timezone catalogue lives in firmware flash, so no SD card is
required.

Wi-Fi credentials are stored in the ignored `include/secrets.h` file. Copy
`include/secrets.example.h` when setting the project up on another machine.

## Project structure

- `src/main.cpp` coordinates screens, buttons, startup, and refresh events.
- `StationLocation` validates, stores, and decodes Maidenhead locators and
  calculates solar elevation.
- `NetworkTimeService` connects Wi-Fi and synchronizes the system clock using
  the local NTP server with pool fallbacks.
- `SolarDataService` downloads, parses, caches, and schedules N0NBH data.
- `DisplayUi` owns every dashboard, diagnostics, editor, and status layout.
- `TimeZoneSettings` provides the IANA catalogue, local-time conversion, and
  persistent timezone selection.
- `AppConfig.h` contains shared constants and application policy.

## Build and upload

Connect the M5Stack Fire over USB, then run:

```powershell
pio run
pio run --target upload
pio device monitor
```

The serial monitor runs at 115200 baud. If PlatformIO cannot identify the
upload port automatically, add `upload_port = COMx` to `platformio.ini`, using
the COM port shown by `pio device list`.
