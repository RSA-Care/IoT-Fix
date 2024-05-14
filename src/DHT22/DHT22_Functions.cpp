#include "DHT22.h"
#include <DHT.h>
#include <DHT_U.h>

DHT dht(DHTPIN, DHTTYPE);

void dhtBegin()
{
  dht.begin();
}

DhtReading dhtRead()
{
  DhtReading reading;
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temp) || isnan(humidity))
  {
    reading.temperatureC = -100; // Or any value to indicate error
    reading.humidity = -1;       // Or any value to indicate error
  }
  else
  {
    reading.temperatureC = temp;
    reading.humidity = humidity;
  }
  return reading;
}