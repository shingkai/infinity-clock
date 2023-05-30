#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define HEARTBEAT_PIN 0
#define NEO_PIN 1
#define NUMPIXELS 12

Adafruit_NeoPixel pixels(NUMPIXELS, NEO_PIN, NEO_RGB + NEO_KHZ800);
#define DELAYVAL 50
#define SLEEPVAL 1000

static const uint32_t colors[NUMPIXELS] = {
  pixels.Color(255, 0, 0),
  pixels.Color(191, 64, 0),
  pixels.Color(127, 128, 0),
  pixels.Color(63, 192, 0),
  pixels.Color(0, 255, 0),
  pixels.Color(0, 191, 64),
  pixels.Color(0, 127, 128),
  pixels.Color(0, 63, 192),
  pixels.Color(0, 0, 255),
  pixels.Color(64, 0, 191),
  pixels.Color(128, 0, 127),
  pixels.Color(192, 0, 63)
};

void setup() {
  pixels.begin();
}

void loop() {
  for (int color_offset = 0; color_offset < NUMPIXELS; color_offset++) {
    for (int pixel = 0; pixel < NUMPIXELS; pixel++) {
      pixels.setPixelColor(pixel, colors[(color_offset + pixel) % NUMPIXELS]);
    }
    pixels.show();
    delay(DELAYVAL);
  }

  //  for (int offset = 0; offset < NUMPIXELS; offset++) {
  //    for (int i = 0; i < NUMPIXELS; i++) {
  //        pixels.setPixelColor(i, colors[(i+offset) % NUMPIXELS]);
  //        pixels.show();
  //        delay(DELAYVAL);
  //    }
  //  }
}
