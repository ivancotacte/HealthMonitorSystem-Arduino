#include <HX711_ADC.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

#define ERROR_PIN D4
#define SUCCESS_PIN D3
#define CONN_PIN D0

const int HX711_dout = D2;
const int HX711_sck = D1;

HX711_ADC LoadCell(HX711_dout, HX711_sck);

const char *ssid = "TP-Link_3838";
const char *password = "nickaikian2143"; 

const int calVal_calVal_eepromAdress = 0;
unsigned long t = 0;

void setup() {
  Serial.begin(57600);

   WiFi.begin(ssid, password);
   Serial.println("");

   pinMode(CONN_PIN, OUTPUT);
   pinMode(SUCCESS_PIN, OUTPUT);
   pinMode(ERROR_PIN, OUTPUT);

   Serial.print("Connecting");
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }

   Serial.println("");
   Serial.print("Connected to ");
   Serial.println(ssid);
   Serial.print("IP address: ");
   Serial.println(WiFi.localIP()); 

  delay(10);
  Serial.println();
  Serial.println("Starting...");

  float calibrationValue;
  calibrationValue = 26.81;
#if defined(ESP8266) || defined(ESP32)
  // EEPROM.begin(512);
#endif
  // EEPROM.get(calVal_eepromAdress, calibrationValue);

  LoadCell.begin();
  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU > HX711 wiring and pin designations");
  }
  else {
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
  Serial.print("HX711 measured settling time ms: ");
  Serial.println(LoadCell.getSettlingTime());
  Serial.println("Note that the settling time may increase significantly if you use delay() in your sketch!");
  if (LoadCell.getSPS() < 7) {
    Serial.println("!! Sampling rate is lower than specification, check MCU > HX711 wiring and pin designations");
  }
  else if (LoadCell.getSPS() > 100) {
    Serial.println("!! Sampling rate is higher than specification, check MCU > HX711 wiring and pin designations");
  }
}

void sendDataToNodeJSAPI(int weightNum) {

}

void toggleConnStat() {
  if(WiFi.status() == WL_CONNECTED) {
    digitalWrite(CONN_PIN, HIGH);
  } else {
    digitalWrite(CONN_PIN, LOW);
  }
}

void loop() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 100;

  if (LoadCell.update()) newDataReady = true;

  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      Serial.print("Load cell output value: ");
      Serial.println(i);
      sendDataToNodeJSAPI(i);
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


  toggleConnStat();

  digitalWrite(SUCCESS_PIN, LOW);
  digitalWrite(ERROR_PIN, LOW);

  
}