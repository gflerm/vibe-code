# ESP32 Blink with PlatformIO (ESP-IDF)

This PlatformIO project blinks the RGB LED connected to GPIO 48 every 500 ms.

## Build and upload

Connect the ESP32, then run:

```sh
pio run
pio run --target upload
```


You can also change `BLINK_INTERVAL_MS` to adjust the blink speed.