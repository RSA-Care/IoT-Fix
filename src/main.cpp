#include <Arduino.h>
#include "OLED/OLED.h"
#include "DHT22/DHT22.h"
#include "A9G/A9G.h"
#include "WiFi/WiFiconn.h"
#include "MQTT/MQTT.h"

#define SSID "bahagia permai"
#define PASS "imut2023"

void setup()
{
  Serial.begin(115200);

  oledBegin();
  dhtBegin();
  A9GBegin();
  WiFibegin(SSID, PASS); // need to add check to switch between gprs connection and wifi connection
  MQTTbegin();

  clearScreen();
  println("=== Startup Finished");
  println(statusCheck());
  delay(2000);
}

void loop()
{
  gpsReading gps = getGPS();
  DhtReading dht = dhtRead();

  if (MQTTConnection())
  {
    String payload = String(gps.longitude) + "," + String(gps.latitude) + "," + String(dht.temperatureC) + "," + String(dht.humidity);

    publish(payload.c_str());
  }

  clearScreen();
  println("Latitude: " + String(gps.latitude));
  println("Longitude: " + String(gps.longitude));
  println("Temperature: " + String(dht.temperatureC));
  println("Humidity: " + String(dht.humidity));
  delay(1000);
}