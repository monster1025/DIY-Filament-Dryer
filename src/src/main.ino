#include <Arduino.h>
#include <GyverOS.h>
// Local import
#include "config.h"
#include "wifi_credentials.h"

#define DEBUG 1
#define LED_PIN D4

GyverOS<5> OS;	// указать макс. количество задач
//web
float target_temperature_in = 0.0;
float max_temperature_heater = 0.0;
float target_fan_percent = 10.0;
uint8_t box_status = 0;
float temperature_in = 0;
float temperature_out = 0;
float temperature_heater = 0;
float humidity_in = 0;
uint8_t fan_duty = 0;
uint8_t humidity_out = 0;
//end web

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);

  setup_temperature_sensors();
  setup_heater();
  setup_fan();
  setup_web();

  // подключаем задачи (порядковый номер, имя функции, период в мс)
  OS.attach(0, task_setup_web, 1000);
  OS.attach(1, task_read_temperatures, 1000);
  OS.attach(2, task_heater, 1000);
}

void loop() {
  OS.tick();	// вызывать как можно чаще, задачи выполняются здесь

  // OS.getLeft() возвращает время в мс до ближайшей задачи
  // на это время можно усыпить МК, например при помощи narcoleptic или GyverPower
  // для примера просто поставим delay на это время
  delay(OS.getLeft());
}