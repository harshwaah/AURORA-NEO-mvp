/*
 * AURORA NEO — ESP32 Firmware (CORS-enabled)
 * Updated: CORS headers added to /data endpoint so the
 * dashboard can fetch from any browser origin without being blocked.
 *
 * REQUIRED ACTION: Upload this updated sketch to your ESP32.
 * Steps:
 *   1. Open this file in Arduino IDE
 *   2. Select board: ESP32 Dev Module
 *   3. Select the correct COM port
 *   4. Click Upload (→)
 *   5. Open Serial Monitor at 115200 baud to confirm IP address
 */

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

// ── WIFI ─────────────────────────────────────────────────────────────────────
const char* ssid     = "KROZAN";
const char* password = "00000000";

WebServer server(80);

// ── PINS ─────────────────────────────────────────────────────────────────────
#define DS18B20_PIN      4
#define RELAY_PELTIER    18
#define RELAY_HUMIDIFIER 19
#define DHT_PIN          16
#define DHT_TYPE         DHT22

// ── THRESHOLDS ───────────────────────────────────────────────────────────────
float tempUpper    = 32;
float tempLower    = 30;
float humidityLow  = 55;
float humidityHigh = 65;

// ── SENSORS ──────────────────────────────────────────────────────────────────
MAX30105 maxSensor;
OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);
DHT dht(DHT_PIN, DHT_TYPE);

// ── VARIABLES ────────────────────────────────────────────────────────────────
float bodyTemp  = 0;
float humidity  = 0;
float ambientTemp = 0;
int   bpm       = 0;
float spo2      = 0;

bool peltierState    = false;
bool humidifierState = false;

// ── HEART RATE BUFFER ────────────────────────────────────────────────────────
const byte RATE_SIZE = 4;
byte  rates[RATE_SIZE];
byte  rateSpot  = 0;
long  lastBeat  = 0;

// ── TIMER ────────────────────────────────────────────────────────────────────
unsigned long lastUpdate = 0;
const int     interval   = 3000;

