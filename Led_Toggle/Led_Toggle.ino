#include "hyperwisor-iot.h"

HyperwisorIOT device;

// ── API Keys ────────────────────────────────────────────────────────────────
const char* API_KEY    = "";
const char* SECRET_KEY = "";

String targetId = ""; 
String deviceId = ""; // Added missing deviceId declaration
const int LED_R = 14;  // CHANGED from 6 to 14 to avoid crashing the ESP32 internal flash memory

void Led_On();
void Led_Off();

void handleCommands(JsonObject& msg) {
  Serial.println("Command received!");
  serializeJson(msg, Serial);
  Serial.println();

  JsonObject payload = msg["payload"];

  // Look for the "Operate" command base
  JsonObject cmd = device.findCommand(payload, "Operate");
  if (!cmd.isNull()) {
    
    // Check for Action: Led_On
    JsonObject act_on = device.findAction(payload, "Operate", "Led_On");
    if (!act_on.isNull()) {
      Led_On(); // FIXED: Called the function correctly instead of re-declaring it
    }

    // Check for Action: Led_Off
    JsonObject act_off = device.findAction(payload, "Operate", "Led_Off");
    if (!act_off.isNull()) {
      Led_Off(); // FIXED: Called the function correctly
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(LED_R, OUTPUT);
  digitalWrite(LED_R, HIGH); // Turn off by default (assuming Active-Low logic)

  device.setUserCommandHandler(handleCommands);
  device.begin();

  targetId = device.getUserId();
  deviceId = device.getDeviceId();
}

void loop() { // FIXED: Corrected spelling from 'lopp()' to 'loop()'
  device.loop();
}

void Led_On() {
  digitalWrite(LED_R, LOW); //Uses Common Anode Led
  device.updateWidget(targetId, "WidgetId", 1);
}

void Led_Off() {
  digitalWrite(LED_R, HIGH);
  device.updateWidget(targetId, "WidgetId", 0); // FIXED: Added missing semicolon
}