#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(25, HIGH);
  delay(2500); // this speeds up the simulation
  digitalWrite(25, LOW);
  digitalWrite(26, HIGH);
  delay(1500);
  digitalWrite(26, LOW);
  digitalWrite(27, HIGH);
  delay(2500);
  digitalWrite(27, LOW);
}
