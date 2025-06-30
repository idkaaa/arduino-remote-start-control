#ifndef WIFI_SETUP_H_
#define WIFI_SETUP_H_

#include <WiFi.h>
#include <ESPmDNS.h>
#include "secrets.h"


void wifi_init() {
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    IPAddress ip = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(ip);

    if (!MDNS.begin(MDNS_HOSTNAME)) {
        Serial.println("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }

    //MDNS.addService("ota", "tcp", 3232); // OTA updates
}

#endif