#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>

char ssid[32];
char password[32];

WiFiManager wifiManager;

const char* httpsServer = "healthmonitorsystem-arduino-api.onrender.com";
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

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // For simplicity, accepts any certificate, not recommended for production

    if (client.connect(httpsServer, httpsPort)) {
      String bpm_rate = bpmRate; // Example BPM rate, replace with your actual value
      
      // Prepare POST data
      String postData = "heart_rate=" + bpm_rate;
      
      // Make POST request
      client.println("POST /hms HTTP/1.1");
      client.println("Host: " + String(httpsServer));
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Content-Length: " + String(postData.length()));
      client.println(); // End of headers
      client.println(postData); // Send POST dat

      delay(1000); // Wait for response
      
      client.stop(); // Close connection
    } else {
      Serial.println("Connection failed");
    }

  }

}

void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    Serial.println(message);
    bpmRateAPI(message);
  }

  ArduinoOTA.handle();
}
