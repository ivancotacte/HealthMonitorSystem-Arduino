#define BLYNK_TEMPLATE_ID "TMPL6swIxX_Aq"
#define BLYNK_TEMPLATE_NAME "ESP32 Capstone Project"
#define BLYNK_AUTH_TOKEN "D8hTCLlstXEbaofvdyV7iItHpIEj5DK1"

#define BLYNK_PRINT Serial

#include <DFRobot_MAX30102.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>

int32_t SPO2;
int8_t SPO2Valid;
int32_t heartRate;
int8_t heartRateValid;

char ssid[32];
char password[32];

String serverName = "http://192.168.0.110/healthMonitorSystemAPI-PHP/api.php";

const char* serverName1 = "http://192.168.0.110/";
String testServerPath = "/healthMonitorSystemAPI-PHP/api.php";

WiFiManager wifiManager;
DFRobot_MAX30102 particleSensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000;

const int buzzerPin = 2;
const int greenLedPin = 15;
const int redLedPin = 4;
const int orangeLedPin = 5; // Define the pin for the orange LED

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(buzzerPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(orangeLedPin, OUTPUT); // Set the orange LED pin as output

  digitalWrite(buzzerPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(orangeLedPin, LOW); // Initially turn off the orange LED

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
    delay(1000);
  }

  particleSensor.sensorConfiguration(60, SAMPLEAVG_8, MODE_MULTILED, SAMPLERATE_400, PULSEWIDTH_411, ADCRANGE_16384);

  lcd.init();
  lcd.backlight();
}

void sendAPI(String heartRate, String SPO2) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    String httpRequestData = "heartRate=" + heartRate + "&SpO2=" + SPO2 + "&action=insertHeartRateSP01";
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    int httpResponseCode = http.POST(httpRequestData);
    
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(greenLedPin, HIGH);
      delay(1000);
      digitalWrite(buzzerPin, LOW);
      digitalWrite(greenLedPin, LOW);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      digitalWrite(redLedPin, HIGH);
      delay(1000);
      digitalWrite(redLedPin, LOW);
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() {
  particleSensor.heartrateAndOxygenSaturation(&SPO2, &SPO2Valid, &heartRate, &heartRateValid);

  long irValue = particleSensor.getIR();

  if (irValue > 5000) {
    if (millis() - lastSendTime >= sendInterval) {
      lastSendTime = millis();
      sendAPI(String(heartRate), String(SPO2));
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HR: " + String(heartRate));
    lcd.setCursor(0, 1);
    lcd.print("SpO2: " + String(SPO2));
    digitalWrite(orangeLedPin, LOW); // Turn off the orange LED
  } else {
    Serial.println("No finger?");
    heartRate = 0;
    SPO2 = 0;
    lastSendTime = millis();

    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Please place");
    lcd.setCursor(2, 1);
    lcd.print("your finger");
    digitalWrite(orangeLedPin, HIGH); // Turn on the orange LED
  }

  delay(100);
}