#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Arduino_JSON.h>
#include "async_web_server.h"
#include "sensor_reader.h"
#include "remote_start_controller.h"

const int HTTP_PORT = 80;
AsyncWebServer* server = nullptr;
bool IsRebootRequired = false;
const char* PARAM_INPUT_COMFORT_TEMP_MIN = "temp_min";
const char* PARAM_INPUT_COMFORT_TEMP_MAX = "temp_max";
const char* PARAM_INPUT_COMFORT_START_ENABLE = "comfort_start_enable";

JSONVar jsonData;

// // Replaces placeholder with DHT values
// String processor(const String &var)
// {
//     // Serial.println(var);
//     if (var == "TEMPERATURE")
//     {
//         return readDHTTemperature();
//     }
//     else if (var == "HUMIDITY")
//     {
//         return readDHTHumidity();
//     }
//     return String();
// }

void server_init() {
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
    remote_start_controller_init();
    server = new AsyncWebServer(HTTP_PORT);

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", String(), false);
    });
    server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/style.css", "text/css");
    });

    server->on("/refresh", HTTP_GET, [](AsyncWebServerRequest *request){
        jsonData["temperature"] = readDHTTemperature();
        jsonData["humidity"] = readDHTHumidity();
        jsonData["comfort_temp_min"] = ComfortTempMin;
        jsonData["comfort_temp_max"] = ComfortTempMax;
        jsonData["comfort_start_enabled"] = IsComfortStartEnabled; // Assuming you have a flag for comfort start
        jsonData["vehicle_running"] = IsVehicleRunning;
        String jsonString = JSON.stringify(jsonData);
        String currentTemp = readDHTTemperature();
        remote_start_comfort_check(currentTemp.toInt());
        request->send(200, "application/json", jsonString);
        jsonString = String();
    });

    server->on("/start", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Manual start request received");
        remote_start_trigger();
        request->send_P(200, "text/plain", "Remote start triggered");
    });

    server->on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("Update request received");
        String inputMessageTempMin;
        String inputMessageTempMax;
        String inputMessageComfortStartEnable;
        if (request->hasParam(PARAM_INPUT_COMFORT_TEMP_MIN) && request->hasParam(PARAM_INPUT_COMFORT_TEMP_MAX)) {
            inputMessageTempMin = request->getParam(PARAM_INPUT_COMFORT_TEMP_MIN)->value();
            inputMessageTempMax = request->getParam(PARAM_INPUT_COMFORT_TEMP_MAX)->value();
            ComfortTempMin = inputMessageTempMin.toInt();
            ComfortTempMax = inputMessageTempMax.toInt();
            Serial.println("Comfort temperature updated: " + inputMessageTempMin + " - " + inputMessageTempMax);
        }else if (request->hasParam(PARAM_INPUT_COMFORT_START_ENABLE)) {
            inputMessageComfortStartEnable = request->getParam(PARAM_INPUT_COMFORT_START_ENABLE)->value();
            IsComfortStartEnabled = (inputMessageComfortStartEnable == "1");
            Serial.println("Comfort start enabled updated to: " + inputMessageComfortStartEnable);
        }
        request->send(200, "text/plain", "OK");
    });

    server->begin();
}