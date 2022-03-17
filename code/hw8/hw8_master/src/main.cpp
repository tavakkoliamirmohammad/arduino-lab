#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SPI.h>

#define BUTTON 2
#define ADDRESS_READ 0
#define VALUE_READ 1
#define NOP 3

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns

char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};
byte rowPins[ROWS] = {2, A4, A5, A3};
byte colPins[COLS] = {9, 8, 7};

const int rs = 5, en = 4, d4 = 6, d5 = 3, d6 = 1, d7 = 0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

volatile byte address, value, operation_mode, recieved_value;
char message[50];

int getKeyPadValue() {
   if (kpd.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)
        {
            if ( kpd.key[i].stateChanged && kpd.key[i].kstate == PRESSED )
            {
              return kpd.key[i].kchar;
            }
        }
    }
    return -1;
}

void initialize_values() {
  address = 0;
  value = 0;
  operation_mode = ADDRESS_READ;
}

void accumulate_address(int v) {
  address = 10 * address + v;
}

void accumulate_value(int v) {
  value = 10 * value + v;
}

void initialize_spi_master() {
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  digitalWrite(SS,HIGH);
}

void setup() {
  lcd.begin(16, 2);
  initialize_values();
  initialize_spi_master();
}

void write_on_lcd(const char* message) {
  lcd.clear();
  lcd.println(message);
  delay(20);
}

void spi_transfer_store_command() {
  SPI.transfer(address);
  delay(20);
  SPI.transfer(value);
  delay(20);
}

int spi_transfer_load_command() {
  int tmp;
  SPI.transfer(address);
  delay(20);
  tmp = SPI.transfer(NOP);
  delay(20);
  return tmp;
}

void spi_init_transfer_command() {
  digitalWrite(SS, LOW);
  SPI.transfer(operation_mode);
  delay(20);
}

void spi_end_transfer_command(){
  digitalWrite(SS, HIGH);
}

void loop() {
  int keypad_value;
  if((keypad_value = getKeyPadValue()) != -1) {
    if(keypad_value >= '0' && keypad_value <= '9') {
      if (operation_mode == ADDRESS_READ)
        {
          accumulate_address(keypad_value - '0');
          write_on_lcd("Reading Address...");
        }
        else if(operation_mode == VALUE_READ) {
          accumulate_value(keypad_value - '0');
          write_on_lcd("Reading value...");
        }
     }
     else if (keypad_value == '*') {
       operation_mode = VALUE_READ;
     }
     else if (keypad_value == '#') {
      spi_init_transfer_command();
      if (operation_mode == VALUE_READ)
       {
        spi_transfer_store_command();
       } else if (operation_mode == ADDRESS_READ)
       {
        recieved_value = spi_transfer_load_command();
        sprintf(message, "Add %d: Val %d", address, recieved_value);
        write_on_lcd(message);
       }
      spi_end_transfer_command();
      initialize_values();
     }
   }
  delay(50);
}