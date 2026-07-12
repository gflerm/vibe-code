# ESP32 Blink with PlatformIO (Arduino)
This PlatformIO project blinks the RGB LED connected to GPIO 48 every 300 ms.

## Build and upload
Connect the ESP32, then run:

```sh
pio run
pio run --target upload
```

If your board uses a different LED pin, change LED_PIN in platformio.ini. You can also change BLINK_INTERVAL_MS to adjust the blink speed.
