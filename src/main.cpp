#include <Arduino.h>
#include "OLED/OLED.h"
#include "DHT22/DHT22.h"
#include "A9G/A9G.h"
#include "WiFi/WiFiconn.h"
#include "MQTT/MQTT.h"

#define SSID "Abbey"
#define PASS "abbey123"

// #define SSID "AHDA 4434"
// #define PASS "Ahda@hotspot"

// #define SSID "Apaweh"
// #define PASS "langsungmasuk"

void setup()
{
  Serial.begin(115200);

  oledBegin();
  A9GBegin();
  SPIFFSBegin();
  dhtBegin();
  WiFibegin(SSID, PASS);

  /*
  Check if wifi is connected, if not then use gsm
  */
  if (WiFi.status() == WL_CONNECTED)
  {
    IPAddress ip_local = WiFi.localIP();
    clearScreen();
    println(ip_local.toString());
    println("Starting MQTT.");
    delay(1000);
    MQTTbegin();
  }
  else
  {
    clearScreen();
    println("WiFi not connected.\nStarting GSM.");
    delay(1000);
    GPRScheckConnection();
  }

  GPSbegin(); // Starting GPS

  clearScreen();
  println("=== Startup Finished");
  println(statusCheck());
  delay(2000);
}

void loop()
{
  gpsReading gps = getGPS();  // getting gps data
  DhtReading dht = dhtRead(); // getting dht data

  // clearScreen();
  oledHeader();
  println("Latitude: " + gps.latitude);
  println("Longitude: " + gps.longitude);
  println("Temperature: " + String(dht.temperatureC));
  println("Humidity: " + String(dht.humidity));

  String payload = gps.longitude + "," + gps.latitude + "," + String(dht.temperatureC) + "," + String(dht.humidity);

  if (checkWiFiConnection()) // check if WiFi is connected
  {
    if (MQTTConnection()) // checking mqtt connection
    {
      publish(payload.c_str());
    }
    else
    {
      MQTTReconnect();
    }
  }
  else
  {
    GPRSMQTTPublish(payload);
  }

  delay(10000);
}