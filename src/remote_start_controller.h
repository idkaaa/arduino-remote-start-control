#ifndef REMOTE_START_CONTROLLER_H_
#define REMOTE_START_CONTROLLER_H_

#include <Arduino.h>

const int REMOTE_START_PIN = 23; // GPIO pin for remote start control

const unsigned int MaxRuntime = 30 * 60 * 1000;  // Maximum runtime in milliseconds (30 minutes)
const unsigned int MinRuntime = 5 * 60 * 1000;   // Minimum runtime in milliseconds (5 minutes)
const unsigned int CooldownTime = 10000; // Cooldown time between stopping and restarting the vehicle in milliseconds

//bool IsComfortStartEnabled = false; // Flag to check if comfort start is enabled

int ComfortTempMin = 19; // Minimum comfort temperature
int ComfortTempMax = 25; // Maximum comfort temperature
bool IsComfortStartEnabled = false; // Flag to check if comfort start is enabled, default to true

int PreviousTemp = -1;

unsigned int LastStartTime = 0; // Last time the vehicle was started
unsigned int LastStopTime = 0; // Last time the vehicle was stopped

bool IsVehicleRunning = false; // Flag to check if the vehicle is running. Assume false for initial boot

void remote_start_controller_init()
{
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
    if (IsVehicleRunning) {
        Serial.println("Vehicle started successfully.");
        LastStartTime = millis(); // Update the last start time
    }else{
        Serial.println("Vehicle stopped successfully.");
        LastStopTime = millis(); // Update the last stop time
    }
}

void remote_start_stop() {
    if (!IsVehicleRunning) {
        Serial.println("Vehicle is not running. No action taken.");
        return;
    }
    if (millis() < (LastStartTime + MinRuntime)) {
        Serial.println("Vehicle has not run long enough to stop. Minimum runtime not reached.");
        return; // Do not stop if the vehicle has not run long enough
    }
    remote_start_trigger(); // Trigger the remote start to stop the vehicle
}

void remote_start_start() {
    if (IsVehicleRunning) {
        if (millis() > (LastStartTime + MaxRuntime)) {
            Serial.println("Time to restart vehicle again.");
            remote_start_stop();
            return;
        }
        Serial.println("Vehicle is already running. No action taken.");
        return;
    }
    if (millis() < (LastStopTime + CooldownTime)) {
        Serial.println("Cooldown period not yet over. Cannot start vehicle.");
        return; // Do not start if cooldown period is not over
    }
    remote_start_trigger(); // Trigger the remote start
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
    Serial.println("Checking comfort start conditions for current temperature: " + String(currentTemp));
    if (isnan(currentTemp)) {
        Serial.println("Current temperature is invalid. Cannot check comfort start conditions.");
        return; // Do not proceed if the current temperature is invalid
    }
    if (!IsComfortStartEnabled) {
        Serial.println("Comfort start is disabled. No action taken.");
        PreviousTemp = -1; // Reset PreviousTemp if comfort start is disabled
        return; 
    }
    if (PreviousTemp == -1) {
        PreviousTemp = currentTemp; // Initialize PreviousTemp on first run
    }
    if (isTempInRange(PreviousTemp)) {
        Serial.println("Previous temperature was in comfort range.");
        if (isTempInRange(currentTemp)) {
            Serial.println("Current temperature is also in comfort range. Stop if needed.");
            remote_start_stop();
        } else {
            Serial.println("Current temperature is outside comfort range. Start if needed.");
            remote_start_start();
        }
    } else {
        Serial.println("Previous temperature was outside comfort range.");
        if (isTempInRange(currentTemp)) {
            Serial.println("Current temperature is now in comfort range. Stop if needed.");
            remote_start_stop();
        } else {
            Serial.println("Current temperature is outside comfort range. Start if needed.");
            remote_start_start();
        }
    }
    PreviousTemp = currentTemp;
}

#endif // REMOTE_START_CONTROLLER_H_