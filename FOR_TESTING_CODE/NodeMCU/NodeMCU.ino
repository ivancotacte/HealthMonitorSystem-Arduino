#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define ERROR_PIN D2
#define SUCCESS_PIN D1
#define CONN_PIN D0

const char *ssid = "TP-Link_3838"; //WIFI NAME OR HOTSPOT
const char *password = "nickaikian2143"; //WIFI PASSWORD POR MOBILE HOTSPOT PASSWORD

void setup() {
  Serial.begin(9600);

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
}

void toggleConnStat() {
  if(WiFi.status() == WL_CONNECTED) {
    digitalWrite(CONN_PIN, HIGH);
  } else {
    digitalWrite(CONN_PIN, LOW);
  }
}

void loop() {
    if (Serial.available()) {
        String message = Serial.readStringUntil('\n');
        Serial.println("Received: " + message);
    }


  toggleConnStat();
  delay (2000);
  
  digitalWrite(SUCCESS_PIN, LOW);
  digitalWrite(ERROR_PIN, LOW);
}