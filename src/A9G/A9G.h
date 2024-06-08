#if !defined(A9G_h)
#define A9G_h

#include <Arduino.h>
#include "../OLED/OLED.h"

// Your GPRS credentials, if any
const char apn[] = "internet";
const char gprsUser[] = "wap";
const char gprsPass[] = "123wap";

typedef struct
{
  String latitude;
  String longitude;
} gpsReading;

// Function prototypes
void A9GBegin();
String sendAT(String command = "AT");
void getInfo();
void A9GReconnect();
bool GPSbegin();
bool GPRScheckConnection();
gpsReading getGPS();

// MQTT Functions
bool GPRSMQTTConnect();
void GPRSMQTTPublish(String payload);
void GPRSMQTTReconnect();
bool GPRSMQTTConnectionCheck();

#endif // A9G_h