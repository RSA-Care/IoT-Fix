#include <Arduino.h>
#include "OLED/OLED.h"
#include "DHT22/DHT22.h"
#include "A9G/A9G.h"
#include "WiFi/WiFiconn.h"
#include "MQTT/MQTT.h"

// #define SSID "Abbey"
// #define PASS "abbey123"

#define SSID "Ahda's"
#define PASS "@hotspot.Personal"

void setup()
{
  Serial.begin(115200);

  oledBegin();
  dhtBegin();
  A9GBegin();

  if (GPRScheckConnection())
  {
    GPRSMQTTConnect();
  }
  else
  {
    WiFibegin(SSID, PASS); // need to add check to switch between gprs connection and wifi connection
    MQTTbegin();
  }

  clearScreen();
  println("=== Startup Finished");
  println(statusCheck());
  delay(2000);
}

void loop()
{
  gpsReading gps = getGPS();  // getting gps data
  DhtReading dht = dhtRead(); // getting dht data

  if (WiFi.status() == WL_CONNECTED) // check if WiFi is connected
  {
    if (MQTTConnection()) // checking mqtt connection
    {
      String payload = gps.longitude + "," + gps.latitude + "," + String(dht.temperatureC) + "," + String(dht.humidity);

      publish(payload.c_str());
    }
    else
    {
      MQTTReconnect();
    }
  }
  else // if not connected to WiFi use GPRS
  {
    if (GPRScheckConnection()) // check gprs connection
    {
      if (GPRSMQTTConnectionCheck()) // check mqtt connection
      {
        String payload = gps.longitude + "," + gps.latitude + "," + String(dht.temperatureC) + "," + String(dht.humidity);
        GPRSMQTTPublish(payload);
      }
      else
      {
        GPRSMQTTReconnect();
      }
    }
  }

  delay(3000);

  clearScreen();
  println("Latitude: " + gps.latitude);
  println("Longitude: " + gps.longitude);
  println("Temperature: " + String(dht.temperatureC));
  println("Humidity: " + String(dht.humidity));
  delay(7000);
}