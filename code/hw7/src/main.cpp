#include <Arduino.h>

#include <LiquidCrystal.h>
#include <Wire.h>

#define DS_1307_ADDRESS 0x68

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char time[50], date[50];
char time_pattern[] = "Time: %d%d:%d%d:%d%d";
char date_pattern[] = "Date: %d%d/%d%d/20%d%d";

struct {
  int second;
  int minute;
  int hour;
  int day;
  int month;
  int year;
} date_time_info;

inline void bcd_to_decimal(int* number) {
   *number = (*number >> 4) * 10 + (*number & 0x0F);
}

inline void decimal_to_bcd(int* number) {
   *number = ((*number / 10) << 4) + (*number % 10);
}

void write_data_ds1307() {
  decimal_to_bcd(&date_time_info.second);
  decimal_to_bcd(&date_time_info.minute);
  decimal_to_bcd(&date_time_info.hour);
  decimal_to_bcd(&date_time_info.day);
  decimal_to_bcd(&date_time_info.month);
  decimal_to_bcd(&date_time_info.year);

  Wire.beginTransmission(DS_1307_ADDRESS);
  Wire.write(0);                             
  Wire.write(date_time_info.second);
  Wire.write(date_time_info.minute);
  Wire.write(date_time_info.hour);
  Wire.write(1);
  Wire.write(date_time_info.day);
  Wire.write(date_time_info.month);
  Wire.write(date_time_info.year);
  Wire.endTransmission();
}

void initial_date_setup() {
  date_time_info.second = 0;
  date_time_info.minute = 36;
  date_time_info.hour = 9;
  date_time_info.day = 6;
  date_time_info.month = 3;
  date_time_info.year = 21;
}

void display_time(){

  bcd_to_decimal(&date_time_info.second);
  bcd_to_decimal(&date_time_info.minute);
  bcd_to_decimal(&date_time_info.hour);
  bcd_to_decimal(&date_time_info.day);
  bcd_to_decimal(&date_time_info.month);
  bcd_to_decimal(&date_time_info.year);

  sprintf(date, date_pattern,
    date_time_info.day / 10, date_time_info.day % 10,
    date_time_info.month / 10, date_time_info.month % 10, 
    date_time_info.year / 10, date_time_info.year % 10
  );
  lcd.setCursor(0, 0);
  lcd.print(date);

  sprintf(time, time_pattern,
    date_time_info.hour / 10, date_time_info.hour % 10,
    date_time_info.minute / 10, date_time_info.minute % 10, 
    date_time_info.second / 10, date_time_info.second % 10
  );
  lcd.setCursor(0, 1);
  lcd.print(time);
}


void setup() {
  lcd.begin(16, 2);
  Wire.begin();
  initial_date_setup();
  write_data_ds1307();
}

void loop() {
  Wire.beginTransmission(DS_1307_ADDRESS);
  Wire.write(0);
  Wire.endTransmission(false); // Don't release the connection
  Wire.requestFrom(DS_1307_ADDRESS, 7);
  date_time_info.second = Wire.read();
  date_time_info.minute = Wire.read();
  date_time_info.hour = Wire.read();
  Wire.read();
  date_time_info.day = Wire.read();
  date_time_info.month = Wire.read();
  date_time_info.year = Wire.read();
  display_time();
  delay(100);
}