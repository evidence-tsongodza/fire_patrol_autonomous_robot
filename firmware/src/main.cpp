#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "secrets.h"

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

unsigned long lastStatusUpdate = 0;
const long StatusInterval = 600000; // 10 minutes
bool dangerAlertSent = false;

float temp = 0;
float humidity = 0;
int smoke = 0;

// Motor A (Left side)
#define IN1 27
#define IN2 26
#define ENA 14

// Motor B (Right side)
#define IN3 25
#define IN4 33
#define ENB 32

#define MOTOR_SPEED 150  // 0-255, tune this later

// Code for RGB LED
#define RGB_RED 13
#define RGB_GREEN 12
#define RGB_BLUE 4

// Code for LED
#define LED 5
unsigned long lastLEDBlink = 0;
int ledBlinkStep = 0;
const long LED_BLINK_ON = 100;
const long LED_BLINK_OFF = 100;
const long LED_BLINK_PAUSE = 3000;

// Active alarm state
bool tempDanger = false;
bool smokeDanger = false;

// Code for the sensors
#define MQ2_PIN 34
#define SMOKE_THRESHOLD 850

#define DHTPIN 15
#define DHTTYPE DHT11
#define TEMP_THRESHOLD 40.0

// Code for Ultrasonic Sensor
#define TRIG 19
#define ECHO 21

// Code for Buzzer
#define BUZZER 18

const long UltrasonicInterval = 100;
unsigned long lastUltrasonicRead = 0;

unsigned long lastDHTRead = 0;
unsigned long lastMQ2Read = 0;
const long DHTInterval = 2000;
const long MQ2Interval = 500;

DHT dht(DHTPIN, DHTTYPE);

// ── Patrol state machine ─────────
enum State { PATROL, AVOID, DANGER };
State currentState = PATROL;

void setupMotors() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    ledcSetup(0, 1000, 8); // channel 0, ENA
    ledcSetup(1, 1000, 8); // channel 1, ENB
    ledcAttachPin(ENA, 0);
    ledcAttachPin(ENB, 1);
}

void stopMotors() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    ledcWrite(0, 0);
    ledcWrite(1, 0);
}

void forward() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    ledcWrite(0, MOTOR_SPEED);
    ledcWrite(1, MOTOR_SPEED);
}

void backward() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    ledcWrite(0, MOTOR_SPEED);
    ledcWrite(1, MOTOR_SPEED);
}

void turn_left() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    ledcWrite(0, MOTOR_SPEED);
    ledcWrite(1, MOTOR_SPEED);
}

void turn_right() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    ledcWrite(0, MOTOR_SPEED);
    ledcWrite(1, MOTOR_SPEED);
}

void setRGB(bool red, bool green, bool blue) {
    digitalWrite(RGB_RED, red);
    digitalWrite(RGB_GREEN, green);
    digitalWrite(RGB_BLUE, blue);
}

void updateLED(unsigned long currentMillis) {
    const long blinkDurations[5] = {LED_BLINK_ON, LED_BLINK_OFF, LED_BLINK_ON, LED_BLINK_OFF, LED_BLINK_PAUSE};
    const bool blinkStates[5] = {HIGH, LOW, HIGH, LOW, LOW};

    if (currentMillis - lastLEDBlink >= blinkDurations[ledBlinkStep]) {
        lastLEDBlink = currentMillis;
        ledBlinkStep = (ledBlinkStep + 1) % 5;
        digitalWrite(LED, blinkStates[ledBlinkStep]);
    }
}

// Returns distance in cm
float getDistance() {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    long duration = pulseIn(ECHO, HIGH);
    return duration * 0.034 / 2;
}


void sendDangerAlert() {
    String message = "*** FIRE DETECTED ***\n";
    message += "Smoke level: " + String(smoke) + "\n";
    message += "Temperature: " + String(temp) + " C\n";
    message += "Robot has stopped and alarm is sounding.";
    
    Serial.println("Attempting to send Telegram alert...");
    bool sent = bot.sendMessage(CHAT_ID, message, "");
    
    if (sent) {
        Serial.println("Telegram alert sent successfully!");
    } else {
        Serial.println("Telegram alert FAILED to send!");
    }
}

