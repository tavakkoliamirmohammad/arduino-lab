#include <Arduino.h>

#define OUTPUT_PIN 4

int toggle = 0;
int val;

void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 0;
  
  TCCR1A |= (1 << COM1A0) | (1 << COM1B0);
  TCCR1B |= (1 << WGM12) | (1 << CS11);
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

ISR(TIMER1_COMPA_vect){
  if(toggle) {
    digitalWrite(OUTPUT_PIN, LOW);
    OCR1A = 1023 - val;
  } else {
    digitalWrite(OUTPUT_PIN, HIGH);
    OCR1A = val;
  }
  toggle = ~toggle;
}


void loop() {
  val = map(analogRead(A0), 0, 1023, 1023 * 0.1, 1023 * 0.9);
}