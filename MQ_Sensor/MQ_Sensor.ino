#include "hyperwisor-iot.h"

// Instantiate the Hyperwisor IoT instance
HyperwisorIOT device;

// Runtime identity variables loaded automatically from flash memory during initialization
String targetId = "";
String deviceId = "";

// Variables dedicated to non-blocking telemetry transmission
unsigned long last = 0;
const unsigned long Delay = 50; // 20ms execution window (samples and streams data at 50 Hz)

// Hardware pin assignment (Using an analog-capable ADC pin for the gas sensor)
int MQ = 34;

void setup() {
  // Initialize serial communication for local debugging interface
  Serial.begin(115200);
  
  // Configure the MQ sensor pin as a standard analog input
  pinMode(MQ, INPUT);
  
  // Start the underlying network and WebSocket stack automatically
  device.begin();
  
  // Fetch the provisioned identification targets needed for pushing widget payloads
  targetId = device.getUserId();
  deviceId = device.getDeviceId();
  
  Serial.println("System Online. Gas sensor monitoring active.");
}

void loop() {
  // Keeps background cloud handshakes, pings, and asynchronous network packets alive
  device.loop();

  // Fetch the current elapsed processor time
  unsigned long current = millis();

  // Non-blocking timer: Executes strictly when the 20ms interval window passes
  if (current - last > Delay) {
    last = current;
    
    // Sample the raw 12-bit analog voltage potential from the MQ sensor (0 - 4095)
    int val = analogRead(MQ);
    
    // Local Debugging Feedback: Prints values to the serial console for testing.
    // NOTE: These print statements are purely diagnostic; they do not alter the core cloud logic.
    Serial.print("MQ Sensor Analog Value: ");
    Serial.println(val);
    
    // Stream the raw telemetry data packet straight up to your target dashboard widget.
    // Because this is sensor data, no Command Tree or Action parameters are required!
    device.updateWidget(targetId, "WidgetId", val);
  }
}
