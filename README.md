<div align="center">

# 🌅 AURORA NEO

### Smart Neonatal Monitoring System

*A hardware-integrated SaaS dashboard for premature and low-birth-weight infant care*

---

![ESP32](https://img.shields.io/badge/Hardware-ESP32-blue?style=flat-square&logo=espressif)
![MAX30102](https://img.shields.io/badge/Sensor-MAX30102-red?style=flat-square)
![DHT22](https://img.shields.io/badge/Sensor-DHT22-green?style=flat-square)
![HTML](https://img.shields.io/badge/Dashboard-HTML%2FJS-orange?style=flat-square&logo=html5)
![TailwindCSS](https://img.shields.io/badge/Styling-TailwindCSS-38bdf8?style=flat-square&logo=tailwindcss)
![License](https://img.shields.io/badge/License-MIT-purple?style=flat-square)

</div>

---

## 📖 What is AURORA NEO?

AURORA NEO is a **smart neonatal survival and monitoring system** built for the critical early period after birth. It combines an ESP32-based hardware prototype with a premium SaaS-style web dashboard, allowing medical professionals to monitor multiple premature infants simultaneously from a single interface.

The dashboard provides:
- A **ward overview** (Home Screen) showing all monitored babies at a glance
- A **live monitoring view** (Monitor Screen) for the hardware-connected incubator — displaying real-time heart rate, SpO₂, ambient temperature, humidity, and actuator states

> **Hackathon Context:** This system was built as a working prototype. Baby body temperature is simulated in the UI in this iteration; all other sensor readings are fully live from the hardware.

---

## 🗂️ Project Structure

```
AURORA NEO/
├── index.html              ← Home Screen (ward overview, 6 baby cards)
├── monitor.html            ← Live Monitor Screen (Bed 04 — Akhi)
├── aurora_neo_esp32.ino    ← ESP32 firmware (upload this to the hardware)
└── README.md               ← This file
```

---

## 🔌 Hardware Components

| Component | Role |
|-----------|------|
| **ESP32** | Main microcontroller — runs the web server and control logic |
| **MAX30102** | Pulse oximeter — measures heart rate (`bpm`) and SpO₂ (`spo2`) |
| **DHT22** | Temperature & humidity sensor — measures ambient temp (`amb`) and humidity (`hum`) |
| **DS18B20** | Body temperature probe — reads `temp` (currently simulated in UI) |
| **Peltier Module** | Thermoelectric cooler — controlled by relay, state exposed as `peltier` |
| **Humidifier** | Humidity actuator — controlled by relay, state exposed as `humidifier` |
| **2× Relay Modules** | Switch control for Peltier and Humidifier |

---

## ⚡ Pin Configuration

| ESP32 Pin | Connected To |
|-----------|-------------|
| `GPIO 4` | DS18B20 data line |
| `GPIO 16` | DHT22 data line |
| `GPIO 18` | Relay → Peltier module |
| `GPIO 19` | Relay → Humidifier |
| `GPIO 21` | I²C SDA → MAX30102 |
| `GPIO 22` | I²C SCL → MAX30102 |

> **Relay logic:** Both relays are **active-LOW**. `HIGH` = relay OFF, `LOW` = relay ON.

---

## 🌐 Data Contract

The ESP32 serves a `/data` endpoint at `http://192.168.1.45/data`.  
The dashboard polls this endpoint every **3 seconds**.

### Live JSON Response

```json
{
  "temp":       36.5,
  "hum":        60.2,
  "amb":        28.4,
  "bpm":        142,
  "spo2":       98.0,
  "peltier":    "ON",
  "humidifier": "OFF"
}
```

### Field Descriptions

| Field | Label in UI | Source | Notes |
|-------|-------------|--------|-------|
| `temp` | Baby Body Temperature | DS18B20 | **Displayed as simulated** in this iteration |
| `hum` | Humidity | DHT22 | Live |
| `amb` | Ambient Temperature | DHT22 | Live |
| `bpm` | Heart Rate | MAX30102 | Live |
| `spo2` | SpO₂ | MAX30102 | Live |
| `peltier` | Peltier Status | Relay state | `"ON"` or `"OFF"` |
| `humidifier` | Humidifier Status | Relay state | `"ON"` or `"OFF"` |

---

## 🛠️ Setup & Installation

### Prerequisites

Before you start, make sure you have:

- [ ] Arduino IDE installed — [download here](https://www.arduino.cc/en/software)
- [ ] ESP32 board support added to Arduino IDE
- [ ] All required Arduino libraries installed (see below)
- [ ] ESP32 connected to your computer via USB
- [ ] All hardware components wired per the pin table above
- [ ] A WiFi network available (currently configured for `KROZAN`)

---

### Step 1 — Install Arduino Libraries

Open Arduino IDE → **Sketch → Include Library → Manage Libraries**, then search for and install:

| Library | Purpose |
|---------|---------|
| `MAX30105` by SparkFun | MAX30102 pulse oximeter driver |
| `heartRate` *(included with MAX30105)* | Beat detection algorithm |
| `DallasTemperature` by Miles Burton | DS18B20 temperature probe |
| `OneWire` by Jim Studt | 1-Wire bus for DS18B20 |
| `DHT sensor library` by Adafruit | DHT22 temperature & humidity |

---

### Step 2 — Configure WiFi Credentials

Open `aurora_neo_esp32.ino` and update the WiFi settings at the top:

```cpp
const char* ssid     = "KROZAN";       // ← Your WiFi network name
const char* password = "00000000";     // ← Your WiFi password
```

> ⚠️ **Important:** The dashboard is hardcoded to connect to `http://192.168.1.45`.  
> If your ESP32 receives a different IP address, you must update the `BASE_URL` variable in `monitor.html`:
> ```javascript
> const BASE_URL = "http://192.168.1.45";  // ← Change this to your ESP32's IP
> ```
> You can find the actual IP in the Arduino Serial Monitor after upload (see Step 4).

---

### Step 3 — Upload Firmware to ESP32

1. Open `aurora_neo_esp32.ino` in Arduino IDE
2. Go to **Tools → Board → ESP32 Arduino → ESP32 Dev Module**
3. Go to **Tools → Port** and select the COM port for your ESP32
4. Click the **Upload** button `→`
5. Wait for `Done uploading` to appear in the status bar

> 🔄 **Why re-upload?** This version adds CORS response headers to the `/data` endpoint. Without them, browsers will block the dashboard's fetch requests due to cross-origin security policy.

---

### Step 4 — Confirm Hardware is Running

1. In Arduino IDE, go to **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. Press the **EN/RST** button on the ESP32
4. You should see output like:

```
Connecting to WiFi...
WiFi connected!
ESP32 IP: 192.168.1.45
Dashboard data endpoint: http://192.168.1.45/data
Web server started.
```

5. Open a browser and navigate to `http://192.168.1.45/data` — you should see the raw JSON response.

> ✅ If you see JSON in the browser, the hardware is fully operational.

---

### Step 5 — Run the Dashboard

The dashboard is a **static HTML application** — no server, no build step required.

#### Option A — Open directly (simplest)

```
Double-click  →  index.html
```

Or right-click → Open with → Google Chrome.

> ⚠️ Some browsers block fetch requests from `file://` origins. If values don't load on the monitor screen, use Option B.

#### Option B — Serve with Python (recommended for live data)

Open a terminal in the `AURORA NEO` folder and run:

```bash
# Python 3
python -m http.server 8080

# Python 2 (fallback)
python -m SimpleHTTPServer 8080
```


Then open your browser to:

```
http://localhost:8080/index.html
```

#### Option C — Use VS Code Live Server

If you have VS Code with the **Live Server** extension:
1. Right-click `index.html` → **Open with Live Server**
2. The dashboard will open automatically at `http://127.0.0.1:5500`

---

### Step 6 — Connect Dashboard to Hardware

1. Ensure your **computer and ESP32 are on the same WiFi network** (`KROZAN`)
2. Open `http://localhost:8080/monitor.html` (or the equivalent from your chosen method)
3. The header will show **"Connecting…"** briefly
4. Within 3 seconds, it should switch to **"🟢 Live Monitoring"**
5. Values for `bpm`, `spo2`, `amb`, `hum`, `peltier`, and `humidifier` will begin updating

> 🫁 **SpO₂ and Heart Rate:** Place a finger firmly over the MAX30102 sensor window. Values take ~5–10 seconds to stabilize after finger placement.

---

## 🖥️ Dashboard Features

### Home Screen (`index.html`)
- Time-based greeting: *Good morning / Good afternoon / Good evening / Good night, Dr. Shravan*
- 6 patient cards: Bed 01–06 with status badges (Stable / Watch / Critical / Live Monitor)
- **Bed 04 (Akhi)** is the hardware-connected bed — click **Open Live Monitor** to proceed

### Live Monitor Screen (`monitor.html`)

| Tab | Contents |
|-----|----------|
| **Monitor** | Bento-box real-time view of all 7 data fields |
| **Analytics** | Live sparklines for BPM and SpO₂ history, session stats, min/max/avg |
| **Events** | Timestamped event log — connection changes, threshold crossings, actuator switches |
| **Controls** | Read-only display of incubator operating thresholds and current actuator states |

### Alert Logic (UI-side only)

| Metric | Normal | Watch | Critical |
|--------|--------|-------|----------|
| Heart Rate | 100–180 bpm | 80–100 or 180–200 | < 80 or > 200 |
| SpO₂ | ≥ 95% | 90–94% | < 90% |

---

## 🔧 Control Thresholds (Firmware)

The ESP32 automatically controls the Peltier and Humidifier based on these values:

```cpp
float tempUpper    = 32;    // Peltier activates at or above this (°C)
float tempLower    = 30;    // Peltier deactivates at or below this (°C)
float humidityLow  = 55;    // Humidifier activates at or below this (%)
float humidityHigh = 65;    // Humidifier deactivates at or above this (%)
```

To change thresholds, edit these values in `aurora_neo_esp32.ino` and re-upload.

---

## ❓ Troubleshooting

| Problem | Likely Cause | Fix |
|---------|-------------|-----|
| Dashboard shows "Connection Lost" | ESP32 not on same WiFi, or wrong IP | Check Serial Monitor for actual IP; update `BASE_URL` in `monitor.html` |
| `bpm` and `spo2` show `--` | No finger on MAX30102 | Place finger firmly over sensor; wait 10s |
| Dashboard values don't update | Browser blocking `file://` fetch | Serve with Python (Option B) or Live Server |
| ESP32 won't connect to WiFi | Wrong credentials | Verify `ssid` and `password` in `.ino` file |
| Serial Monitor shows garbage | Wrong baud rate | Set to **115200** |
| MAX30102 not found | I²C wiring issue | Check SDA → GPIO21, SCL → GPIO22; ensure 3.3V power |
| Upload fails | Wrong COM port or board | Re-select port and board in Arduino IDE Tools menu |

---

## 🏗️ Built With

- **ESP32** — Microcontroller and web server
- **Arduino IDE** — Firmware development
- **HTML5 + Vanilla JS** — Dashboard core
- **Tailwind CSS** (CDN) — UI styling
- **Plus Jakarta Sans + Be Vietnam Pro** — Typography
- **Google Material Symbols** — Iconography

---

<div align="center">

*Built for the hardware hackathon — AURORA NEO prototype*  
*Medical-grade aesthetics. Real-time hardware integration. Zero deployment overhead.*

</div>
