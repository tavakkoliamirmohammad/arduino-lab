#include <Arduino.h>

#define LOWER_NUMBER 0
#define EQUAL_NUMBER 1
#define HIGHER_NUMBER 2

int start, end, guess;
bool is_done;


void find_guess(int command){
  if(is_done) return;
  if (command == LOWER_NUMBER)
  {
    start = guess;
  }
  else if (command == HIGHER_NUMBER)
  {
   end = guess;
  }
  else {
    is_done = true;
    return;
  }
  guess = (end - start) / 2 + start;
  delay(100);
  Serial.println(guess);
}


void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  is_done = false;
  start = 0;
  end = 100;
  guess = (end - start) / 2 + start;
  delay(100);
  Serial.println(guess);
}

void loop() {

}

void serialEvent() {
  int command = Serial.parseInt();
  find_guess(command);
}
