#include <Arduino.h>
#include "wifi_setup.h"
#include "async_web_server.h"
#include "ota_updater.h"

void setup() {
  Serial.begin(115200);
  wifi_init();
  server_init();
  ota_updater_init();
}

void loop() {
  ota_updater_check();
}