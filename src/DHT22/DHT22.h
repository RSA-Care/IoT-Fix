#ifndef DHT22_H
#define DHT22_H

#include <Arduino.h>
#include "../OLED/OLED.h"

// Define DHT pin and sensor type
#define DHTPIN 18
#define DHTTYPE DHT22

// Structure to hold DHT sensor readings
typedef struct
{
  float temperatureC;
  float humidity;
  float heatIndex;
} DhtReading;

// Function prototypes for interacting with DHT sensor
void dhtBegin();
DhtReading dhtRead();

#endif