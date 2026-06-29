#include "hyperwisor-iot.h"
#include <Wire.h>

HyperwisorIOT device;

const char* API_KEY    = "";
const char* SECRET_KEY = "";

String targetId = "";
String deviceId = "";

const int MPU_ADDR = 0x68; 
unsigned long lastTime = 0;

// --- 6-AXIS FILTER PARAMETERS ---
float roll = 0.0;
float pitch = 0.0;
const float dt = 0.1;        // 100ms time step (in seconds)
const float alpha = 0.96;     // 96% Gyro / 4% Accelerometer weighting split

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C Bus on GPIO 18 & 19
  Wire.begin(18, 19);
  
  // Wake up MPU6500
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);

  // Initialize Cloud Connection
  device.begin();
  targetId = device.getUserId();
  deviceId = device.getDeviceId();
}

void loop() {
  device.loop(); 
  
  // Non-blocking 100ms execution window
  if (millis() - lastTime >= 100) {
    lastTime = millis();
    
    // Request all 14 bytes sequentially from the MPU6500 register map
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B); 
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 14, true); 
    
    // 1. Read Accelerometer Data (3 Axes)
    int16_t raw_ax = Wire.read() << 8 | Wire.read();
    int16_t raw_ay = Wire.read() << 8 | Wire.read();
    int16_t raw_az = Wire.read() << 8 | Wire.read();
    
    // Skip internal chip temperature registers
    Wire.read() << 8 | Wire.read(); 
    
    // 2. Read Gyroscope Data (3 Axes)
    int16_t raw_gx = Wire.read() << 8 | Wire.read();
    int16_t raw_gy = Wire.read() << 8 | Wire.read();
    int16_t raw_gz = Wire.read() << 8 | Wire.read();

    // Calculate static tilt angles from accelerometer data
    float accRoll  = (raw_ay / 182.0); 
    float accPitch = (raw_ax / 182.0);

    // Convert raw gyroscope readings into real-world Angular Velocity (°/s)
    float gyroXRate = raw_gx / 131.0; // Roll Speed
    float gyroYRate = raw_gy / 131.0; // Pitch Speed
    float gyroZRate = raw_gz / 131.0; // Yaw Speed (Spinning left/right on flat ground)

    // Blend data using a Complementary Filter to eliminate noise and drift
    roll  = alpha * (roll + gyroXRate * dt) + (1.0 - alpha) * accRoll;
    pitch = alpha * (pitch + gyroYRate * dt) + (1.0 - alpha) * accPitch;

    // Local debug print to verify output ranges locally
    Serial.print("Roll: ");   Serial.print(roll, 1);
    Serial.print(" | Pitch: "); Serial.print(pitch, 1);
    Serial.print(" | Gyro Z-Rate: "); Serial.println(gyroZRate, 1);
    
    // ======================================================================
    // WIDGET 1: THE FLIGHT "Altitude Indicator" Widget (PITCH & ROLL TOGETHER)
    // ======================================================================
    device.updateFlightAttitude(targetId, "WidgetId", roll, pitch); 

    // ======================================================================
    // WIDGET 2: THE DEDICATED GYROSCOPE DIAL "GAUGE" Widget (YAW RATE / TURN VELOCITY)
    // ======================================================================
    device.updateWidget(targetId, "WidgetId", gyroZRate); 
  } // <-- This closes the 100ms timing window safely, keeping variables in scope!
} // <-- This closes the loop() function cleanly