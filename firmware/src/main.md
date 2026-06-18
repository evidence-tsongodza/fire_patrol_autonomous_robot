
```c++
#include <Arduino.h>

const int green = 25;
const int amber = 26;
const int red = 27;

void setup() {
  // put your setup code here, to run once:
  pinMode(green, OUTPUT);
  pinMode(amber, OUTPUT);
  pinMode(red, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(green, HIGH);
  delay(2500); // this speeds up the simulation
  digitalWrite(green, LOW);
  digitalWrite(amber, HIGH);
  delay(1500);
  digitalWrite(amber, LOW);
  digitalWrite(red, HIGH);
  delay(2500);
  digitalWrite(red, LOW);
}
```