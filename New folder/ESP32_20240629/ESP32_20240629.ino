#define BLYNK_TEMPLATE_ID "TMPL6swIxX_Aq"
#define BLYNK_TEMPLATE_NAME "ESP32 Capstone Project"
#define BLYNK_AUTH_TOKEN "D8hTCLlstXEbaofvdyV7iItHpIEj5DK1"

#define BLYNK_PRINT Serial

#include <DFRobot_MAX30102.h>
#include <BlynkSimpleEsp32.h>
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

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 4000;

const int buzzerPin = 2;
const int greenLedPin = 15;
const int redLedPin = 4;
const int BTN_RESETWIFI = 4;

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(buzzerPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  digitalWrite(buzzerPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);

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
  } else {
    Serial.println("No finger?");
    heartRate = 0;
    SPO2 = 0;
    lastSendTime = millis();
  }

  delay(100);
}