#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "TP-Link_3838";   
const char *pass = "nickaikian2143";

WiFiClient client;

void setup() {
  Serial.begin(9600);
  delay(10);
  
  Serial.println("Connecting to WiFi network:");
  Serial.println(ssid); 

  WiFi.begin(ssid, pass); 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected"); 
}

void HeartRate(String HeartRate) {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String postData = "heartRate=" + String(HeartRate);
    http.begin(client, "http://192.168.0.108:3000/register");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    int httpCode = http.POST(postData);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    
    http.end();
  }
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    Serial.println(message);
    HeartRate(message);
  }
}