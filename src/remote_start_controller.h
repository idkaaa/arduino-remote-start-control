#ifndef REMOTE_START_CONTROLLER_H_
#define REMOTE_START_CONTROLLER_H_

#include <Arduino.h>

const int REMOTE_START_PIN = 23; // GPIO pin for remote start control

//bool IsComfortStartEnabled = false; // Flag to check if comfort start is enabled

int ComfortTempMin = 10; // Minimum comfort temperature
int ComfortTempMax = 30; // Maximum comfort temperature
bool IsComfortStartEnabled = false; // Flag to check if comfort start is enabled, default to true

int PreviousTemp = -1;

unsigned int ComfortCheckInterval; // Interval to check comfort temperature in milliseconds
unsigned int NextComfortRestartTime = 0; // Next time to restart for comfort temperature

bool IsVehicleRunning = false; // Flag to check if the vehicle is running. Assume false for initial boot

void remote_start_controller_init()
{
    ComfortCheckInterval = 37 * 60 * 1000; // Set comfort check interval to 37 minutes
    pinMode(REMOTE_START_PIN, OUTPUT);
    digitalWrite(REMOTE_START_PIN, HIGH); // Set the remote start pin to HIGH (inactive state)
}

void toggle_vehicle_running_state() {
    IsVehicleRunning = !IsVehicleRunning; // Toggle the vehicle running state
    Serial.println("Vehicle running state toggled. Is vehicle running? " + String(IsVehicleRunning ? "Yes" : "No"));
}

void remote_start_trigger(){
    digitalWrite(REMOTE_START_PIN, LOW);
    delay(1000); // Trigger the remote start for 1 second
    digitalWrite(REMOTE_START_PIN, HIGH); // Turn off the relay
    toggle_vehicle_running_state(); // Toggle the vehicle running state
}

bool isTempInRange(int temp) {
    if (temp > ComfortTempMax) {
        Serial.println("Temperature is above comfort range.");
        return false;
    }
    if (temp < ComfortTempMin) {
        Serial.println("Temperature is below comfort range.");
        return false;
    }
    return true;
}

void remote_start_comfort_check(int currentTemp)
{
    if (!IsComfortStartEnabled) { 
        PreviousTemp = -1; // Reset PreviousTemp if comfort start is disabled
        return; 
    }
    if (PreviousTemp == -1) {
        PreviousTemp = currentTemp; // Initialize PreviousTemp on first run
    }
    if (isTempInRange(PreviousTemp)) {
        Serial.println("Previous temperature was in comfort range.");
        if (isTempInRange(currentTemp)) {
            Serial.println("Current temperature is also in comfort range. No action needed.");
        } else {
            Serial.println("Current temperature is outside comfort range. Triggering remote start.");
            remote_start_trigger();
            NextComfortRestartTime = millis() + ComfortCheckInterval; // Set next restart time
        }
    } else {
        Serial.println("Previous temperature was outside comfort range.");
        if (isTempInRange(currentTemp)) {
            Serial.println("Current temperature is now in comfort range. Ending run cycle.");
            remote_start_trigger();
        } else {
            Serial.println("Current temperature is still outside comfort range. Checking if time to start vehicle again.");
            if (millis() >= NextComfortRestartTime) {
                Serial.println("Time to start vehicle again.");
                remote_start_trigger();
                NextComfortRestartTime = millis() + ComfortCheckInterval;
            }else{
                Serial.println("Not time to start vehicle yet. Current time: " + String(millis()) + ", Next comfort restart time: " + String(NextComfortRestartTime));
            }
        }
    }
    PreviousTemp = currentTemp;
}

#endif // REMOTE_START_CONTROLLER_H_