#include <Arduino.h>
#include "DHT22/DHT22.h"
#include "A9G/A9G.h"

void setup()
{
  Serial.begin(115200);

  dhtBegin();
  A9GBegin();
}

void loop()
{
  gpsReading gps = getGPS();
  DhtReading data = dhtRead();
}