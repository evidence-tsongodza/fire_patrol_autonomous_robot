# Simulation Notes

## Platform Used

Wokwi — https://wokwi.com (browser-based ESP32 simulator)

## What Was Simulated

| Component | Simulation Method |
|-----------|-----------------|
| DHT22 (DHT11 equivalent) | Native Wokwi DHT22 component |
| MQ-2 smoke sensor | Potentiometer on GPIO34 (analog voltage simulation) |
| ESP32 | Native Wokwi ESP32 board |

## Simulation Limitations

- Wokwi does not have a native MQ-2 component — potentiometer was used to simulate analog output
- Motor driver (L298N) simulation was skipped — motor direction testing done on physical hardware
- DHT22 was used in simulation instead of DHT11 (same library, same code, minor difference)

## Test Scenarios Verified in Simulation

| Scenario | Expected Result | Verified |
|----------|----------------|---------|
| Low smoke + low temp | "All clear" printed | Yes |
| High smoke only | "WARNING — Smoke only" | Yes |
| High temp only | "WARNING — High temp only" | Yes |
| High smoke + high temp | "DANGER — FIRE DETECTED" | Yes |

## How to Re-run Simulation

1. Go to https://wokwi.com
2. Create new ESP32 project
3. Add DHT22 component — wire VCC to 3.3V, GND to GND, DATA to GPIO15
4. Add Potentiometer — wire VCC to 3.3V, GND to GND, SIG to GPIO34
5. Paste sensor code from firmware/src/main.cpp
6. Click Run and use component sliders to test scenarios
MDEOF

cat > "/home/claude/robot_docs/media/media_index.md" << 'MDEOF'
# Media Index

## Photos

Add your robot photos here. Suggested shots:

- photo_chassis_assembly.jpg — Chassis with motors attached
- photo_l298n_wiring.jpg — L298N mounted on chassis with wires
- photo_full_wiring.jpg — Complete robot with all sensors connected
- photo_robot_top_view.jpg — Top view showing all components
- photo_robot_side_view.jpg — Side profile
- photo_serial_monitor.jpg — Serial monitor showing sensor readings
- photo_telegram_alert.jpg — Phone showing Telegram alert received

## Videos

- video_motor_test.mp4 — Robot moving forward/backward/left/right
- video_obstacle_avoidance.mp4 — Robot detecting and avoiding obstacle
- video_fire_detection.mp4 — Smoke near sensor triggers alarm
- video_full_patrol.mp4 — Complete autonomous patrol demo
- video_telegram_demo.mp4 — End-to-end: smoke detected, Telegram received

## Notes

Place all media files in this folder.
Name files clearly so they match this index.
MDEOF
