#include <Arduino.h>

const uint16_t kBellEnablePin = 26;
const uint16_t kLightEnablePin = 27;
void setup() {
  // put your setup code here, to run once:
  pinMode(kBellEnablePin, OUTPUT);
  pinMode(kLightEnablePin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // digitalWrite(kBellEnablePin, true);
  digitalWrite(kLightEnablePin, true);
  delay(50);
  // digitalWrite(kBellEnablePin, false);
  digitalWrite(kLightEnablePin, false);
  delay(100);
}