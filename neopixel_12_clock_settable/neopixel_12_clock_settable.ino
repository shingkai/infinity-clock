#include <math.h>
#include <TinyWireM.h>
#include <USI_TWI_Master.h>
#include <DS3232RTC.h>
#include <Adafruit_NeoPixel.h>


// define pins
// DS3231 SCL to Attiny85 PB2 - SDA to Attiny85 PB0
#define HEARTBEAT_PIN 0
#define NEO_PIN 1
#define BTN_PIN 3
#define POT_PIN 2 // A2 is PB4/IC3

// define NeoPixel settings
#define NUM_PIXELS 12
#define PIXEL_FORMAT NEO_RGB + NEO_KHZ800
#define DELAYVAL 50
#define MAX_BRIGHTNESS 255
#define START_HOUR 11
#define START_MINUTE 30

enum clock_mode {Clock, SetHour, SetMinute} mode = Clock;
boolean debounce = false;
int brightness = MAX_BRIGHTNESS;

tmElements_t tm;

int hourHand[12];
int minuteHand[12];
int secondHand[12];

Adafruit_NeoPixel *pixels;

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);

  // setup NeoPixels
  pixels = new Adafruit_NeoPixel(NUM_PIXELS, NEO_PIN, PIXEL_FORMAT);
  pixels->begin();

  // initialize I2C lib
  TinyWireM.begin();

  // setup RTC
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  while (timeStatus() != timeSet) { // flash if there was an error syncing with RTC
    flash();
  }

  flash();
}

void loop() {
  // check the button and cycle the mode if its pressed
  if (!digitalRead(BTN_PIN) && !debounce) { // BTN_PIN is pulled up, drops to LOW when pressed
    cycleMode();
    debounce = true;
  } else if (digitalRead(BTN_PIN) && debounce) { // if the button is not pressed, turn off debounce
    debounce = false;
  }

  // run based on the mode of the clock
  if (mode == Clock) {
    runClock();
  } else if (mode == SetHour) {
    setHourHand();
  } else if (mode = SetMinute) {
    setMinuteHand();
  }
}

void cycleMode() {
  if (mode == Clock) {
    mode = SetHour;
  } else if (mode == SetHour) {
    mode = SetMinute;
  } else if (mode = SetMinute) {
    mode = Clock;
  }
}

void setHourHand() {
  int val = analogRead(POT_PIN);
  int setHour = map(val, 0, 1023, 0, 11);

  updateHourHand(hourHand, setHour, brightness);

  for (int i = 0; i < 12; i++) {
    pixels->setPixelColor(i, pixels->Color(hourHand[i], 0, 0));
  }
  pixels->show();

  tm.Hour = setHour;
  tm.Second = 0;
  RTC.write(tm);
}

void setMinuteHand() {
  int val = analogRead(POT_PIN);
  int setMinute = map(val, 0, 1023, 0, 59);

  updateMinSecHand(minuteHand, setMinute, brightness / 2);
  for (int i = 0; i < 12; i++) {
    pixels->setPixelColor(i, pixels->Color(0, minuteHand[i], 0));
  }
  pixels->show();

  tm.Minute = setMinute;
  tm.Second = 0;
  RTC.write(tm);
}

void setBrightness() {
  int val = analogRead(POT_PIN);
  brightness = map(val, 0, 1023, 0, 255);
}

void runClock() {
  setBrightness();
  updateHourHand(hourHand, getHours(), brightness);
  updateMinSecHand(minuteHand, getMinutes(), brightness / 2);
  updateMinSecHand(secondHand, getSeconds(), brightness / 4);

  for (int i = 0; i < 12; i++) {
    pixels->setPixelColor(i, pixels->Color(hourHand[i], minuteHand[i], secondHand[i]));
  }
  pixels->show();
}

int getSeconds() {
  RTC.read(tm);
  return tm.Second;
}

int getMinutes() {
  RTC.read(tm);
  return tm.Minute;
}

int getHours() {
  RTC.read(tm);
  return tm.Hour % 12;
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
