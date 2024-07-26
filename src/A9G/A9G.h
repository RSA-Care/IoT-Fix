#if !defined(A9G_h)
#define A9G_h

#include <Arduino.h>
#include "../OLED/OLED.h"
#include "Data/DataHandler.h"

// Your GPRS credentials, if any
const char apn[] = "internet";
const char gprsUser[] = "wap";
const char gprsPass[] = "123wap";

typedef struct
{
  String latitude = "0.00";
  String longitude = "0.00";
} gpsReading;

typedef struct
{
  int signalStrength = 0;
  int errorRate = 0;
} A9G_Data;

// Function prototypes
void A9GBegin();
String sendAT(String command = "AT");
void getInfo();
void A9GReconnect();
bool GPSbegin();
bool GPRScheckConnection();
bool internetCheck();
bool saveGPSData(gpsReading gps);
A9G_Data deviceInfo();
gpsReading getGPS();

// MQTT Functions
void GPRSMQTTPublish(String payload);
void oledHeader();

#endif // A9G_h