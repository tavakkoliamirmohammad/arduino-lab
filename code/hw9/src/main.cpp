#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define MOTOR_PORT 9

#define MOTOR_READ_PORT A0
#define LM35_PORT A1
#define LED_WARNING_PORT 7

#define WARNING_TEMPERTURE 30
#define CRITICAL_TEMPERTURE 40

#define TOTAL_DELAY 300
#define SPEED_CHANGE_EVENT 1
#define OVER_TEMPERTURE_EVENT 2
#define SHUT_DOWN_EVENT 3

SemaphoreHandle_t eeporm_mutex;
int system_health;
int old_motor_speed, new_motor_speed, old_temperture, new_temperture;
byte log_address;
char message[100];


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void log_error_code_eeprom(int value) {
  xSemaphoreTake(eeporm_mutex, portMAX_DELAY);
  EEPROM.update(log_address++, value);
  xSemaphoreGive(eeporm_mutex);
}

int current_error_code_eeprom() {
  int value;
  xSemaphoreTake(eeporm_mutex, portMAX_DELAY);
  value = EEPROM.read(log_address - 1);
  xSemaphoreGive(eeporm_mutex);
  return value;
}

void set_motor_speed_init() {
  pinMode(MOTOR_READ_PORT, INPUT);
  pinMode(MOTOR_PORT, OUTPUT);
  old_motor_speed = -1;
  new_motor_speed = 0;
}

void set_motor_speed_task(void* params) {
  for (;system_health;){
    Serial.println("MOTOR");
    new_motor_speed = analogRead(MOTOR_READ_PORT) >> 2;
    analogWrite(MOTOR_PORT, new_motor_speed);
    vTaskDelay(TOTAL_DELAY / portTICK_PERIOD_MS);
  }
  analogWrite(MOTOR_PORT, 0);
  vTaskDelete(NULL);
}

void read_temperture_init() {
  pinMode(LM35_PORT, INPUT);
  new_temperture = 0;
  old_temperture = -1;
}

void read_temperture_task(void* params) {
  for (;;){
    Serial.println("TEMP");
    new_temperture = analogRead(LM35_PORT) * 500.0 /1024;
    vTaskDelay(TOTAL_DELAY / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}
void show_info_lcd_init(void) {
  lcd.begin(16, 2);
}

void show_info_lcd_task(void* params) {
  for (;;){
    Serial.println("LCD");
    sprintf(message, "T:%d S:%d E:%d", new_temperture, new_motor_speed, current_error_code_eeprom());
    lcd.clear();
    lcd.println(message);
    vTaskDelay(TOTAL_DELAY / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void log_errors_init() {
  eeporm_mutex = xSemaphoreCreateMutex();
  log_address = 0;
}

void log_errors_task(void* params) {
  for (;;){
    Serial.println("Logger");
    if(old_temperture != new_temperture) {
      old_temperture = new_temperture;
      if (new_temperture >= CRITICAL_TEMPERTURE) {
        log_error_code_eeprom(SHUT_DOWN_EVENT);
      } else if (new_temperture >= WARNING_TEMPERTURE) {
        log_error_code_eeprom(OVER_TEMPERTURE_EVENT);
      }
    }
    if (old_motor_speed != new_motor_speed) {
      log_error_code_eeprom(SPEED_CHANGE_EVENT);
      old_motor_speed = new_motor_speed;
    }
    vTaskDelay(TOTAL_DELAY / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void controller_init() {
  system_health = 1;
  pinMode(LED_WARNING_PORT, OUTPUT);
}


void controller_task(void* params) {
  for (;;){
    Serial.println("Controller");
    if (new_temperture >= CRITICAL_TEMPERTURE) {
      system_health = 0;
    }
    if (new_temperture >= WARNING_TEMPERTURE) {
      digitalWrite(LED_WARNING_PORT, HIGH);
    } else {
       digitalWrite(LED_WARNING_PORT, LOW);
    }
    vTaskDelay(TOTAL_DELAY / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(9600);

  set_motor_speed_init();
  read_temperture_init();
  log_errors_init();
  show_info_lcd_init();
  controller_init();

  xTaskCreate(set_motor_speed_task, "Motor Speed Task", 128, NULL, tskIDLE_PRIORITY + 3, NULL);
  xTaskCreate(read_temperture_task, "Read Temperture Task", 128, NULL, tskIDLE_PRIORITY + 3, NULL);
  xTaskCreate(log_errors_task, "Log Error Task", 64, NULL, tskIDLE_PRIORITY + 2, NULL);
  xTaskCreate(show_info_lcd_task, "Show Info LCD Task", 128, NULL, tskIDLE_PRIORITY + 3, NULL);
  xTaskCreate(controller_task, "Controller Task", 64, NULL, tskIDLE_PRIORITY + 4, NULL);
}

void loop() {}