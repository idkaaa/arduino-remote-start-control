#include <Arduino.h>
#include "wifi_setup.h"
#include "async_web_server.h"


void setup() {
  Serial.begin(115200);
  wifi_init();
  server_init();
}

void loop() {
  // put your main code here, to run repeatedly:
}