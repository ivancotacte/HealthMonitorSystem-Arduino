#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>

char ssid[32];
char password[32];

WiFiManager wifiManager;

const char* httpsServer = "healthmonitorsystem-arduino.onrender.com";
const int httpsPort = 443; 
const int resetSwitchPin = D1;

void setup() {
  Serial.begin(9600);
  
  pinMode(resetSwitchPin, INPUT_PULLUP);

  if(digitalRead(resetSwitchPin) == LOW) {
    wifiManager.resetSettings();
    Serial.println("WiFiManager settings reset.");
  }
  else {
    wifiManager.autoConnect("AutoConnectAP");
  }

  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());

  ArduinoOTA.begin();
}

void bpmRateAPI(String bpmRate) {

}

void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    Serial.println(message);
  }

  ArduinoOTA.handle();

  delay(5000);
}
