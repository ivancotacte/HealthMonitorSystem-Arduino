1#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char *ssid = "TP-Link_3838";
const char *password = "nickaikian2143"; 

void setup() {
  Serial.begin(9600);

   WiFi.begin(ssid, password);
   Serial.println("");
   
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

void sendRfidLog(String heartRate) {
  
  if(WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    String postData = "heartRate=" + String(heartRate) + "&action=insertHeartRate";
    http.begin(client, "http://192.168.0.108/New%20folder/api.php");              
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
        Serial.println("Received: " + message);
        sendRfidLog(message);
    }
}