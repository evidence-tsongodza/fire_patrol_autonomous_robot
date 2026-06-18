# Autonomous Fire Patrol Robot — Project Overview

## Project Summary

An autonomous wheeled robot built on an ESP32 NodeMCU-32S that patrols a room,
detects fire and smoke threats using multiple sensors, sounds an alarm, and sends
real-time alerts via WiFi and Telegram.

## Problem Statement

Fires can start anywhere in a room and cause significant damage before being noticed,
especially when the occupant is asleep, studying, or away. Fixed smoke detectors only
cover one spot. This robot solves that by actively patrolling, covering the entire room,
and detecting threats from multiple positions.

## Objectives

- Build a mobile robot that autonomously patrols a room
- Detect smoke using MQ-2 gas sensor
- Detect abnormal temperature using DHT11
- Avoid obstacles using HC-SR04 ultrasonic sensor
- Alert via buzzer, RGB LED, and Telegram notification
- Run fully autonomously without human intervention

## Key Concepts Demonstrated

| Concept | Implementation |
|---------|---------------|
| Sensor fusion | MQ-2 + DHT11 dual confirmation before alert |
| Autonomous navigation | Timer-based patrol with obstacle avoidance |
| Non-blocking code | millis() instead of delay() throughout |
| Embedded networking | ESP32 WiFi + Telegram Bot API |
| PWM motor control | ledcAttach/ledcWrite for L298N speed control |
| State machine | PATROL, DANGER, HALT states |

## Project Timeline

| Week | Focus | Status |
|------|-------|--------|
| Week 1 | PC setup, simulation, motor code | Done |
| Week 2 | Sensors, detection, alarm | Done |
| Week 3 | Autonomous patrol, obstacle avoidance | In progress |
| Week 4 | WiFi alerts, documentation, demo | Pending |

## Tools and Technologies

- Microcontroller: ESP32 NodeMCU-32S
- IDE: VS Code + PlatformIO
- Framework: Arduino
- Notification: Telegram Bot API
- Version Control: Git + GitHub

## Author

Ice — Embedded Systems Project 2026
MDEOF

cat > "/home/claude/robot_docs/docs/design_decisions.md" << 'MDEOF'
# Design Decisions

## Why ESP32 over Arduino UNO?

The Arduino UNO has no WiFi capability. The ESP32 provides built-in WiFi and Bluetooth,
dual-core processor, more GPIO pins, and analog ADC pins needed for the MQ-2 sensor.
This allows the robot to send Telegram alerts without any additional modules.

## Why 2WD instead of 4WD?

The available chassis was a 2WD kit. For room patrol purposes, 2WD is sufficient —
it reduces motor driver complexity and power consumption while still providing
full directional control (forward, backward, left pivot, right pivot).

## Why dual-sensor confirmation before alarm?

Using only smoke OR temperature triggers too many false alarms. A candle or cooking
smoke could trigger a single sensor. Requiring BOTH MQ-2 AND DHT11 to exceed their
thresholds simultaneously makes the system far more reliable and trustworthy.

## Why millis() instead of delay()?

delay() blocks the entire microcontroller — no sensor reads, no obstacle detection,
no serial output during the wait. millis() allows the robot to check all sensors,
detect obstacles, and control motors simultaneously in a single fast loop.

## Why L298N motor driver?

The L298N is widely available, well documented, handles 2A per channel (sufficient
for small DC motors), and conveniently provides a regulated 5V output that powers
the ESP32 directly — eliminating the need for a separate voltage regulator.

## Sensor Threshold Decisions

| Sensor | Clean Value | Danger Value | Threshold Set |
|--------|------------|--------------|---------------|
| MQ-2 | ~500 ADC | ~1300 ADC | 900 ADC |
| DHT11 | ~25 C | >35 C | 35 C |

Thresholds were determined by physical calibration testing on the actual hardware,
not from datasheet estimates.

## Pin Assignment Rationale

GPIO34 was chosen for MQ-2 because it is an input-only ADC pin — perfect for
analog sensor reading. GPIO14, 27, 26, 25, 33, 32 were chosen for motors because
they are all PWM-capable and do not interfere with boot strapping on the ESP32.
MDEOF