void sendStatusUpdate() {
    String message = "Status Update\n";
    message += "State: ";
    if (currentState == PATROL) message += "Patrolling\n";
    else if (currentState == AVOID) message += "Avoiding obstacle\n";
    else if (currentState == DANGER) message += "DANGER\n";
    message += "Temperature: " + String(temp) + " C\n";

    message += "Smoke level: " + String(smoke) + "\n";
    message += "All systems normal.";
    bot.sendMessage(CHAT_ID, message, "");
}

void setup() {
    Serial.begin(9600);
    setupMotors();
    Serial.println("System ready.");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());

    secured_client.setInsecure(); // skip SSL certificate check

    bot.sendMessage(CHAT_ID, "Fire Patrol Robot is online and starting patrol.", "");

    // Code for RGB LED
    pinMode(RGB_RED, OUTPUT);
    pinMode(RGB_GREEN, OUTPUT);
    pinMode(RGB_BLUE, OUTPUT);

    // Code for LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    // code for the sensors
    dht.begin();

    // Code for Ultrasonic Sensor
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    // Code for Buzzer
    pinMode(BUZZER, OUTPUT);

    randomSeed(analogRead(0));
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastStatusUpdate >= StatusInterval) {
    lastStatusUpdate = currentMillis;
    sendStatusUpdate();
}
    // ── DHT11 every 2s ───────────────────────────────────────────
    if (currentMillis - lastDHTRead >= DHTInterval) {
        lastDHTRead = currentMillis;
        temp = dht.readTemperature();

        if (isnan(temp)) {
            Serial.println("DHT read failed!");
        } else {
            Serial.print("Temp: "); Serial.print(temp);
            Serial.println("C");

            if (temp > TEMP_THRESHOLD) {
                tempDanger = true;
                Serial.println("DANGER — High temperature!");
            } else {
                tempDanger = false;
                Serial.println("Temperature normal.");
            }
        }
    }

    // ── MQ2 every 500ms ──────────────────────────────────────────
    if (currentMillis - lastMQ2Read >= MQ2Interval) {
        lastMQ2Read = currentMillis;
        smoke = analogRead(MQ2_PIN);
        Serial.print("Smoke: "); Serial.println(smoke);

        if (smoke > SMOKE_THRESHOLD) {
            smokeDanger = true;
            Serial.println("DANGER — Smoke detected!");
        } else {
            smokeDanger = false;
            Serial.println("Air clear.");
        }
    }

    // ── LED blink ─────────────────────────────────────────────────
    updateLED(currentMillis);

    // ── Determine state ───────────────────────────────────────────
    if (tempDanger && smokeDanger || smoke > 1500 || temp > 50) {
        currentState = DANGER;
    } else if (currentState == DANGER && !tempDanger && !smokeDanger) {
        currentState = PATROL;
        dangerAlertSent = false;
        Serial.println("Danger cleared — resuming patrol");
    }

    // ── Ultrasonic every 100ms ────────────────────────────────────
    if (currentMillis - lastUltrasonicRead >= UltrasonicInterval) {
        lastUltrasonicRead = currentMillis;
        float distance = getDistance();
        Serial.print("Distance: "); Serial.print(distance); Serial.println(" cm");

        if (currentState != DANGER && distance < 20) {
            currentState = AVOID;
        } else if (currentState == AVOID && distance >= 20) {
            currentState = PATROL;
        }
    }

    // ── State machine ─────────────────────────────────────────────
    switch (currentState) {

        case PATROL:
            setRGB(0, 1, 0);
            digitalWrite(BUZZER, LOW);
            forward();
            break;

        case AVOID:
            setRGB(1, 0, 0);
            digitalWrite(BUZZER, LOW);
            stopMotors();
            delay(300);
            backward();
            delay(300);
            stopMotors();
            if (random(2) == 0) {
                turn_left();
                Serial.println("Avoiding — turning left");
            } else {
                turn_right();
                Serial.println("Avoiding — turning right");
            }
            delay(250);
            stopMotors();
            currentState = PATROL;
            break;

        case DANGER:
            stopMotors();
            setRGB(1, 0, 0);
            digitalWrite(BUZZER, HIGH);
            Serial.println("*** DANGER — FIRE DETECTED ***");
            if (!dangerAlertSent) {
            sendDangerAlert();
            dangerAlertSent = true;
            }
            Serial.print("Smoke: "); Serial.print(smoke);
            Serial.print(" | Temp: "); Serial.println(temp);
            break;
    }
}