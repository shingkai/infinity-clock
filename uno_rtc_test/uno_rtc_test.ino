#include <Wire.h>

const int rtcAddr = 0x68;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);

  Serial.println("setting RTC");

  Wire.beginTransmission(rtcAddr);
  Wire.write(0); // set the register to write, seconds
  Wire.write(0x00); // seconds
  Wire.write(0x30); // minutes
  Wire.write(0x11); // hours
  Wire.endTransmission();
}

void loop() {
  // put your main code here, to run repeatedly:

  Wire.beginTransmission(rtcAddr);
  Wire.write(0); // set the register to write, seconds
  Wire.endTransmission();

  Wire.requestFrom(rtcAddr, 3);
  int seconds = Wire.read();
  int minutes = Wire.read();
  int hours = Wire.read();

  Serial.print(hours, HEX);
  Serial.print(":");
  Serial.print(minutes, HEX);
  Serial.print(":");
  Serial.println(seconds, HEX);

  //  Serial.print((hours >> 4) && 0x01);
  //  Serial.print(hours && 0x0F);
  //  Serial.print(":");
  //  Serial.print(minutes >> 4);
  //  Serial.print(minutes && 0x0F);
  //  Serial.print(":");
  //  Serial.print(seconds >> 4);
  //  Serial.println(seconds && 0x0F);

  delay(100);
}
