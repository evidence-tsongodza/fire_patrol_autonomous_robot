# Autonomous Fire Patrol Robot

> ESP32-based autonomous robot that patrols a room and detects fire threats in real time.

## Features

- Autonomous room patrol with obstacle avoidance
- Smoke detection via MQ-2 sensor
- Temperature monitoring via DHT11
- Dual-sensor validation to eliminate false alarms
- RGB LED + buzzer alarm system
- Real-time Telegram alerts via WiFi (Week 4)

## Hardware

| Component | Model |
|-----------|-------|
| Microcontroller | ESP32 NodeMCU-32S |
| Motor Driver | L298N |
| Chassis | 2WD Robot Kit |
| Smoke Sensor | MQ-2 |
| Temp Sensor | DHT11 |
| Distance Sensor | HC-SR04 |
| Power | 9V Battery |

## Project Structure

```
fire_patrol_robot/
  docs/               — Design notes and decisions
  firmware/           — PlatformIO source code
  hardware/           — Pin assignments and components list
  media/              — Photos and videos
  simulation/         — Wokwi simulation notes
  README.md           — This file
```

## Quick Start

1. Wire components per hardware/pin_assignments.md
2. Open firmware/ folder in VS Code with PlatformIO
3. Disconnect L298N 5V wire, connect USB
4. Run: pio run --target upload
5. Reconnect 5V wire and connect 9V battery
6. Robot starts automatically

## Detection Thresholds

| Sensor | Threshold | Clean Air Value |
|--------|-----------|----------------|
| MQ-2 | 900 ADC | ~500 ADC |
| DHT11 | 35 C | ~25 C |

## Author

Ice — Embedded Systems Project 2026
MDEOF
