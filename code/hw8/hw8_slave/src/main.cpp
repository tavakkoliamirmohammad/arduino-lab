#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>
#define REC_COMMAND 0
#define REC_ADDRESS 1
#define REC_VALUE 2
#define COMMAND_STORE 1
#define COMMAND_LOAD 0

volatile boolean rec_done;
volatile byte recieved_value;
volatile byte rec_state, rec_address, rec_value, rec_command, sent_value;
char message[50];
char message1[50];

ISR (SPI_STC_vect) {
  if(rec_state == REC_COMMAND) {
    rec_command = SPDR;
    sprintf(message, "rec command is %d", rec_command);
    Serial.println(message);
    rec_state = REC_ADDRESS;
  }
  else if (rec_state == REC_ADDRESS)
  {
    if(rec_done) return;
    rec_address = SPDR;
    sprintf(message, "rec address is %d", rec_address);
    Serial.println(message);
    if (rec_command == COMMAND_LOAD) {
      rec_done = true;
    } else if(rec_command == COMMAND_STORE) {
       rec_state = REC_VALUE;
    }
  } else if(rec_state == REC_VALUE) {
    rec_value = SPDR;
    sprintf(message, "rec value is %d", rec_value);
    Serial.println(message);
    if (rec_command == COMMAND_STORE) {
      rec_done = true;
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(MISO,OUTPUT);
  SPCR |= _BV(SPE);
  rec_done = false;
  rec_state = REC_COMMAND;
  SPI.attachInterrupt();
}

void loop() {
  if(rec_done) {
    if (rec_command == COMMAND_STORE) {
      sprintf(message1, "Writing to %d home value %d", rec_address, rec_value);
      Serial.println(message1);
      EEPROM.write(rec_address, rec_value);
    } else if (rec_command == COMMAND_LOAD) {
      sent_value = EEPROM.read(rec_address);
      sprintf(message1, "Reading from %d home value %d", rec_address, sent_value);
      SPDR = sent_value;
      delay(20);
      Serial.println(message1);
    }
    rec_done = false;
    rec_state = REC_COMMAND;
  }
}