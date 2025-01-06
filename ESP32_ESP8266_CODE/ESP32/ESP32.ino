#include <DFRobot_MAX30102.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

int32_t SPO2;
int8_t SPO2Valid;
int32_t heartRate;
int8_t heartRateValid;

const char* ssid = "realme 6i";
const char* password = "12345678";

const char* serverName = "bantaykalusuganph.online";
String ServerPath = "/request";

const int port = 443;

WiFiClientSecure client;
DFRobot_MAX30102 particleSensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 5000;

const int buzzerPin = 2;
const int greenLedPin = 15;
const int redLedPin = 4;
const int orangeLedPin = 5;

bool dataSent = false;

void setup() {
  Serial.begin(115200);
  delay(100);

  lcd.init();
  lcd.backlight();

  pinMode(buzzerPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(orangeLedPin, OUTPUT);

  digitalWrite(buzzerPin, LOW);

  displayMessage("Initializing...", "Please wait");

  digitalWrite(greenLedPin, HIGH);
  digitalWrite(redLedPin, HIGH);
  digitalWrite(orangeLedPin, HIGH);
  delay(1000);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(orangeLedPin, LOW);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  while (!particleSensor.begin()) {
    Serial.println("MAX30102 was not found");
    displayError("MAX30102", "not found");
    delay(1000);
  }

  particleSensor.sensorConfiguration(60, SAMPLEAVG_8, MODE_MULTILED, SAMPLERATE_400, PULSEWIDTH_411, ADCRANGE_16384);
}

void sendAPI(String heartRate, String SPO2) {
  updateLCD();
  client.setInsecure(); 
  
  if (client.connect(serverName, port)) {
    String postData = "heartRate=" + heartRate + "&SpO2=" + SPO2 + "&action=insertHeartRateSP01";

    client.println("POST " + ServerPath + " HTTP/1.1");
    client.println("Host: " + String(serverName));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);

    bool error = false;
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
      if (line.startsWith("HTTP/1.1 4") || line.startsWith("HTTP/1.1 5")) {
        error = true;
      }
    }

    if (error) {
      displayError("API Error", "Check Server");
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(redLedPin, HIGH);
      delay(1000);
      digitalWrite(redLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
    } else {
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(greenLedPin, HIGH);
      delay(2000);
      digitalWrite(buzzerPin, LOW);
      digitalWrite(greenLedPin, LOW);
    }

    client.stop();
  } else {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(redLedPin, LOW);
    digitalWrite(buzzerPin, LOW);
    displayError("Connection", "failed");
  }
}

void displayMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void displayError(String errorType, String errorMessage) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Error: " + errorType);
  lcd.setCursor(0, 1);
  lcd.print(errorMessage);
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HR: " + String(heartRate));
  Serial.println("Heart Rate: " + String(heartRate));
  lcd.setCursor(0, 1);
  lcd.print("SpO2: " + String(SPO2));
  Serial.println("SpO2: " + String(SPO2));
}

void loop() {
  particleSensor.heartrateAndOxygenSaturation(&SPO2, &SPO2Valid, &heartRate, &heartRateValid);

  long irValue = particleSensor.getIR();

  if (irValue > 5000) {
    if (!dataSent) {
      if (millis() - lastSendTime >= sendInterval) {
        lastSendTime = millis();
        sendAPI(String(heartRate), String(SPO2));
        dataSent = true;
      }
    }
    
    if (dataSent) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Please remove");
      lcd.setCursor(2, 1);
      lcd.print("your finger");
    } else {
      updateLCD();
    }
    
    digitalWrite(orangeLedPin, LOW);
  } else {
    if (dataSent) {
      dataSent = false;
    }

    Serial.println("No finger?");
    heartRate = 0;
    SPO2 = 0;
    lastSendTime = millis();

    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Please place");
    lcd.setCursor(2, 1);
    lcd.print("your finger");
    digitalWrite(orangeLedPin, HIGH);
  }

  delay(100);
}