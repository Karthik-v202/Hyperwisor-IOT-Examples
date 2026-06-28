#include "hyperwisor-iot.h"

// Instantiate the Hyperwisor IoT instance
HyperwisorIOT device;

// ── API Keys ────────────────────────────────────────────────────────────────
// Authentication keys generated from your Hyperwisor Developer Dashboard
const char* API_KEY    = "";
const char* SECRET_KEY = "";

// Runtime identity variables loaded automatically during initialization
String targetId = ""; 

// Hardware pin assignment (Ensure you use a general-purpose GPIO like Pin 14)
const int Button = 14;

// Tracks the raw, un-debounced transient input state of the hardware pin
int button_state = 0;

// Variables dedicated to non-blocking software debouncing
unsigned long last = 0;             // Stores the last time the pin bounced or toggled
const unsigned long interval = 50;  // Debounce time window in milliseconds (ignores electrical noise)

void setup() {
  Serial.begin(115200);
  
  // Configure the GPIO pin with an external 10k Pull-Down resistor reference
  pinMode(Button, INPUT);
  
  // Start the underlying network and WebSocket stack automatically
  device.begin();

  // Fetch the provisioned user/app ID target needed for pushing widget payloads
  targetId = device.getUserId();
}

void loop() {
  // Keeps background cloud handshakes, pings, and asynchronous network packets alive
  device.loop();
  
  // Sample the real-time electrical potential of the button pin
  int reading = digitalRead(Button);

  // Mechanical switch bounce detector: Reset the timer if the state fluctuated
  if (reading != button_state) {
    last = millis(); 
    button_state = reading;
  }

  // Process the signal change ONLY after it has remained electrically stable for > 50ms
  if ((millis() - last) > interval) {
    // Tracks the last structurally confirmed and debounced hardware logic level
    static int confirmedButtonState = LOW;

    // Check if the confirmed state has changed relative to the previous loop cycle
    if (reading != confirmedButtonState) {
      confirmedButtonState = reading;

      // Active-High Logic Assignment via external pull-down wiring:
      // Pressed  -> Pin drives to VCC -> HIGH (1)
      // Released -> 10k drains to GND -> LOW (0)
      int dashboardValue = confirmedButtonState; 

      if (dashboardValue == HIGH) {
        // Broadcast custom warning status message to the specific canvas text/status widget
        device.updateWidget(targetId, "WidgetId", "Warning");
        Serial.println("🔴 Button Event: PRESSED (HIGH)");
      } else {
        // Broadcast resting status message to clear the dashboard interface panel
        device.updateWidget(targetId, "WidgetId", "Safe");
        Serial.println("⚪ Button Event: RELEASED (LOW)");
      }
    }
  }
}
