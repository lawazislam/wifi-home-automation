# Voice-Controlled WiFi Home Automation

Firmware for a NodeMCU ESP8266 that switches household appliances remotely over WiFi, through an Android app's buttons or voice commands. Four LEDs stand in for four real appliances on the breadboard.

## How it works

The NodeMCU joins the local WiFi network, then runs a small web server. When the user presses an ON or OFF button in the Android app, or speaks a command the app converts to the same action, the app sends an HTTP request to the NodeMCU's IP address:

```
http://<NodeMCU-IP>/set?relay=1&state=1   (relay 1 ON)
http://<NodeMCU-IP>/set?relay=1&state=0   (relay 1 OFF)
```

The NodeMCU reads the request and drives the matching GPIO pin high or low, which energizes or releases the corresponding relay on a four-channel relay module and switches the connected LED. Because control runs over WiFi, the app works from anywhere on the same network as the receiver, and opening the NodeMCU's IP in any browser shows the same on/off controls without the app.

## Hardware

- NodeMCU ESP8266-12E development board
- Four-channel relay module
- Four LEDs as appliance stand-ins, each with a 220-ohm series resistor
- Breadboard, jumper wires, regulated power supply

## Wiring

| Relay module | NodeMCU pin |
|---|---|
| IN1 | D1 |
| IN2 | D2 |
| IN3 | D5 |
| IN4 | D6 |
| VCC | Vin (5V) |
| GND | GND (common ground with NodeMCU) |

D1, D2, D5 and D6 are used deliberately: D3, D4 and D8 are boot-sensitive pins that can prevent the NodeMCU from starting if pulled the wrong way at power-up.

## Setup

1. Install the ESP8266 board core in Arduino IDE.
2. Open `wifi_home_automation.ino`, set `WIFI_SSID` and `WIFI_PASS` at the top.
3. Upload to the NodeMCU.
4. Open the Serial Monitor at 115200 baud. It prints the IP address the board was assigned; enter that IP into the Android control app (or a browser).

## My contribution

Solo project. I wrote the firmware (WiFi connection handling, the web server, the relay-switching logic), wired the relay module and LEDs, and configured the Android app's voice input to call the same HTTP endpoints as its buttons.

Full project report (with circuit diagram): [lawazislam.com/projects](https://lawazislam.com/projects)
