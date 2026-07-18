# M5Stack Fire Band Conditions Roadmap

## Recommended next feature

- [ ] Add an NCDXF/IARU beacon screen.
  - Show the beacon currently transmitting on each supported band.
  - Display its callsign, frequency, distance, and bearing from the configured Maidenhead locator.
  - Show a countdown to the next beacon transmission.
  - Include the South African ZS6DN beacon.
  - Use the beacon screen to compare predicted conditions with signals actually heard.

## Propagation tools

- [ ] Add a target-locator mode.
  - Allow entry of a destination Maidenhead locator.
  - Calculate short-path and long-path distance and bearing.
  - Display the target's local time and daylight state.
  - Suggest likely bands for communication with the target.

- [ ] Add a greyline dashboard.
  - Display countdowns to local sunrise and sunset.
  - Compare the local and target greyline windows.
  - Highlight possible 40 m, 80 m, and 160 m opportunities.

- [ ] Add observed propagation information.
  - Investigate Reverse Beacon Network and PSK Reporter data sources.
  - Summarize actual CW or digital activity by band and direction.
  - Clearly distinguish observed reports from calculated predictions.

- [ ] Add a band recommendation score.
  - Score each band using daylight, season, time, SFI, A-index, K-index, and observed reports.
  - Show likely regions or paths for each recommended band.
  - Explain the score calculation on the diagnostic screen.

## Space weather

- [ ] Add enhanced NOAA space-weather data.
  - Display the current Kp index and a short history graph.
  - Display solar-wind speed and Bz direction.
  - Display the current X-ray flare class.
  - Show geomagnetic-storm, radio-blackout, and aurora warnings.

- [ ] Add configurable operating alerts.
  - Alert when Kp rises quickly or crosses a configured threshold.
  - Alert for significant M-class or X-class flares.
  - Alert when 10 m conditions improve.
  - Alert before a greyline window begins.
  - Optionally alert for wanted callsigns or Maidenhead locators.
  - Save alert settings in non-volatile storage.

## Configuration and storage

- [ ] Add a device configuration webpage.
  - Start a temporary Wi-Fi access point using a button action.
  - Configure callsign, Maidenhead locator, timezone, Wi-Fi, preferred modes, and alert thresholds from a phone or computer.
  - Store configuration in ESP32 NVS.

- [ ] Add optional SD-card history.
  - Log solar readings, propagation data, and band scores.
  - Display 24-hour and seven-day graphs.
  - Allow exporting the history as CSV.
  - Continue storing ordinary device settings in NVS rather than requiring an SD card.

## Additional operating tools

- [ ] Add an amateur-satellite pass screen.
  - Show upcoming ISS and amateur-radio satellite passes.
  - Display AOS time, maximum elevation, azimuth, and countdown.
  - Periodically download and cache current orbital elements.

## Reference data sources

- NCDXF/IARU beacon schedule: <https://www.ncdxf.org/beacon/>
- NCDXF/IARU beacon locations: <https://www.ncdxf.org/beacon/beaconlocations.html>
- NOAA Space Weather Prediction Center data: <https://www.swpc.noaa.gov/content/data-access>
