# Pin Assignments — ESP32 NodeMCU-32S

## Motor Driver (L298N)

| L298N Pin | ESP32 GPIO | Function |
|-----------|-----------|---------|
| ENA | GPIO14 | Left motor speed (PWM) |
| IN1 | GPIO27 | Left motor direction A |
| IN2 | GPIO26 | Left motor direction B |
| IN3 | GPIO25 | Right motor direction A |
| IN4 | GPIO33 | Right motor direction B |
| ENB | GPIO32 | Right motor speed (PWM) |

## Sensors

| Component | Pin | ESP32 GPIO |
|-----------|-----|-----------|
| DHT11 | DATA | GPIO15 |
| MQ-2 | AOUT | GPIO34 |
| HC-SR04 | TRIG | GPIO19 |
| HC-SR04 | ECHO | GPIO21 |

## Outputs

| Component | Pin | ESP32 GPIO |
|-----------|-----|-----------|
| RGB LED | Red | GPIO13 |
| RGB LED | Green | GPIO12 |
| RGB LED | Blue | GPIO4 |
| Regular LED | Anode | GPIO5 |
| Active Buzzer | Positive | GPIO18 |

## Power

| Connection | From | To |
|------------|------|----|
| Motor power | 9V battery (+) | L298N +12V |
| Ground | 9V battery (-) | L298N GND |
| ESP32 power | L298N +5V | ESP32 5V pin |
| Shared ground | L298N GND | ESP32 GND |

## Notes

- GPIO34 is input-only — only use for reading, never output
- Disconnect L298N 5V wire from ESP32 before uploading code via USB
- Never connect 9V directly to ESP32 — maximum is 5V on the 5V pin
- RGB LED uses 330 ohm resistors on each color leg
- Regular LED uses 330 ohm resistor on anode
MDEOF

cat > "/home/claude/robot_docs/hardware/components_list.md" << 'MDEOF'
# Components List

## Main Components

| Component | Model | Quantity | Purpose |
|-----------|-------|----------|---------|
| Microcontroller | ESP32 NodeMCU-32S | 1 | Main brain, WiFi |
| Motor Driver | L298N | 1 | Controls DC motors |
| Robot Chassis | 2WD kit | 1 | Physical platform |
| DC Motors | Included with chassis | 2 | Movement |
| Smoke Sensor | MQ-2 | 1 | Smoke/gas detection |
| Temp Sensor | DHT11 | 1 | Temperature + humidity |
| Ultrasonic Sensor | HC-SR04 | 1 | Obstacle detection |
| Active Buzzer | Generic | 1 | Audible alarm |
| RGB LED | 5mm 4-pin | 1 | Visual status indicator |
| Regular LED | 5mm | 1 | Power/activity indicator |
| Battery | 9V | 1 | Power source |
| Battery Snap | 9V connector | 1 | Battery connection |

## Passive Components

| Component | Value | Quantity | Purpose |
|-----------|-------|----------|---------|
| Resistor | 330 ohm | 4 | LED current limiting |
| Jumper Wires | M-M 20cm | 10 | Connections |
| Jumper Wires | M-F 20cm | 10 | Sensor connections |
| Breadboard | 830 points | 1 | Power rail distribution |

## Software Libraries

| Library | Version | Purpose |
|---------|---------|---------|
| DHT sensor library | Latest | DHT11 reading |
| Adafruit Unified Sensor | Latest | DHT dependency |
| UniversalTelegramBot | Latest | Telegram alerts (Week 4) |
| ArduinoJson | Latest | JSON parsing (Week 4) |

## Power Consumption Estimate

| Component | Current Draw |
|-----------|-------------|
| ESP32 | ~240mA (WiFi active) |
| 2x DC Motors | ~500mA total |
| Sensors | ~50mA total |
| LEDs + Buzzer | ~30mA total |
| Total | ~820mA |

A 9V battery typically provides 500-600mAh — expect 30-45 minutes runtime per battery.
MDEOF
