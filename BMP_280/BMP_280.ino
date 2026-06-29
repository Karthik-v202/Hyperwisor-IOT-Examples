#include "hyperwisor-iot.h"
#include <Wire.h>
#include <Adafruit_BMP280.h>

// Instantiate the Hyperwisor IoT framework and BMP280 sensor instances
HyperwisorIOT device;
Adafruit_BMP280 bmp;

// Runtime identity token loaded automatically from memory during system setup
String targetId = "";

// Asynchronous telemetry timer configuration variables
unsigned long lastUpdateTime = 0;   
const unsigned long interval = 2000; // 2000ms = 2 seconds polling threshold window

void setup() {
  // Initialize standard hardware serial terminal for local diagnostic monitoring
  Serial.begin(115200);
  
  // Initialize the I2C bus with custom pins (SDA = GPIO 18, SCL = GPIO 19)
  Wire.begin(18, 19);
  
  // Initialize the physical BMP280 sensor over I2C at the standard 0x76 address
  bmp.begin(0x76);
  
  // Start the underlying network handshakes and WebSocket pipeline loops
  device.begin();
  
  // Fetch the active user dashboard identifier string for routing data packets
  targetId = device.getUserId();
}

void loop() {
  // MUST execute continuously to keep WebSocket lines, keep-alives, and pings active
  device.loop();
  
  // Non-blocking clock filter: Executes cleanly once every 2 seconds
  if (millis() - lastUpdateTime >= interval) {
    lastUpdateTime = millis();
    
    // Read the ambient temperature metrics in Degrees Celsius (°C)
    float t = bmp.readTemperature();
    
    // Read the atmospheric pressure and convert it from Pascals (Pa) to Hectopascals (hPa)
    float p = bmp.readPressure() / 100.0;
    
    // Compute approximate altitude in meters (m) based on local Sea Level Pressure reference (e.g., 1010.8 hPa)
    float a = bmp.readAltitude(1010.8);
    
    // Local Diagnostic Feedback: Output data array to the local Serial Monitor
    Serial.print("Temperature: "); Serial.print(t); Serial.print(" °C | ");
    Serial.print("Pressure: ");    Serial.print(p); Serial.print(" hPa | ");
    Serial.print("Altitude: ");    Serial.print(a); Serial.println(" m");

    // Stream telemetry payloads up to your distinct Hyperwisor dashboard widgets
    // REMEMBER: Replace placeholder "WidgetId" parameters with your actual unique canvas string tokens!
    device.updateWidget(targetId, "YOUR_TEMP_WIDGET_ID", t);
    device.updateWidget(targetId, "YOUR_PRESS_WIDGET_ID", p);
    device.updateWidget(targetId, "YOUR_ALT_WIDGET_ID", a);
  }
}