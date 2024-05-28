#if !defined(A9G_h)
#define A9G_h

#include <Arduino.h>
#include "../OLED/OLED.h"

#define broker_url "35.209.3.73"
#define user "admin"
#define pass "hivemq"
#define port 1883

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
void sendAT(String command = "AT");
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