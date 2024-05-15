#include "OLED.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void oledBegin()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    return;
  }

  display.display();
  delay(1000);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("GPS TRACKER");
  display.println("FOR DOWN SYNDROME");
  display.println("=================");
  display.display();
  delay(2000);
  return;
}

void clearScreen()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.display();
}

void print(String message)
{
  Serial.print(message);
  display.print(message);
  display.display();
}

void println(String message)
{
  Serial.println(message);
  display.println(message);
  display.display();
}