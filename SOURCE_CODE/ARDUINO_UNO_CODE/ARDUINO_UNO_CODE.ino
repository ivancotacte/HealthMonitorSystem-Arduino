#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1

#define LED_READING 13
#define LED_READY 11
#define BUZZER 12

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static const unsigned char PROGMEM logo2_bmp[] =
{ 
  0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E,  0x02, 0x10, 0x0C, 0x03, 0x10,
  0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40,  0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
  0x02, 0x08, 0xB8, 0x04,  0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
  0x00,  0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00
};

static const unsigned char PROGMEM logo3_bmp[] =
{
  0x01, 0xF0, 0x0F,  0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
  0x20,  0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08,  0x03,
  0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01,  0x80, 0x00, 0x14, 0x00,
  0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40,  0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
  0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31,  0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
  0x00, 0x60, 0xE0,  0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
  0x01,  0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C,  0x00,
  0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00,  0x00, 0x01, 0x80, 0x00
};

// Define a variable to store the start time
unsigned long startTime = 0; // Initialize to 0
unsigned long lastPrintTime = 0; // Initialize to 0
const unsigned long printInterval = 20000; // Interval in milliseconds (20 seconds)

void setup() {  
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC,  0x3C);
  display.display();
  delay(3000);
  particleSensor.begin(Wire, I2C_SPEED_FAST);
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);

  pinMode(LED_READING, OUTPUT);
  pinMode(LED_READY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  long irValue = particleSensor.getIR();

  if (irValue > 5000) {
    if (startTime == 0) { // Start the timer if not already started
      startTime = millis();
    }

    digitalWrite(LED_READY, LOW);
    display.clearDisplay();
    display.drawBitmap(50, 5, logo2_bmp, 24, 21, WHITE);
    display.display();

    if (checkForBeat(irValue) == true) {
      display.clearDisplay();
      display.drawBitmap(50, 0, logo3_bmp, 32, 32, WHITE);

      // Calculate elapsed time
      unsigned long elapsedTime = millis() - startTime;

      // Print heartbeat average every 20 seconds
      if (elapsedTime >= lastPrintTime + printInterval) {
        Serial.println(beatAvg);

        lastPrintTime = elapsedTime;
        digitalWrite(BUZZER, HIGH);
      }

      display.display();
      digitalWrite(LED_READING, HIGH);
      delay(100);
      digitalWrite(LED_READING, LOW);
      long delta = millis() - lastBeat;
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
        rates[rateSpot++] = (byte)beatsPerMinute;
        rateSpot %= RATE_SIZE;

        beatAvg = 0;
        for (byte x = 0; x < RATE_SIZE; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }

  } else if (irValue < 5000) {
    startTime = 0; // Reset the timer when finger is removed
    beatAvg = 0;
    lastPrintTime = 0; // Reset last print time

    digitalWrite(BUZZER, LOW);
    digitalWrite(LED_READY, HIGH);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(30, 0);
    display.println("Please Place");
    display.setCursor(30, 10);
    display.println("your finger");
    display.setCursor(0, 25);
    display.println("GROUP10 - LFSA322N002");
    display.display();
  }
}
