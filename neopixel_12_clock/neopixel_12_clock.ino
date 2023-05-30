#include <Adafruit_NeoPixel.h>
#include <math.h>

#define HEARTBEAT_PIN 0
#define NEO_PIN 1
#define NUM_PIXELS 12
#define PIXEL_FORMAT NEO_RGB + NEO_KHZ800
#define DELAYVAL 50
#define BRIGHTNESS 255
#define START_HOUR 11
#define START_MINUTE 30

int hourHand[12];
int minuteHand[12];
int secondHand[12];

Adafruit_NeoPixel *pixels;

void setup() {
  pixels = new Adafruit_NeoPixel(NUM_PIXELS, NEO_PIN, PIXEL_FORMAT);
  pixels->begin();

  flash();
}

void loop() {
  updateHourHand(hourHand, getHours() + 1, BRIGHTNESS);
  updateMinSecHand(minuteHand, getMinutes(), BRIGHTNESS / 2);
  updateMinSecHand(secondHand, getSeconds(), BRIGHTNESS / 4);

  for (int i = 0; i < 12; i++) {
    pixels->setPixelColor(i, pixels->Color(hourHand[i], minuteHand[i], secondHand[i]));
  }
  pixels->show();
}

int getSeconds() {
  return (millis() / 1000) % 60;
}

int getMinutes() {
  return (millis() / 1000 / 60) % 60;
}

int getHours() {
  return (millis() / 1000 / 60 / 60) % 12;
}


void updateHourHand(int *hourHand, int hour, int brightness) {
  for (int i = 0; i < 12; i++) {
    hourHand[(hour - i + 12) % 12] = brightness / pow(2, i);
  }
}

void updateMinSecHand(int *minSecHand, int minSec, int brightness) {

  int floorMinSec = minSec / 5; // 12 pixels = 60 minSec / 5
  int remainderMinSecs = minSec % 5;

  for (int i = 0; i < 12; i++) {
    int clockIndex = (floorMinSec - i + 12) % 12;
    int pixelValue = brightness / pow(2, i);
    minSecHand[clockIndex] = brightness / pow(2, i);

    // smooth out transitions by fading out
    minSecHand[clockIndex] -= remainderMinSecs * pixelValue / 10;
  }
  // start fading into the next pixel
  minSecHand[(floorMinSec + 1) % 12] += remainderMinSecs * brightness / 10;

  return minSecHand;
}

void flash() {
  digitalWrite(HEARTBEAT_PIN, HIGH);
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels->setPixelColor(i, pixels->Color(20, 20, 20));
  }
  pixels->show();
  delay(1000);

  digitalWrite(HEARTBEAT_PIN, LOW);
  pixels->clear();
  pixels->show();
  delay(1000);
}
