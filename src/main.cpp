#include <Arduino.h>
#include "OLED/OLED.h"
#include "DHT22/DHT22.h"
#include "A9G/A9G.h"

void setup()
{
  Serial.begin(115200);

  oledBegin();
  dhtBegin();
  A9GBegin();

  clearScreen();
  println("=== Startup Finished ===");
  delay(1000);
}

void loop()
{
  gpsReading gps = getGPS();
  DhtReading data = dhtRead();

  clearScreen();
  println("Latitude: " + String(gps.latitude));
  println("Longitude: " + String(gps.longitude));
  println("Temperature: " + String(data.temperatureC));
  println("Humidity: " + String(data.humidity));
  delay(1000);
}