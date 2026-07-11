#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define RGB_LED_PIN 48
#define NUM_RGB_LEDS 1
#define STEP_DELAY_MS 300
#define LED_BRIGHTNESS 20

Adafruit_NeoPixel rgbLed(NUM_RGB_LEDS, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

void showColor(uint32_t color) {
  rgbLed.setPixelColor(0, color);
  rgbLed.show();
  delay(STEP_DELAY_MS);
}

void setup() {
  rgbLed.begin();
  rgbLed.setBrightness((LED_BRIGHTNESS * 255 + 1) / 100);
  rgbLed.clear();
  rgbLed.show();
}

void loop() {
  showColor(rgbLed.Color(255, 0, 0));
  showColor(rgbLed.Color(0, 255, 0));
  showColor(rgbLed.Color(0, 0, 255));
  showColor(rgbLed.Color(0, 0, 0));
}
