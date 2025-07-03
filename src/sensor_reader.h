#ifndef SENSOR_READER_H_
#define SENSOR_READER_H_

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

const uint8_t DHTPIN = 18;     // what digital pin we're connected to
const uint8_t DHTTYPE = DHT11; // DHT 11

DHT dht(DHTPIN, DHTTYPE);

float CurrentTemperatureCelcius = -1;
float CurrentHumidity = -1;

void sensor_reader_init() {
  // Initialize the DHT sensor
  dht.begin();
  Serial.println("DHT sensor initialized.");
}

void readDHTSensor() {
  Serial.println("Reading DHT sensor data...");
  CurrentTemperatureCelcius = dht.readTemperature();
  CurrentHumidity = dht.readHumidity();
}

String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = CurrentTemperatureCelcius;
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = CurrentHumidity;
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}


#endif // SENSOR_READER_H_