#include "hyperwisor-iot.h"
#include <ESP32Servo.h>

HyperwisorIOT device;
Servo servo;

String targetId = ""; 
int mtr = 18; // Moved away from Input-Only Pin 34

// Global shared variables
volatile int targetAngle = 0;
volatile int currentSliderVal = 0;
volatile bool newCommandAvailable = false;

// Non-blocking telemetry timer variables
unsigned long lastCloudUpload = 0;
const unsigned long uploadDelay = 500; // Restrict dashboard updates to twice a second (2 Hz)

void handleCommands(JsonObject& msg) {
  JsonObject payload = msg["payload"];
  
  JsonObject cmd = device.findCommand(payload, "CMD");
  if (cmd.isNull()) return;

  JsonObject Action = device.findAction(payload, "CMD", "Action");
  if (Action.isNull()) return;

  // Extract values instantly and hand them off to global scope
  currentSliderVal = Action["params"]["value"] | 0;
  targetAngle = map(currentSliderVal, 0, 100, 0, 180);
  
  newCommandAvailable = true;
}

void setup() {
  Serial.begin(115200);
  servo.attach(mtr, 500, 2400); // Calibrated pulse range for full 180 sweep
  servo.write(0);
  
  device.setUserCommandHandler(handleCommands);
  device.begin();
  targetId = device.getUserId();
}

void loop() {
  device.loop(); // Keeps WebSocket link completely unhindered

  // 1. Actuate physical hardware immediately
  if (newCommandAvailable) {
    newCommandAvailable = false;
    servo.write(targetAngle);
  }

  // 2. Asynchronously stream data up to the dashboard widget safely
  unsigned long current = millis();
  if (current - lastCloudUpload > uploadDelay) {
    lastCloudUpload = current;
    
    // SAFE UPDATE: Pushes the state back smoothly without choking the input command channel
    device.updateWidget(targetId, "WidgetId", currentSliderVal);
  }
}