#include <USI_TWI_Master.h>

#include <TinyI2CMaster.h>


const int rtcAddr = 0x68;
const int ledPin = 1;

void setup() {
  pinMode(ledPin, OUTPUT);

  blink(5);

  // put your setup code here, to run once:
  TinyI2C.init();
  TinyI2C.start(rtcAddr, 0);
  TinyI2C.write(0);
  TinyI2C.write(0);
  TinyI2C.write(0x12);
  TinyI2C.write(0x34);
  TinyI2C.stop();

  blink(3);
}

void loop() {

  // put your main code here, to run repeatedly:
  TinyI2C.start(rtcAddr, 0);
  TinyI2C.write(0);
  TinyI2C.restart(rtcAddr, 1);
  int secondsBCD = TinyI2C.read();
  TinyI2C.stop();
  //
  //  TinyWireM.beginTransmission(rtcAddr);
  //  TinyWireM.write(0);
  //  TinyWireM.endTransmission();
  //
  //  TinyWireM.requestFrom(rtcAddr, 1);
  //  int seconds = TinyWireM.read();

  int seconds = (secondsBCD>>4)*10 + (secondsBCD & 0x0F);

  blink(seconds);
  
  delay(1000);
}

void blink(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}
