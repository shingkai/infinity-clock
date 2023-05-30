const int ledPin = 1;

void setup() {
  pinMode(ledPin, OUTPUT);

  blink(5);
}

void loop() {
  delay(3000);
  blink(2);
}



void blink(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}
