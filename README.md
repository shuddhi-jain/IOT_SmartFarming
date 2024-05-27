# Arduino Greenhouse Monitoring and Control System

This project demonstrates how to run an Arduino sketch in Proteus software, simulating a greenhouse monitoring and control system. The system uses various sensors to monitor environmental conditions and actuates devices like fans, water pumps, and shades based on the sensor readings.

## Table of Contents
- [Requirements](#requirements)
- [Setting Up Proteus](#setting-up-proteus)
- [Running the Simulation](#running-the-simulation)
- [Code Explanation](#code-explanation)

## Requirements
- Arduino IDE
- Proteus software (version 8 or later)
- Proteus library for Arduino
- Proteus library for LCD
- DHT library (version 1.3.0 by Adafruit)
- LiquidCrystal library
- SoftwareSerial library
- Servo library

## Setting Up Proteus

1. **Install Proteus and Libraries:**
   - Ensure you have Proteus software installed on your system.
   - Download and add the Arduino library to Proteus.

2. **Create a New Project:**
   - Open Proteus and create a new project.
   - Select the Arduino Uno board from the library.

3. **Add Components:**
   - Add the following components to your schematic:
     - Arduino Uno
     - LCD Display (16x4)
     - DHT22 sensor
     - Servo motor
     - Light sensor
     - Moisture sensor
     - pH sensor
     - GSM module
     - Relays for water pump and sprayer
     - Fans (exhaust forward and reverse)
   
4. **Connect Components:**
   - Wire the components according to the pin configuration in the code.

5. **Load Arduino Code:**
   - Compile the Arduino sketch to generate the HEX file.
   - In Proteus, double-click the Arduino Uno to open the properties.
   - Load the generated HEX file into the Arduino Uno.

## Running the Simulation

1. **Start Simulation:**
   - Click the play button in Proteus to start the simulation.
   - Observe the LCD display for sensor readings and device statuses.
   
2. **Interacting with the Simulation:**
   - Adjust the sensor values to simulate different environmental conditions.
   - Observe the corresponding changes in the operation of the fans, water pump, sprayer, and shade.

## Code Explanation

### Libraries and Pin Definitions
```cpp
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "DHT.h"
#include <Servo.h>

SoftwareSerial mySerial(9, 10); // RX, TX

#define LIGHTBULB_PIN 30
#define MoistureSensorPin A0
#define PH_PIN A3
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define WATER_PUMP 38
#define WATER_SPRAY 40
#define EXHAUST_FAN_FORWARD 34
#define EXHAUST_FAN_REVERSE 35

LiquidCrystal lcd(26, 27, 22, 23, 24, 25);
Servo shadeMotor;

int pos = 0;
bool isShadeOpen = false;
bool isPhLowMessageSent = false;
bool isPhHighMessageSent = false;
bool isOverSaturatedMessageSent = false;
```
- Importing necessary libraries for handling LCD, serial communication, DHT sensor, and Servo motor.
- Defining pins for various sensors and actuators.

### Setup Function
```cpp
void setup() {
  lcd.begin(16, 4);
  mySerial.begin(9600);
  Serial1.begin(9600);
  dht.begin();
  shadeMotor.attach(4);
  shadeMotor.write(0);
  
  pinMode(LIGHTBULB_PIN, OUTPUT);
  pinMode(A2, INPUT);
  pinMode(PH_PIN, INPUT);
  pinMode(EXHAUST_FAN_FORWARD, OUTPUT);
  pinMode(EXHAUST_FAN_REVERSE, OUTPUT);
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(WATER_SPRAY, OUTPUT);
  turn_off_waterPump();
  turn_off_waterSpray();
  Serial.begin(9600);
}
```
- Initializing LCD, serial communication, DHT sensor, and servo motor.
- Setting pin modes for sensors and actuators.
- Turning off water pump and spray initially.

### Main Loop
```cpp
void loop() {
  int moistureSensorValue = analogRead(MoistureSensorPin);
  int moistureValuePercent = map(moistureSensorValue, 0, 1023, 0, 100);
  float humidityValue = dht.readHumidity();
  float temperatureValue = dht.readTemperature();
  float phValue = analogRead(PH_PIN);
  phValue = (phValue / 1023) * 14;

  if (isnan(humidityValue) || isnan(temperatureValue)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error");
    delay(1000);
    return;
  }

  bool adjusting_temperature = check_temperature(temperatureValue);
  check_humidity(humidityValue, adjusting_temperature);
  check_moisture(moistureValuePercent);
  check_ph(phValue);
  delay(100);
}
```
- Reading sensor values for moisture, humidity, temperature, and pH.
- Validating sensor readings.
- Calling functions to adjust environmental conditions based on sensor values.

### Helper Functions

#### GSM Messaging Functions
```cpp
void sms_ph_high() { ... }
void sms_ph_low() { ... }
void sms_oversaturation() { ... }
```
- Sending SMS alerts for high pH, low pH, and over-saturation using GSM module.

#### Fan Control Functions
```cpp
void exhaust_fan_air_in() { ... }
void exhaust_fan_air_out() { ... }
void exhaust_fan_stop() { ... }
```
- Controlling exhaust fan for air intake, air outflow, and stopping.

#### Light Control Functions
```cpp
void light_bulb_on() { ... }
void light_bulb_off() { ... }
```
- Turning light bulb on and off.

#### Water Control Functions
```cpp
void turn_off_waterPump() { ... }
void turn_on_waterPump() { ... }
void turn_off_waterSpray() { ... }
void turn_on_waterSpray() { ... }
```
- Controlling water pump and spray.

#### Shade Control Functions
```cpp
void close_shade() { ... }
void open_shade() { ... }
```
- Opening and closing the shade using servo motor.

#### Sensor Data Handling Functions
```cpp
void print_moistureValue(int value) { ... }
void check_moisture(int value) { ... }
void check_ph(float phValue) { ... }
void check_humidity(float humidityValue, bool adjusting_temperature) { ... }
bool check_temperature(float temperatureValue) { ... }
```
- Displaying sensor values on the LCD.
- Checking moisture, pH, humidity, and temperature to adjust actuators accordingly.

### Conclusion
This Arduino sketch simulates a comprehensive greenhouse monitoring and control system. By running the provided code in Proteus, you can observe how different environmental conditions affect the operations of various devices like fans, water pump, sprayer, and shade, ensuring optimal growth conditions within the greenhouse.
