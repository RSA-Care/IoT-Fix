#include <Arduino.h>
#include "OLED/OLED.h"
#include "DHT22/DHT22.h"
#include "A9G/A9G.h"
#include "WiFi/WiFiconn.h"
#include "MQTT/MQTT.h"

#define SSID "Abbey"
#define PASS "abbey123"

// #define SSID "Apaweh"
// #define PASS "langsungmasuk"

void setup()
{
  Serial.begin(115200);

  oledBegin();
  SPIFFSBegin();
  A9GBegin();
  dhtBegin();

  // WiFibegin(SSID, PASS);

  GPSbegin(); // Starting GPS

  clearScreen();
  println("=== Startup Finished");
  println(statusCheck());
  delay(2000);
}

void loop()
{
  A9G_Data gprs = deviceInfo(); // A9G Device info
  gpsReading gps = getGPS();    // getting gps data
  DhtReading dht = dhtRead();   // getting dht data

  // clearScreen();
  header(String(gprs.signalStrength), getData("topic.txt"));
  println("Latitude: " + gps.latitude);
  println("Longitude: " + gps.longitude);
  println("Temperature: " + String(dht.temperatureC));
  println("Humidity: " + String(dht.humidity));

  String payload = gps.longitude + "," + gps.latitude + "," + String(dht.temperatureC) + "," + String(dht.humidity);

  GPRSMQTTPublish(payload);

  delay(10000);
}