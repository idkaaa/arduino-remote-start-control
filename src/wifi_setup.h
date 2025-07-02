#ifndef WIFI_SETUP_H_
#define WIFI_SETUP_H_

#include <WiFi.h>
#include <ESPmDNS.h>
#include "secrets.h"

void wifi_start_ap_mode() {
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    IPAddress ip = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(ip);
}

void wifi_init() {
    Serial.println("Attempting to connect to wifi in station mode for SSID: "+ WIFI_SSID_DEBUG);
    WiFi.begin(WIFI_SSID_DEBUG, WIFI_PASSWORD_DEBUG);
    for (int i = 0; i < 3; i++) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.print("Connecting to WiFi in STA mode, attempt ");
            Serial.println(i + 1);
            delay(1000);
        }
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to WiFi in STA mode.");
        Serial.println(WiFi.localIP());
    }else{
        Serial.println("Failed to connect to WiFi in STA mode, switching to AP mode with SSID: "+ WIFI_SSID);
        wifi_start_ap_mode();
    }
    
    if (!MDNS.begin(MDNS_HOSTNAME)) {
        Serial.println("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }

    //MDNS.addService("ota", "tcp", 3232); // OTA updates
}

#endif