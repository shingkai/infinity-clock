// Infinity Clock
// Mike Chiu 2021

#include <math.h>
#include <TinyI2CMaster.h>
#include <Adafruit_NeoPixel.h>


// define pins
// DS3231 SCL to Attiny85 PB2 - SDA to Attiny85 PB0
#define NEO_PIN 1
#define BTN_PIN 3
#define POT_PIN 2 // A2 is PB4/IC3

// define NeoPixel settings
#define NUM_PIXELS 12
#define PIXEL_FORMAT NEO_RGB + NEO_KHZ800

// define RTC address
const int rtcAddr = 0x68;

enum clock_mode {Clock, SetHour, SetMinute} mode = Clock;
boolean debounce = false;
int brightness = 255; // start with brightness at max

int hourHand[12];
int minuteHand[12];
int secondHand[12];

Adafruit_NeoPixel *pixels;

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);

  // setup NeoPixels
  pixels = new Adafruit_NeoPixel(NUM_PIXELS, NEO_PIN, PIXEL_FORMAT);
  pixels->begin();

  // setup tiny i2c
  TinyI2C.init();
//  TinyI2C.start(rtcAddr, 0);
//  TinyI2C.write(0); // register select: seconds
//  TinyI2C.write(0); // set seconds
//  TinyI2C.write(0); // set minutes
//  TinyI2C.write(0); // set hours
//  TinyI2C.stop();

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
  delay(100);
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

  uint8_t MASK_12_HOUR = 1 << 6; // 0100 0000, bit 6 HIGH sets to 12-hour mode, bit 5 LOW sets to AM
  uint8_t hourCode = ((setHour / 10) << 4) | (setHour % 10);// | MASK_12_HOUR;

  TinyI2C.start(rtcAddr, 0);
  TinyI2C.write(2);
  TinyI2C.write(hourCode);
  TinyI2C.stop();
}

void setMinuteHand() {
  int val = analogRead(POT_PIN);
  int setMinute = map(val, 0, 1023, 0, 59);

  updateMinSecHand(minuteHand, setMinute, brightness / 2);
  
  for (int i = 0; i < 12; i++) {
    pixels->setPixelColor(i, pixels->Color(0, minuteHand[i], 0));
  }
  pixels->show();

  uint8_t minuteCode = ((setMinute / 10) << 4) | (setMinute % 10);

  TinyI2C.start(rtcAddr, 0);
  TinyI2C.write(0); // 0x00h is the seconds register
  TinyI2C.write(0); // reset seconds
  TinyI2C.write(minuteCode); // set minutes
  TinyI2C.stop();
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
  TinyI2C.start(rtcAddr, 0);
  TinyI2C.write(0); // seconds are stored in register 0x00h
  TinyI2C.restart(rtcAddr, 1);
  int secondsBCD = TinyI2C.read();
  TinyI2C.stop();

  int seconds = (secondsBCD >> 4)*10 + (secondsBCD & 0x0F);
  return seconds;
}

int getMinutes() {
  TinyI2C.start(rtcAddr, 0);
  TinyI2C.write(1); // minutes are stored in register 0x01h
  TinyI2C.restart(rtcAddr, 1);
  int minutesBCD = TinyI2C.read();
  TinyI2C.stop();

  int minutes = (minutesBCD >> 4)*10 + (minutesBCD & 0x0F);
  return minutes;
}

int getHours() {
  TinyI2C.start(rtcAddr, 0);
  TinyI2C.write(2); // hours are stored in register 0x02h
  TinyI2C.restart(rtcAddr, 1);
  int hoursBCD = TinyI2C.read();
  TinyI2C.stop();

  int hours = (hoursBCD >> 4)*10 + (hoursBCD & 0x0F);
  return hours;
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
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels->setPixelColor(i, pixels->Color(20, 20, 20));
  }
  pixels->show();
  delay(1000);

  pixels->clear();
  pixels->show();
  delay(1000);
}
