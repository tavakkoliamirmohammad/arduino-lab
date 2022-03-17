#include <Arduino.h>

#define MOTOR_PORT 9

void setup() {
  pinMode(A0, INPUT);
  pinMode(MOTOR_PORT, OUTPUT);
}

int value = 0;

void loop() {

  value = analogRead(A0) >> 2;
  analogWrite(MOTOR_PORT, value);
  
}