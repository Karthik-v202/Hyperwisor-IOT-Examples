//Using a "Common Anode" Led
// Invert the logic to work with Common Cathode

#include "hyperwisor-iot.h"

HyperwisorIOT device;

// ── API Keys ────────────────────────────────────────────────────────────────
const char* API_KEY    = "";
const char* SECRET_KEY = "";

String targetId = ""; 
const int LED_R = 14; 
const int LED_G = 12; // Updated from 2 to 4

void handleCommands(JsonObject& msg) {
  JsonObject payload = msg["payload"];

  // --- RED: Look for Command "RED_CTRL" ---
  JsonObject redCmd = device.findCommand(payload, "RED_CTRL");
  if (!redCmd.isNull()) {
    JsonObject updateAction = device.findAction(payload, "RED_CTRL", "UPDATE");
    if (!updateAction.isNull()) {
      int val = updateAction["params"]["value"] | 0;
      analogWrite(LED_R, 255 - constrain(val, 0, 255));
      Serial.print("RED logic active: "); Serial.println(val);
      device.updateWidget(targetId, "WidgetId", val);
    }
  }

  // --- GREEN: Look for Command "GRN_CTRL" ---
  JsonObject grnCmd = device.findCommand(payload, "GRN_CTRL");
  if (!grnCmd.isNull()) {
    JsonObject updateAction = device.findAction(payload, "GRN_CTRL", "UPDATE");
    if (!updateAction.isNull()) {
      int val = updateAction["params"]["value"] | 0;
      analogWrite(LED_G, 255 - constrain(val, 0, 255));
      Serial.print("GREEN logic active: "); Serial.println(val);
      device.updateWidget(targetId, "WidgetId", val);
    }
  }
  else {
      Serial.println("Error: Dashboard forgot the 'value' parameter for Green.");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);

  // For Common Anode, 255 is OFF. 
  // Changed from 0 to 255 to keep hardware off on boot.
  analogWrite(LED_R, 255); 
  analogWrite(LED_G, 255); 

  device.setApiKeys(API_KEY, SECRET_KEY);
  device.setUserCommandHandler(handleCommands);
  device.begin();

  targetId = device.getUserId();
  Serial.println("System Online. Ready for commands.");
}

void loop() {
  device.loop();
}