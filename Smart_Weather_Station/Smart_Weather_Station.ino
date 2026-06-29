#include "hyperwisor-iot.h"
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

HyperwisorIOT device;
Adafruit_BMP280 bmp;

// NOTE: It's good practice to hide your API/Secret keys before sharing code publicly!
const char* API_KEY    = "";
const char* SECRET_KEY = "";

String targetId = "";
String deviceId = "";

#define DHTTYPE DHT11
#define DHTPIN 4
DHT dht(DHTPIN, DHTTYPE);


//const int thrs = ;

unsigned long lasttime = 0;
const unsigned long current = 2500;

int gas = 34;

void setup() {
  pinMode(gas, INPUT);
  Serial.begin(115200);
  dht.begin();
  Wire.begin(18, 19);
  
  if (!bmp.begin(0x76)) {
    Serial.println("❌ Could not find a valid BMP280 sensor, check wiring!");
  }
  
  device.begin();
  targetId = device.getUserId();
  deviceId = device.getDeviceId();
}

void loop() {
  device.loop(); // Keeps framework tasks alive continuously
  
  unsigned long CurrentTime = millis();
  
  if (CurrentTime - lasttime >= current) {
    lasttime = CurrentTime;
    
    // --- 1. PROCESS DHT11 SENSOR ---
    float h = dht.readHumidity();
    if (!isnan(h)) {
      Serial.print("Live Humidity: "); Serial.print(h); Serial.println(" %");
      device.updateWidget(targetId, "widget_1781933849014", h);
    } else {
      Serial.println("⚠️ Failed to read from DHT11 sensor!");
    }

    // --- 2. PROCESS BMP280 SENSOR ---
    float t = bmp.readTemperature();
    float p = bmp.readPressure() / 100.0 + 1.56;
    float a = bmp.readAltitude(1010.60); // Calibrated baseline pressure for Kochi

    // Check if BMP280 read is healthy
    if (!isnan(t) && !isnan(p)) {
      Serial.print("Live Temperature: "); Serial.print(t); Serial.println(" °C");
      Serial.print("Live Air Pressure: "); Serial.print(p); Serial.println(" hPa");
      Serial.print("Live Altitude: "); Serial.print(a); Serial.println(" m");
      
      device.updateWidget(targetId, "WidgetId", t);  
      device.updateWidget(targetId, "WidgetId", p);
      device.updateWidget(targetId, "WidgetId", a);
    } else {
      Serial.println("⚠️ Failed to read from BMP280 sensor!");
    }
    int val = analogRead(gas);
    int g_val = map(val,1600,4095,0,100);
    device.updateWidget(targetId, "WidgetId", g_val);
    Serial.println(g_val);
    Serial.print(val);
    if (val>3000){
      device.updateWidget(targetId, "WidgetId", "Warning");
    }
    else{
      device.updateWidget(targetId, "WidgetId", "Safe");
    }
    
    Serial.println("----------------------------------------");
  }
}