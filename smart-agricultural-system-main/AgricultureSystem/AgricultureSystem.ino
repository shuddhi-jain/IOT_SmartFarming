#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "DHT.h" // version 1.3.0 by Adafruit
#include <Servo.h>

SoftwareSerial mySerial(9, 10); // RX, TX

// Defining Light sensor pin
#define LIGHTBULB_PIN 30

// Defining Moisture sensor pin
#define MoistureSensorPin A0

#define PH_PIN A3

// Defining Temperature & Humidity sensor pin
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define WATER_PUMP 38
#define WATER_SPRAY 40

#define EXHAUST_FAN_FORWARD 34
#define EXHAUST_FAN_REVERSE 35

// LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(26, 27, 22, 23, 24, 25);

// Creating servo object to control a servo motor
Servo shadeMotor;

// Initially shade is closed
int pos = 0; // Variable to store the servo position
bool isShadeOpen = false;

// For sending message using GSM
bool isPhLowMessageSent = false;
bool isPhHighMessageSent = false;
bool isOverSaturatedMessageSent = false;

void setup() {
  lcd.begin(16, 4);
  mySerial.begin(9600); // Baud rate for GSM module
  Serial1.begin(9600); // Baud rate for Serial monitor
  dht.begin();
  shadeMotor.attach(4); // Servo for shade on pin 4
  shadeMotor.write(0);
  
  pinMode(LIGHTBULB_PIN, OUTPUT); // Setting output pin for light bulbs
  pinMode(A2, INPUT);
  pinMode(PH_PIN, INPUT);

  // Initializing pinMode for Exhaust Fan
  pinMode(EXHAUST_FAN_FORWARD, OUTPUT);
  pinMode(EXHAUST_FAN_REVERSE, OUTPUT);

  // Initializing pinMode for Water pump & sprayer
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(WATER_SPRAY, OUTPUT);
  turn_off_waterPump();
  turn_off_waterSpray();

  mySerial.begin(9600); // Setting the baud rate of GSM Module  
  Serial.begin(9600);   // Setting the baud rate of Serial Monitor (Arduino)
}

void loop() {
  int moistureSensorValue = analogRead(MoistureSensorPin);
  int moistureValuePercent = map(moistureSensorValue, 0, 1023, 0, 100);
  float humidityValue = dht.readHumidity();
  float temperatureValue = dht.readTemperature();
  

 

  float phValue = analogRead(PH_PIN);
  phValue = (phValue / 1023) * 14;

    // Check if readings are valid
  if (isnan(humidityValue) || isnan(temperatureValue)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error");
    delay(1000); // Wait for a second before retrying
    return;
  }


   //Debugging: Manually set values for testing
   //moistureValuePercent = 120;
   //temperatureValue = 22;
   //humidityValue = 120;
   //phValue = 3;
  
  bool adjusting_temperature = check_temperature(temperatureValue);
  check_humidity(humidityValue, adjusting_temperature);
     check_moisture(moistureValuePercent);
  check_ph(phValue);


  delay(100); // Shorter delay for smoother simulation
}

// Adding delay
void add_delay() {
  delay(800);
}

// For GSM message
void sms_ph_high() {
  if (!isPhHighMessageSent) {
    isPhHighMessageSent = true;
    mySerial.println("Warning: PH of soil too high");
    delay(100);
    mySerial.println((char)26); // ASCII code of CTRL+Z
  }
}

void sms_ph_low() {
  if (!isPhLowMessageSent) {
    isPhLowMessageSent = true;
    mySerial.println("Warning: PH of soil too low");
    delay(100);
    mySerial.println((char)26); // ASCII code of CTRL+Z
  }
}

void sms_oversaturation() {
  if (!isOverSaturatedMessageSent) {
    isOverSaturatedMessageSent = true;
    mySerial.println("Warning: Over-saturation - Please improve drainage system");
    delay(100);
    mySerial.println((char)26); // ASCII code of CTRL+Z
  }
}

// Exhaust Fan Start
void exhaust_fan_air_in() {
  digitalWrite(EXHAUST_FAN_FORWARD, HIGH);
  digitalWrite(EXHAUST_FAN_REVERSE, LOW);
}

