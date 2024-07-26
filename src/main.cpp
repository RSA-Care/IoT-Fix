#include <Arduino.h>
#include "OLED/OLED.h"
#include "DHT22/DHT22.h"
#include "A9G/A9G.h"
#include "MQTT/MQTT.h"

void setup()
{
  Serial.begin(115200);

  oledBegin();
  SPIFFSBegin();
  A9GBegin();
  dhtBegin();

  GPSbegin(); // Starting GPS

  delay(2000);
  A9G_Data gprs = deviceInfo(); // A9G Device info
  header(String(gprs.signalStrength), getData("topic.txt"));
}

void loop()
{
  internetCheck();
  gpsReading gps = getGPS();  // getting gps data
  DhtReading dht = dhtRead(); // getting dht data

  // clearScreen();
  gpsDisplay(String(gps.latitude), String(gps.longitude));
  dhtDisplay(String(dht.temperatureC), String(dht.humidity));

  String payload = gps.longitude + "," + gps.latitude + "," + String(dht.temperatureC) + "," + String(dht.humidity);

  GPRSMQTTPublish(payload);

  delay(10000);
}