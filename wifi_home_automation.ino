/* ============================================================================
   Voice-Controlled WiFi Home Automation - NodeMCU ESP8266 Firmware
   ----------------------------------------------------------------------------
   Controls a 4-channel relay module over WiFi. Each relay switches one
   appliance (here, one LED on the breadboard). The NodeMCU runs a small web
   server; the Android app (and its voice input) sends HTTP requests to turn
   each channel ON or OFF. You can also control it from any browser on the
   same network by opening the NodeMCU's IP address.

   How control works:
   - Browser/app calls: http://<NodeMCU-IP>/set?relay=1&state=1 (relay 1 ON)
                         http://<NodeMCU-IP>/set?relay=1&state=0 (relay 1 OFF)
   - Opening http://<NodeMCU-IP>/ shows on/off buttons for all 4 channels.

   Board: NodeMCU 1.0 (ESP-12E). Install the ESP8266 core in Arduino IDE,
   set your WiFi name/password below, upload, then open the Serial Monitor
   at 115200 baud to see the assigned IP address.
   ============================================================================ */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ---- 1. Set your WiFi credentials -----------------------------------------
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

// ---- 2. Wiring (match your four jumper wires to these pins) ----------------
// Relay module IN1 -> NodeMCU D1
// Relay module IN2 -> NodeMCU D2
// Relay module IN3 -> NodeMCU D5
// Relay module IN4 -> NodeMCU D6
// Relay module VCC -> NodeMCU Vin (5V)
// Relay module GND -> NodeMCU GND (grounds MUST be common)
// D1, D2, D5, D6 are safe general-purpose pins. Avoid D3, D4, and D8 for
// relay inputs: they are boot-sensitive and can stop the NodeMCU from starting.
const int relayPins[4] = { D1, D2, D5, D6 };

// Most 4-channel relay boards are ACTIVE-LOW: a LOW signal turns the relay ON.
// If your relays behave backwards, change this to false.
const bool ACTIVE_LOW = true;

// Current state of each channel (false = OFF, true = ON).
bool relayState[4] = { false, false, false, false };

ESP8266WebServer server(80);

// ---- Drive one relay to match its stored state -----------------------------
void applyRelay(int i) {
  int level;
  if (ACTIVE_LOW) level = relayState[i] ? LOW : HIGH;   // active-low board
  else            level = relayState[i] ? HIGH : LOW;   // active-high board
  digitalWrite(relayPins[i], level);
}

// ---- Build the simple control web page -------------------------------------
String buildPage() {
  String html = F("<!DOCTYPE html><html><head><meta name='viewport' "
                   "content='width=device-width,initial-scale=1'>"
                   "<title>Home Automation</title></head><body>"
                   "<h2>WiFi Home Automation</h2>");
  for (int i = 0; i < 4; i++) {
    int n = i + 1;
    html += "<p>Relay " + String(n) + ": <b>" +
            (relayState[i] ? "ON" : "OFF") + "</b> &nbsp; ";
    html += "<a href='/set?relay=" + String(n) + "&state=1'>[ON]</a> ";
    html += "<a href='/set?relay=" + String(n) + "&state=0'>[OFF]</a></p>";
  }
  html += F("</body></html>");
  return html;
}

void handleRoot() {
  server.send(200, "text/html", buildPage());
}

// ---- Handle on/off requests from the app or browser ------------------------
void handleSet() {
  if (server.hasArg("relay") && server.hasArg("state")) {
    int r = server.arg("relay").toInt();  // 1..4
    int s = server.arg("state").toInt();  // 1 = ON, 0 = OFF
    if (r >= 1 && r <= 4) {
      relayState[r - 1] = (s == 1);
      applyRelay(r - 1);
      Serial.printf("Relay %d -> %s\n", r, relayState[r - 1] ? "ON" : "OFF");
    }
  }
  // Send the user back to the main page.
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);

  // Start every relay in a known OFF state.
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    relayState[i] = false;
    applyRelay(i);
  }

  // Connect to WiFi.
  Serial.print("\nConnecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected.");
  Serial.print("Open this IP in your app or browser: ");
  Serial.println(WiFi.localIP());  // <-- put this IP into the Android app

  // Register routes and start the server.
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  server.handleClient();  // process incoming requests
}
