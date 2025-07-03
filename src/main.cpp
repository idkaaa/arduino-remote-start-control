#include <Arduino.h>
#include "wifi_setup.h"
#include "async_web_server.h"

void setup() {
  Serial.begin(115200);
  wifi_init();
  server_init();
}

void loop() {
  delay(5000);
  refresh_sensor_data(); // Read sensor data periodically
  remote_start_check();
}