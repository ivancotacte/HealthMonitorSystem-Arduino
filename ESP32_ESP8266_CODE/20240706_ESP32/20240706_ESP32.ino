#define BLYNK_TEMPLATE_ID "TMPL6swIxX_Aq"
#define BLYNK_TEMPLATE_NAME "ESP32 Health Monitoring System"
#define BLYNK_AUTH_TOKEN "kPlQEtx23LIunXkUMBqImgQu5o5cK9VR"

#define BLYNK_PRINT Serial

#include <DFRobot_MAX30102.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>

int32_t SPO2;
int8_t SPO2Valid;
int32_t heartRate;
int8_t heartRateValid;

char ssid[32];
char password[32];

const char* serverName = "bantaykalusugan.replit.app";
String ServerPath = "/api.php";

const int port = 443;

WiFiManager wifiManager;
WiFiClientSecure client;
DFRobot_MAX30102 particleSensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 20000;

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
  
  wifiManager.autoConnect("Group 10 - AutoConnectAP");

  WiFi.begin(ssid, password);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

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
  client.setCACert(nullptr); 
  
  if (client.connect(serverName, port)) {
    Serial.println("Connection Successful");

    String postData = "heartRate=" + heartRate + "&SpO2=" + SPO2 + "&action=insertHeartRateSP01";

    client.println("POST " + ServerPath + " HTTP/1.1");
    client.println("Host: " + String(serverName));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);

    // Check for server response
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
      digitalWrite(redLedPin, HIGH);
      delay(1000);
      digitalWrite(redLedPin, LOW);
    } else {
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(greenLedPin, HIGH);
      delay(1000);
      digitalWrite(buzzerPin, LOW);
      digitalWrite(greenLedPin, LOW);
    }

    client.stop();
  } else {
    digitalWrite(redLedPin, HIGH);
    delay(1000);
    digitalWrite(redLedPin, LOW);
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
  lcd.setCursor(0, 1);
  lcd.print("SpO2: " + String(SPO2));
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
