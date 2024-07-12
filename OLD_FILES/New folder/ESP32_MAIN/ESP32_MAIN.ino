#define BLYNK_TEMPLATE_ID "TMPL6swIxX_Aq"
#define BLYNK_TEMPLATE_NAME "ESP32 Capstone Project"
#define BLYNK_AUTH_TOKEN "D8hTCLlstXEbaofvdyV7iItHpIEj5DK1"

#define BLYNK_PRINT Serial

#include <DFRobot_MAX30102.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiManager.h>

int32_t SPO2;
int8_t SPO2Valid;
int32_t heartRate;
int8_t heartRateValid;

char ssid[32];
char password[32];

const int BTN_RESETWIFI = 4;
const int BUZZER = 13;

WiFiManager wifiManager;
DFRobot_MAX30102 particleSensor;

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

void loop() {
  long irValue = particleSensor.getIR();

  if(irValue > 50000) {
    particleSensor.heartrateAndOxygenSaturation(&SPO2, &SPO2Valid, &heartRate, &heartRateValid);
    Serial.print(F("heartRate="));
    Blynk.virtualWrite(V2, heartRate);
    Serial.print(heartRate, DEC);
    Serial.print(F(", heartRateValid="));
    Serial.print(heartRateValid, DEC);
    Serial.print(F("; SPO2="));
    Blynk.virtualWrite(V3, SPO2);
    Serial.print(SPO2, DEC);
    Serial.print(F(", SPO2Valid="));
    Serial.println(SPO2Valid, DEC);
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
  } else if (irValue < 50000) {
    Serial.println("No finger?");
  }

}
