#if !defined(A9G_h)
#define A9G_h

#include <Arduino.h>

// Your GPRS credentials, if any
const char apn[] = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";

typedef struct
{
  float latitude;
  float longitude;
} gpsReading;

// Function prototypes
void A9GBegin();
void sendAT(String command = "AT");
void getInfo();
gpsReading getGPS();

#endif // A9G_h