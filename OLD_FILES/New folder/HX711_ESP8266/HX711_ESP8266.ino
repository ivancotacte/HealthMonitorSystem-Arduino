#define BLYNK_TEMPLATE_ID "TMPL6swIxX_Aq"
#define BLYNK_TEMPLATE_NAME "ESP32 Capstone Project"
#define BLYNK_AUTH_TOKEN "D8hTCLlstXEbaofvdyV7iItHpIEj5DK1"

#define BLYNK_PRINT Serial

#include <HX711_ADC.h>
#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>

char ssid[32];
char password[32];

const int BTN_RESETWIFI = D3;

const int HX711_dout = D2;
const int HX711_sck = D1;

WiFiManager wifiManager;
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_calVal_eepromAdress = 0;
unsigned long t = 0;

void setup() {
  Serial.begin(115200); 

  pinMode(BTN_RESETWIFI, INPUT_PULLUP);

  if (digitalRead(BTN_RESETWIFI) == LOW) {
    wifiManager.resetSettings();
  } else {
    wifiManager.autoConnect("AutoConnectAP");
  }

  WiFi.begin(ssid, password);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  float calibrationValue = 26.81;

  LoadCell.begin();
  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
  } else {
    LoadCell.setCalFactor(calibrationValue);
    Serial.println("Startup is complete");
  }
  while (!LoadCell.update());
  Serial.print("Calibration value: ");
  Serial.println(LoadCell.getCalFactor());
  Serial.print("HX711 measured conversion time ms: ");
  Serial.println(LoadCell.getConversionTime());
  Serial.print("HX711 measured sampling rate HZ: ");
  Serial.println(LoadCell.getSPS());
  Serial.print("HX711 measured settlingtime ms: ");
  Serial.println(LoadCell.getSettlingTime());
  Serial.println("Note that the settling time may increase significantly if you use delay() in your sketch!");
  if (LoadCell.getSPS() < 7) {
    Serial.println("!!Sampling rate is lower than specification, check MCU>HX711 wiring and pin designations");
  } else if (LoadCell.getSPS() > 100) {
    Serial.println("!!Sampling rate is higher than specification, check MCU>HX711 wiring and pin designations");
  }
}

void loop() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0;

  if (LoadCell.update()) newDataReady = true;

  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float weight = LoadCell.getData() / 1000.0; 

      if (weight < 0) {
        weight = 0.0;
      }
      
      Serial.print("Load_cell output val (kg): ");
      Blynk.virtualWrite(V1, weight);
      Serial.println(weight);
      newDataReady = 0;
      t = millis();
    }
  }

  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
}