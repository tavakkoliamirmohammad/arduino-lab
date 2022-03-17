#include <Arduino.h>

#define LED_PIN_START 2
#define LED_COUNT 4
#define LED_ON 1
#define LED_OFF 0


int adc_val = 0;

void setup() {
   pinMode(A0, INPUT);
   for (int i = 0; i < LED_COUNT; i++)
   {
     pinMode(LED_PIN_START + i, OUTPUT);
   }
}

void loop() {
  int val = analogRead(A0);
  adc_val = map(val, 0, 1023, 0, 100);
   for (int i = 0; i < LED_COUNT; i++) {
     if (adc_val > 90 && i == 3)
     {
        digitalWrite(LED_PIN_START + i, LED_ON);
     }
     else {
      if (adc_val >= (i + 1) * 25)
        {
          digitalWrite(LED_PIN_START + i, LED_ON);
        } else {
          digitalWrite(LED_PIN_START + i, LED_OFF);
        }
     }
   }
}