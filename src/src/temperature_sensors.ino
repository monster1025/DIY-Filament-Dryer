#include <Adafruit_BME280.h>

Adafruit_BME280 bme280;  // BME280 на I2C

void setup_temperature_sensors(){
      bme280.begin(0x76);
}

void task_read_temperatures() {
  //digitalWrite(LED_PIN, !digitalRead(LED_PIN));

  // температура
  temperature_in = bme280.readTemperature();
  humidity_in = bme280.readHumidity();

  current_temperature_updated(temperature_in);

  #if DEBUG
  String txt1="";
  txt1="Temp=";
  txt1= txt1+String(temperature_in);
  txt1= txt1+" *C";
  Serial.println(txt1);

  // влажность
  txt1="Hum=";
  txt1= txt1+String(humidity_in);
  txt1= txt1+" %";
  Serial.println(txt1);
  Serial.println("--------------------------------------------");
  #endif
}