// ── CORS HEADERS ─────────────────────────────────────────────────────────────
// These headers tell the browser it is allowed to read the response
// even when the dashboard is opened from a different origin (file://, localhost, etc.)
void addCORSHeaders() {
  server.sendHeader("Access-Control-Allow-Origin",  "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

// ── DASHBOARD HTML ───────────────────────────────────────────────────────────
// Kept for direct access at http://192.168.1.45/
// The full SaaS dashboard is served separately as index.html + monitor.html
String webpage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>AURORA NEO — Smart Baby Incubator</title>
<meta name="description" content="Direct ESP32 telemetry server for AURORA NEO smart neonatal incubator">
<meta name="theme-color" content="#0b132b">
<link rel="icon" type="image/svg+xml" href="data:image/svg+xml,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 32 32' width='32' height='32'><defs><linearGradient id='a' x1='0' y1='1' x2='1' y2='0'><stop offset='0' stop-color='%237e5b64'/><stop offset='0.5' stop-color='%23c12048'/><stop offset='1' stop-color='%23ffd1dc'/></linearGradient><linearGradient id='b' x1='0' y1='0' x2='1' y2='0'><stop offset='0' stop-color='%23ffd1dc'/><stop offset='0.5' stop-color='%2310b981'/><stop offset='1' stop-color='%23ffffff'/></linearGradient><radialGradient id='g' cx='50%25' cy='50%25' r='50%25'><stop offset='0' stop-color='%23ffd1dc' stop-opacity='0.9'/><stop offset='1' stop-color='%23ffd1dc' stop-opacity='0'/></radialGradient></defs><rect width='32' height='32' rx='8' fill='%231b1e2e'/><circle cx='16' cy='11' r='5' fill='url(%23g)'/><path d='M6.5 21.5 C6.5 12.5 25.5 12.5 25.5 21.5' fill='none' stroke='url(%23a)' stroke-width='2.5' stroke-linecap='round'/><path d='M7 21.5 H12 L14.5 15.5 L17.5 25.5 L20 19 L22 21.5 H25' fill='none' stroke='url(%23b)' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'/><circle cx='16' cy='10.5' r='2' fill='%23ffd1dc'/></svg>">
<style>
body{font-family:Arial;text-align:center;background:#0b132b;color:white;}
.card{background:#1c2541;padding:20px;margin:20px;border-radius:15px;font-size:22px;}
</style>
<script>
setInterval(function(){
  fetch("/data")
    .then(r => r.json())
    .then(data => {
      document.getElementById("temp").innerHTML       = data.temp;
      document.getElementById("hum").innerHTML        = data.hum;
      document.getElementById("amb").innerHTML        = data.amb;
      document.getElementById("bpm").innerHTML        = data.bpm;
      document.getElementById("spo2").innerHTML       = data.spo2;
      document.getElementById("peltier").innerHTML    = data.peltier;
      document.getElementById("humidifier").innerHTML = data.humidifier;
    });
}, 3000);
</script>
</head>
<body>
<h1>AURORA NEO — Smart Baby Incubator</h1>
<div class="card">Body Temperature: <span id="temp">--</span> °C</div>
<div class="card">Humidity: <span id="hum">--</span> %</div>
<div class="card">Ambient Temp: <span id="amb">--</span> °C</div>
<div class="card">Heart Rate: <span id="bpm">--</span> BPM</div>
<div class="card">SpO2: <span id="spo2">--</span> %</div>
<div class="card">Peltier: <span id="peltier">--</span></div>
<div class="card">Humidifier: <span id="humidifier">--</span></div>
</body>
</html>
)rawliteral";

// ── SENSOR FUNCTIONS ─────────────────────────────────────────────────────────
void readHeart() {
  long irValue  = maxSensor.getIR();
  long redValue = maxSensor.getRed();

  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat   = millis();
    float beat = 60 / (delta / 1000.0);

    if (beat > 20 && beat < 255) {
      rates[rateSpot++] = (byte)beat;
      rateSpot %= RATE_SIZE;
      int avg = 0;
      for (byte i = 0; i < RATE_SIZE; i++) avg += rates[i];
      bpm = avg / RATE_SIZE;
    }
  }

  if (irValue > 50000) {
    float ratio = (float)redValue / (float)irValue;
    float calc  = 110 - (25 * ratio);
    if (calc > 100) calc = 100;
    if (calc < 80)  calc = 80;
    spo2 = calc;
  }
}

void readTemp() {
  ds18b20.requestTemperatures();
  bodyTemp = ds18b20.getTempCByIndex(0);
}

void readHum() {
  ambientTemp = dht.readTemperature();
  humidity    = dht.readHumidity();
}

// ── CONTROL LOGIC ─────────────────────────────────────────────────────────────
void controlTemp() {
  if (bodyTemp >= tempUpper) {
    digitalWrite(RELAY_PELTIER, LOW);
    peltierState = true;
  }
  if (bodyTemp <= tempLower) {
    digitalWrite(RELAY_PELTIER, HIGH);
    peltierState = false;
  }
}

void controlHum() {
  if (humidity <= humidityLow) {
    digitalWrite(RELAY_HUMIDIFIER, LOW);
    humidifierState = true;
  }
  if (humidity >= humidityHigh) {
    digitalWrite(RELAY_HUMIDIFIER, HIGH);
    humidifierState = false;
  }
}

// ── WEB HANDLERS ─────────────────────────────────────────────────────────────
void handleRoot() {
  addCORSHeaders();
  server.send(200, "text/html", webpage);
}

void handleData() {
  // ↓ CORS headers — required so browsers can fetch this from dashboard
  addCORSHeaders();

  String json = "{";
  json += "\"temp\":"       + String(bodyTemp)                           + ",";
  json += "\"hum\":"        + String(humidity)                           + ",";
  json += "\"amb\":"        + String(ambientTemp)                        + ",";
  json += "\"bpm\":"        + String(bpm)                                + ",";
  json += "\"spo2\":"       + String(spo2)                               + ",";
  json += "\"peltier\":\""  + String(peltierState    ? "ON" : "OFF")    + "\",";
  json += "\"humidifier\":\"" + String(humidifierState ? "ON" : "OFF")  + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

// Handles browser preflight OPTIONS requests (sent before cross-origin GETs)
void handleOptions() {
  addCORSHeaders();
  server.send(204);   // No Content — just headers are enough
}

// Handles browser favicon requests directly to avoid 404 logs
void handleFavicon() {
  addCORSHeaders();
  server.send(204);   // No Content — favicon is embedded as SVG data URI in webpage
}

// ── SETUP ─────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  ds18b20.begin();
  dht.begin();

  pinMode(RELAY_PELTIER,    OUTPUT);
  pinMode(RELAY_HUMIDIFIER, OUTPUT);

  // Relays are active-LOW — HIGH = OFF
  digitalWrite(RELAY_PELTIER,    HIGH);
  digitalWrite(RELAY_HUMIDIFIER, HIGH);

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Dashboard data endpoint: http://" + WiFi.localIP().toString() + "/data");

  // Register routes
  server.on("/",            HTTP_GET,     handleRoot);
  server.on("/data",        HTTP_GET,     handleData);
  server.on("/data",        HTTP_OPTIONS, handleOptions);  // CORS preflight
  server.on("/favicon.ico", HTTP_GET,     handleFavicon);  // Browser favicon

  server.begin();
  Serial.println("Web server started.");

  // MAX30102
  if (!maxSensor.begin(Wire)) {
    Serial.println("ERROR: MAX30102 not found. Check wiring.");
    while (1);
  }
  maxSensor.setup();
}

// ── LOOP ──────────────────────────────────────────────────────────────────────
void loop() {
  server.handleClient();
  readHeart();

  if (millis() - lastUpdate > interval) {
    lastUpdate = millis();
    readTemp();
    readHum();
    controlTemp();
    controlHum();
  }
}
