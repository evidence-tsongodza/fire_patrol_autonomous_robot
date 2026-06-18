# Firmware Architecture

## Overview

The firmware is structured as a single-file Arduino sketch using a non-blocking
event loop pattern. All timing is handled with millis() to allow simultaneous
sensor reading, motor control, and output management.

## File Structure

```
firmware/
  src/
    main.cpp        — Main firmware file
  platformio.ini    — Board config and library dependencies
  README.md         — Upload instructions
```

## State Machine

The robot operates in 3 states:

```
PATROL ──── obstacle detected ──── AVOID ──── clear ──── PATROL
   |
   |──── smoke > 900 AND temp > 35 ──── DANGER ──── reset ──── PATROL
```

| State | Motors | RGB | Buzzer | Serial |
|-------|--------|-----|--------|--------|
| PATROL | Moving | Green | Off | Zone + readings |
| AVOID | Stop/Turn | Blue | Off | "Obstacle" |
| DANGER | Stopped | Red | On | "DANGER" |

## Timing (millis intervals)

| Task | Interval |
|------|---------|
| DHT11 read | 2000ms |
| MQ-2 read | 500ms |
| Ultrasonic read | 100ms |
| LED blink | 500ms |
| Patrol zone change | 3000ms |

## Key Functions

| Function | Purpose |
|----------|---------|
| setupMotors() | Initialize PWM channels and GPIO |
| forward() | Both motors forward |
| backward() | Both motors backward |
| turnLeft() | Left motor back, right forward |
| turnRight() | Right motor back, left forward |
| stopMotors() | Cut all motor power |
| getDistance() | HC-SR04 pulse measurement |
| setRGB(r,g,b) | Set RGB LED color |
| dangerAlert() | Red LED + buzzer on |
| allClear() | Green LED + buzzer off |

## platformio.ini

```ini
[env:nodemcu-32s]
platform = espressif32
board = nodemcu-32s
framework = arduino
upload_speed = 115200
upload_port = /dev/ttyUSB0
lib_deps =
    adafruit/DHT sensor library
    adafruit/Adafruit Unified Sensor
```

## Upload Instructions

1. Disconnect L298N 5V wire from ESP32
2. Connect USB cable to PC
3. Run: pio run --target upload
4. If upload fails: hold BOOT button, press EN, release BOOT, retry upload
5. After upload: disconnect USB, reconnect 5V wire, connect battery
MDEOF

cat > "/home/claude/robot_docs/firmware/troubleshooting.md" << 'MDEOF'
# Firmware Troubleshooting

## Upload Issues

### "Unable to verify flash chip connection"
- Cause: ESP32 not in flash mode
- Fix: Hold BOOT button during upload when "Connecting..." appears

### "Packet content transfer stopped"
- Cause: Upload speed too high or port conflict
- Fix: Set upload_speed = 115200 in platformio.ini

### "Failed to open serial port /dev/ttyS0"
- Cause: Wrong port selected in serial monitor
- Fix: Change port to /dev/ttyUSB0

### Upload works only when other wires disconnected
- Cause: L298N 5V fighting with USB 5V (two power sources)
- Fix: Always disconnect L298N 5V wire before uploading

## Sensor Issues

### DHT11 reads "nan" or "Sensor read failed"
- Check 3.3V on VCC pin (not 5V)
- Check DATA wire on GPIO15
- DHT11 needs 1-2 seconds warmup after power on

### MQ-2 always reads high values
- Sensor needs 24-48 hours burn-in period when new
- Let it run powered for a few hours before calibrating

### Ultrasonic reads 0 or very large numbers
- Check TRIG on GPIO19 and ECHO on GPIO21
- Ensure nothing is closer than 2cm to sensor

## Motor Issues

### Motors not spinning
- Check ENA and ENB jumpers are REMOVED
- Check ledcAttach is used not analogWrite
- Verify 9V battery is connected to L298N +12V

### Motors spinning wrong direction
- Swap the two wires on that motor at OUT terminals
- No code change needed

### Robot drifts left or right
- Adjust MOTOR_SPEED individually per side
- Add separate LEFT_SPEED and RIGHT_SPEED defines
MDEOF
