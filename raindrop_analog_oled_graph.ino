/*
raindrop module analog output for graph

1. Rain Sensor Module	Arduino	Notes
VCC	5V	Power supply
GND	GND	Ground
DO (digital out)	D2 (example)	HIGH when dry, LOW when wet
AO (analog out)	A0 (optional)	Returns analog moisture level
2. OLED wire connecction
Wiring: I2C OLED
OLED Pin	Arduino Uno / Mega
VCC	5V
GND	GND
SDA	A4 (Uno) / 20 (Mega)
SCL	A5 (Uno) / 21 (Mega)
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RAIN_SENSOR_AO_PIN A0
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Graph data buffer
uint8_t graphBuffer[SCREEN_WIDTH] = {0};
uint8_t graphIndex = 0;

// Max analog range to scale (0–700)
const int MAX_ANALOG = 700;

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED not found"));
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Rain Graph Init");
  display.display();
  delay(1000);
}

void loop() {
  int rainValue = analogRead(RAIN_SENSOR_AO_PIN); // 0 (wet) to 1023 (dry)
  int clampedValue = constrain(rainValue, 0, MAX_ANALOG);

  // Map to OLED height (0 = bottom, MAX_ANALOG = top)
  uint8_t y = map(clampedValue, 0, MAX_ANALOG, 0, SCREEN_HEIGHT - 1);
  graphBuffer[graphIndex] = y;

  display.clearDisplay();

  // Draw Y-axis labels (optional)
  display.setCursor(0, 0);
  display.print(MAX_ANALOG); // Top
  display.setCursor(0, SCREEN_HEIGHT / 2 - 3);
  display.print(MAX_ANALOG / 2); // Middle
  display.setCursor(0, SCREEN_HEIGHT - 8);
  display.print("0"); // Bottom

  // Y-axis line
  display.drawLine(25, 0, 25, SCREEN_HEIGHT, SSD1306_WHITE);

  // X-axis line
  display.drawLine(25, SCREEN_HEIGHT - 1, SCREEN_WIDTH, SCREEN_HEIGHT - 1, SSD1306_WHITE);

  // Draw graph line
  for (uint8_t i = 1; i < SCREEN_WIDTH - 25; i++) {
    int x1 = i + 25 - 1;
    int y1 = SCREEN_HEIGHT - 1 - graphBuffer[(graphIndex + i - 1) % (SCREEN_WIDTH - 25)];
    int x2 = i + 25;
    int y2 = SCREEN_HEIGHT - 1 - graphBuffer[(graphIndex + i) % (SCREEN_WIDTH - 25)];
    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }

  // Show current value
  display.setCursor(70, 0);
  display.print("Rain:");
  display.print(rainValue);

  display.display();

  graphIndex = (graphIndex + 1) % (SCREEN_WIDTH - 25);
  delay(100);
}
