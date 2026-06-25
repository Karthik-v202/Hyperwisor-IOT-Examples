# Hyperwisor IoT Core Examples & Developer Reference

A curated collection of modular, production-ready firmware examples and architectural templates for integrating ESP32 hardware with the Hyperwisor IoT cloud ecosystem.

---

## ⚠️ Pre-Upload Check (Crucial)

Before compiling and flashing any code to your ESP32, you must ensure that your local non-volatile storage (NVS) is completely wiped. This prevents old Wi-Fi credentials or corrupted device provisions stored in flash memory from interfering with the new firmware.

1. Open your **Arduino IDE**.
2. Navigate to **Tools** on the top menu bar.
3. Find the option **Erase All Flash Before Sketch Upload** and change it to **Enabled**.

### 📸 IDE Configuration Reference
![Arduino IDE Pre-Upload Configuration](Screenshot_2026-06-25_162905.png)

---

## 🌐 1. Provisioning & Connecting Your ESP32

Once the code is successfully flashed, the ESP32 will boot into **AP (Access Point) Provisioning Mode** because it does not yet have network credentials.

1. Open your computer or mobile phone's Wi-Fi network list.
2. Search for and connect to the temporary wireless **Access Point** broadcasted by your ESP32 (typically named something like `Hyperwisor-Setup` or matching your device type).
3. Open the **Hyperwisor User Dashboard** on your web browser.
4. Navigate to the top navigation bar and select the **Product Management** tab.

### 📸 Dashboard Navigation Reference
![Hyperwisor Product Management Navigation](image_310d03.png)

5. Scroll down to find your registered item card (e.g., *Toggle LED*).
6. Under **Quick Actions**, click the **Setup Device** button. Follow the on-screen configuration portal instructions to link the hardware cleanly to your local Wi-Fi router.

### 📸 Device Setup Card Reference
![Hyperwisor Device Setup Interface](image_310dbf.png)

---

## 📲 2. Adding the Project to Your User Dashboard via QR Code

To add a provisioned product directly to your personal user control layout, you can scan its distinct registration matrix token. Manufacturers can locate and distribute this token directly from the development console:

1. Log in to the **Hyperwisor Development Studio / Manufacturer Dashboard**.
2. Select your targeted project (e.g., *LED PWM*).
3. From the left sidebar menu under the **DEPLOY** section, click on **QR & Distribution**.
4. Scan the generated QR code using your smartphone or dashboard camera app to add and authorize the device under your active User Account panel.

### 📸 Deployment & Overview Console Reference
![Hyperwisor Development Studio Overview](image_311488.png)

---

## 👥 3. Understanding Platform User Roles

The Hyperwisor platform manages layouts and hardware parameters based on the account login credentials used. Ensure you log in to the proper portal view depending on your deployment goals:

### A. Manufacturer Login
* **Purpose:** Architecture, debugging, and administration.
* **Privileges:** Access to the **Commands Visual Builder**, **Widget Canvas Designer**, setting baseline schemas, and overseeing raw incoming device configurations. Use this mode when developing your repository assets or updating templates.

### B. User Login
* **Purpose:** Consumer utilization and day-to-day operations.
* **Privileges:** Access to the streamlined, compiled end-user control interface dashboard. This mode is restricted to interacting with widgets (like switches, sliders, and gauges) without risks of altering the core command hierarchy architecture.

---

## 🌲 4. Under the Hood: Commands and Actions

Instead of scattering unmapped variables across abstract registers, this platform establishes a structured hierarchy called a **Command Tree**:

$$\text{Command Category} \longrightarrow \text{Specific Isolated Action}$$

### Command (The Parent Group)
A high-level logical container wrapping an overall operational block. In the base switch project, this container is named **`Operate`**.

### Action (The Specific State Execution)
Distinct operational tasks grouped inside that parent block. The default switch uses two explicit actions:
1. **`Led_On`**: Dispatched by the frontend switch when toggled into the active state.
2. **`Led_Off`**: Dispatched by the frontend switch when flipped back to the resting state.

---

## 💻 5. Firmware Integration Guide

Your micro-controller hooks into the incoming stream inside the `handleCommands()` callback loop. It processes the raw JSON object payloads directly:

```cpp
void handleCommands(JsonObject& msg) {
  JsonObject payload = msg["payload"];

  // 1. Isolate the top-level command category
  JsonObject cmd = device.findCommand(payload, "Operate");
  if (!cmd.isNull()) {
    
    // 2. Filter for the specific inner Action requested
    JsonObject act_on = device.findAction(payload, "Operate", "Led_On");
    if (!act_on.isNull()) {
      Led_On(); // Executes physical pin logic
    }

    JsonObject act_off = device.findAction(payload, "Operate", "Led_Off");
    if (!act_off.isNull()) {
      Led_Off();
    }
  }
}
