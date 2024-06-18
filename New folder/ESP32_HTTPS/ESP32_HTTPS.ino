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

const int BTN_RESETWIFI = 4;
const int BUZZER = 13;

WiFiManager wifiManager;
DFRobot_MAX30102 particleSensor;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);

  pinMode(BTN_RESETWIFI, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  wifiManager.autoConnect("AutoConnectAP");

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

void SendValuetoAPI(String heartRate, String SPO2) {

    WiFiClient client;
    HTTPClient http;
      
    String httpRequestData = "heartRate=" + heartRate + "&SPO2=" + SPO2;
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
    int httpResponseCode = http.POST(httpRequestData);
      
    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
    } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
      
    http.end();
}


void loop() {
  particleSensor.heartrateAndOxygenSaturation(&SPO2, &SPO2Valid, &heartRate, &heartRateValid);

  long irValue = particleSensor.getIR();

  if(irValue > 50000) {

    SendValuetoAPI(String(heartRate), String(SPO2));

  } else if (irValue < 50000) {
    Serial.println("No finger?");
  }

}