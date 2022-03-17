#include <Arduino.h>
#include <LiquidCrystal.h>

#define LOWER_NUMBER 0
#define EQUAL_NUMBER 1
#define HIGHER_NUMBER 2

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int random_number;
int guess;
char lcd_message[100];

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  random_number = map(analogRead(A0), 0, 1023, 0, 100);
  lcd.begin(16, 2);
}

void loop() {
  if(Serial.available() > 0) {
      guess = Serial.parseInt();
    if (guess == random_number)
    {
      Serial.println(EQUAL_NUMBER);
      sprintf(lcd_message, "Found: %d", guess);
    }
    else if(guess < random_number) {
      Serial.println(LOWER_NUMBER);
      sprintf(lcd_message, "Less: %d", guess);
    }
    else {
      Serial.println(HIGHER_NUMBER);
      sprintf(lcd_message, "Greater: %d", guess);
    }
    lcd.clear();
    lcd.println(lcd_message);
    delay(100);
  }
}