void exhaust_fan_air_out() {
  digitalWrite(EXHAUST_FAN_FORWARD, LOW);
  digitalWrite(EXHAUST_FAN_REVERSE, HIGH);
}

// Exhaust fan stop
void exhaust_fan_stop() {
  digitalWrite(EXHAUST_FAN_FORWARD, LOW);
  digitalWrite(EXHAUST_FAN_REVERSE, LOW);
}

// Turn on light bulb
void light_bulb_on() {
  digitalWrite(LIGHTBULB_PIN, HIGH);
}

// Turn off light bulb
void light_bulb_off() {
  digitalWrite(LIGHTBULB_PIN, LOW);
}

// Turn off water pump
void turn_off_waterPump() {
  digitalWrite(WATER_PUMP, LOW);
}

// Turn on water pump
void turn_on_waterPump() {
  digitalWrite(WATER_PUMP, HIGH);
}

// Turn off water spray
void turn_off_waterSpray() {
  digitalWrite(WATER_SPRAY, LOW);
}

// Turn on water spray
void turn_on_waterSpray() {
  digitalWrite(WATER_SPRAY, HIGH);
}

// Close roof shade, pos has to be 0
void close_shade() {
  for (; pos >= 0; pos -= 2) {
    shadeMotor.write(pos);
    delay(15);
  }
}

// Open roof shade, pos has to be 180
void open_shade() {
  for (; pos <= 180; pos += 2) {
    shadeMotor.write(pos);
    delay(15);
  }
}

void print_moistureValue(int value) {
  lcd.clear();
  lcd.print("Moisture is: ");
  lcd.print(value);
  lcd.print("%");
}

void check_moisture(int value) {
  if (value < 20) {
    isOverSaturatedMessageSent = false;
    print_moistureValue(value);
    lcd.setCursor(0, 1);
    lcd.print("Need irrigation!");
    lcd.setCursor(0, 2);
    lcd.print("Turning on");
    lcd.setCursor(0, 3);
    lcd.print("Water pump");
    turn_on_waterPump();
  } else if (value >= 20 && value <= 60) {
    isOverSaturatedMessageSent = false;
    print_moistureValue(value);
    lcd.setCursor(0, 1);
    lcd.print("Turning off");
    lcd.setCursor(0, 2);
    lcd.print("Water pump");
    turn_off_waterPump();
  } else {
    // Notify user about Over saturation using GSM
    print_moistureValue(value);
    sms_oversaturation();
    turn_off_waterPump();
  }
  add_delay();
}

void check_ph(float phValue) {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(String("PH = ") + String(phValue));
  lcd.setCursor(0, 2);
  if (phValue > 6.5) {
    isPhLowMessageSent = false;
    sms_ph_high();
  } else if (phValue < 5.5) {
    isPhHighMessageSent = false;
    sms_ph_low();
  } else {
    // pH is normal
    isPhHighMessageSent = false;
    isPhLowMessageSent = false;
  }
  add_delay();
}

void check_humidity(float humidityValue, bool adjusting_temperature) {
  if (adjusting_temperature) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Adjust Temp");
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(String("Hum = ") + String(humidityValue));
  if (humidityValue < 50) {
    turn_on_waterSpray();
    exhaust_fan_stop();
  } else if (humidityValue > 90) {
    exhaust_fan_air_out();
    turn_off_waterSpray();
  } else {
    exhaust_fan_stop();
    turn_off_waterSpray();
  }
  add_delay();
}

bool check_temperature(float temperatureValue) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(String("Temp = ") + String(temperatureValue));
  if (temperatureValue < 20) {
    if (!isShadeOpen) {
      Serial1.println("Opening...");
      open_shade();
      isShadeOpen = true;
    }
    turn_off_waterSpray();
    light_bulb_on();
    exhaust_fan_stop();
    return true;
  } else if (temperatureValue > 25) {
    turn_on_waterSpray();
    if (isShadeOpen) {
      Serial1.println("Closing...");
      close_shade();
      isShadeOpen = false;
    }
    exhaust_fan_air_in();
    light_bulb_off();
    return true;
  } else {
    if (!isShadeOpen) {
      Serial1.println("Opening...");
      open_shade();
      isShadeOpen = true;
    }
    turn_off_waterSpray();
    exhaust_fan_stop();
    light_bulb_off();
    return false;
  }


    add_delay();
}

