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
  println("GPS TRACKER");
  println("FOR DOWN SYNDROME");
  println("=================");
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
  if (display.getCursorX() + message.length() > display.width())
  {
    display.println(message);
  }
  else
  {
    display.print(message);
  }
  display.display();
}

void println(String message)
{
  Serial.println(message);

  if (display.getCursorY() == display.height())
  {
    clearScreen();
    display.println(message);
  }
  else
  {
    display.println(message);
  }

  display.display();
}

// Execute only once
void header(String signal, String topic)
{
  clearScreen();
  String text = "DST Tracker";
  int width = display.width();
  int box_height = 14;
  display.drawRect(0, 0, width, box_height, SSD1306_WHITE);
  display.setCursor(3, 3);
  display.print(text);
  display.setCursor(display.width() - 3 - (topic.length() * 6), 3);
  display.println(topic);

  // gps data
  display.setCursor(0, box_height + 2);
  display.print("Latitude");
  display.setCursor(SCREEN_WIDTH / 2, box_height + 2);
  display.print("Longitude");

  // dht data
  display.setCursor(0, 40);
  display.println("Temp");
  display.println("Humidity");

  display.display();
}

// After initialization only
void gpsDisplay(String latitude, String longitude)
{
  int cursorHeight = 24;

  display.fillRect(0, cursorHeight - 8, SCREEN_WIDTH, cursorHeight, BLACK);

  display.setCursor(0, cursorHeight);
  display.print(latitude);
  display.setCursor(SCREEN_WIDTH / 2, cursorHeight);
  display.print(longitude);
  display.display();
}

void dhtDisplay(String temperature, String humidity)
{
  int cursorWidth = SCREEN_WIDTH / 2;
  int cursorHeight = 40;

  display.fillRect(cursorWidth, cursorHeight - 8, SCREEN_WIDTH / 2, SCREEN_HEIGHT, BLACK);

  display.setCursor(cursorWidth, cursorHeight);
  display.print(temperature + " C");
  display.setCursor(cursorWidth, cursorHeight + 8);
  display.print(humidity + " %");

  display.display();
}