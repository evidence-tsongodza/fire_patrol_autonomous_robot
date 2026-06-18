#include <Arduino.h>
#include <DHT.h>

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

#define MOTOR_SPEED 200  // 0-255, tune this later

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
#define TEMP_THRESHOLD 45.0

// Code for Ultrasonic Sensor
#define TRIG 19
#define ECHO 21

// Code for Buzzer
#define BUZZER 18

const long UltrasonicInterval = 100;
unsigned long lastUltrasonicRead = 0;

// Buzzer beep timing (milliseconds)
const long BUZZER_ON_DURATION = 100;
const long BUZZER_OFF_DURATION = 100;
unsigned long lastBuzzerToggle = 0;
bool buzzerState = false;

unsigned long lastDHTRead = 0;
unsigned long lastMQ2Read = 0;
const long DHTInterval = 2000;
const long MQ2Interval = 500;

DHT dht(DHTPIN, DHTTYPE);

void setupMotors() {
    pinMode(IN1, OUTPUT); 
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
}

void stopMotors() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}

void forward() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, MOTOR_SPEED);
    analogWrite(ENB, MOTOR_SPEED);
}

void backward() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, MOTOR_SPEED);
    analogWrite(ENB, MOTOR_SPEED);
}

void turn_left() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, MOTOR_SPEED);
    analogWrite(ENB, MOTOR_SPEED);
}

void turn_right() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, MOTOR_SPEED);
    analogWrite(ENB, MOTOR_SPEED);
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

float getDistance() {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    long duration = pulseIn(ECHO, HIGH);
    return duration * 0.034 / 2;
}

void setup() {
    Serial.begin(9600);
    setupMotors();
    Serial.println("System ready.");

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
}

void loop() {

    unsigned long currentMillis = millis();

    // DHT11 reads every 2 seconds
    if (currentMillis - lastDHTRead >= DHTInterval) {
        lastDHTRead = currentMillis;

        temp = dht.readTemperature();
        humidity = dht.readHumidity();

        if (isnan(temp) || isnan(humidity)) {
            Serial.println("DHT read failed!");
        } else {
            Serial.print("Temp: ");
            Serial.print(temp);
            Serial.print("C  |  Humidity: ");
            Serial.print(humidity);
            Serial.println("%");

            if (temp > TEMP_THRESHOLD) {
                Serial.println("DANGER — High temperature!");
                tempDanger = true;
            } else {
                Serial.println("Temperature normal.");
                tempDanger = false;
            }
        }
    }

    // MQ2 reads every 500ms
    if (currentMillis - lastMQ2Read >= MQ2Interval) {
        lastMQ2Read = currentMillis;

        smoke = analogRead(MQ2_PIN);
        Serial.print("Smoke: ");
        Serial.println(smoke);

        if (smoke > SMOKE_THRESHOLD) {
            Serial.println("DANGER — Smoke detected!");
            smokeDanger = true;
        } else {
            Serial.println("Air clear.");
            smokeDanger = false;
        }
    }

    // LED blink pattern: two quick blinks, then 2-second pause
    updateLED(currentMillis);

    // Ultrasonic read + obstacle logic
    if (currentMillis - lastUltrasonicRead >= UltrasonicInterval) {
        lastUltrasonicRead = currentMillis;
        float distance = getDistance();
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");

        bool obstacleDanger = false;
        if (distance < 20) {
            Serial.println("Obstacle detected — turning left!");
            turn_left();
            obstacleDanger = true;
        } else {
            forward();
            obstacleDanger = false;
        }

        // RGB indicates any danger; buzzer only for obstacle OR (temp AND smoke)
        if (tempDanger || smokeDanger || obstacleDanger) {
            setRGB(1, 0, 0); // red
        } else {
            setRGB(0, 1, 0); // green
        }

        // Beep the buzzer (non-blocking) when obstacle OR (temp AND smoke)
        bool shouldBeep = obstacleDanger || tempDanger || smokeDanger;
        if (shouldBeep) {
            // toggle buzzer state based on configured durations
            if (currentMillis - lastBuzzerToggle >= (buzzerState ? BUZZER_ON_DURATION : BUZZER_OFF_DURATION)) {
                lastBuzzerToggle = currentMillis;
                buzzerState = !buzzerState;
                digitalWrite(BUZZER, buzzerState ? HIGH : LOW);
            }
        } else {
            // ensure buzzer is off and reset timing
            buzzerState = false;
            digitalWrite(BUZZER, LOW);
            lastBuzzerToggle = currentMillis;
        }
    }
}