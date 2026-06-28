#include "hyperwisor-iot.h"
#include <DHT.h>

#define DHTTYPE DHT11
#define DHTPIN 4

DHT dht(DHTPIN, DHTTYPE);
HyperwisorIOT device;

String targetId = "";

// ── Non-Blocking Timer Variables ───────────────────────────────────────────
unsigned long lastUpdateTime = 0;   
const unsigned long interval = 2000; // 2000 milliseconds = 2 seconds

void setup() {
  Serial.begin(115200);
  
  dht.begin();
  device.begin();
  targetId = device.getUserId();
  
  Serial.println("System initialized. Waiting for stable DHT11 readings...");
}

void loop() {
  device.loop(); // Must run continuously at full speed

  // Only run this block once every 2 seconds
  if (millis() - lastUpdateTime >= interval) {
    lastUpdateTime = millis(); 

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    // Debugging print: Check if the sensor is actually responding
    if (isnan(t) || isnan(h)) {
      Serial.println("❌ Failed to read from DHT sensor! Check hardware connections.");
      return; 
    }

    Serial.print("Temp: "); Serial.print(t); Serial.print(" °C | ");
    Serial.print("Hum: "); Serial.print(h); Serial.println(" %");

    // Send the valid data to your dashboard widgets
    device.updateWidget(targetId, "widget_1781843534936", t);
    device.updateWidget(targetId, "widget_1781843536519", h);
  }
}