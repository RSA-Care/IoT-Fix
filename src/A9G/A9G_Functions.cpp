#include "A9G.h"
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

TinyGPSPlus _gps;
HardwareSerial SerialAT(2);

void A9GBegin()
{
  SerialAT.begin(115200);
  delay(3000);
  clearScreen();

  Serial.println("A9G module started");

  sendAT();
  delay(1000);

  // sendAT("AT+RST=1");
  // delay(1000);

  sendAT("ATI");

  Serial.println("Checking SIM Card Status.");
  sendAT("AT+CPIN?");

  Serial.println("Set result error message.");
  sendAT("AT+CMEE=2");

  Serial.println("Checking Operator Selection.");
  sendAT("AT+COPS?");

  Serial.println("Checking Network Registration.");
  SerialAT.println("AT+CREG?");
  delay(500);
  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    Serial.println(response);
    response.replace("\n", "");
    if (response.equals("+CREG: 1,1") >= 0)
    {
      println("Network registered.");
    }
    else
    {
      println("Network not registered.");
    }
  }

  Serial.println("Activating GPRS.");
  sendAT("AT+CGATT=1");
  sendAT("AT+CGATT?");

  Serial.println("Configuring GPRS.");
  sendAT("AT+CGDCONT=1, \"IP\", \"internet\"");
  sendAT("AT+CGDCONT?");

  Serial.println("Activating and Checking GPRS PDP Context.");
  sendAT("AT+CGACT=1,1");
  SerialAT.println("AT+CGACT?");
  delay(500);
  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    Serial.println(response);
    response.replace("\n", "");

    if (response.equals("+CGACT: 0,0") >= 0)
    {
      Serial.println("Failed to register PDP Context.");
    }
  }

  Serial.println("Starting GPS.");
  sendAT("AT+GPS=1");

  SerialAT.println("AT+GPS?");
  delay(500);
  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    Serial.println(response);
    response.replace("\n", "");

    if (response.equals("+GPS: 1") >= 0)
    {
      println("GPS started.");
    }
    else
    {
      println("GPS not started.");
    }
  }

  Serial.println("Set GPS interval to 5 Second.");
  SerialAT.println("AT+GPSRD=5");
}

void getInfo()
{
  sendAT("ATI");
}

void sendAT(String command)
{
  clearScreen();

  Serial.println("==========");
  println(command);
  SerialAT.println(command);
  delay(500);

  while (!SerialAT.available())
  {
    Serial.println(command);
    SerialAT.println(command);
    delay(1000);
  }

  String response = SerialAT.readString();
  response.trim();
  println(response);
  Serial.println("==========");

  delay(1000);
}

gpsReading getGPS()
{
  /*
  Use this to get the latitude and longitude coordinate
  AT+LOCATION=1

  return:
  <latitude>,<longitude>

  OK
  */

  Serial.println("Checking GPS data.");
  gpsReading gps;

  delay(500);
  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    Serial.println(response);

    char data = SerialAT.read();
    if (_gps.encode(data))
    {
      if (_gps.location.isValid())
      {
        Serial.println("GPS Data.");
        gps.latitude = _gps.location.lat();
        gps.longitude = _gps.location.lng();
        Serial.println("Latitude: " + String(gps.latitude));
        Serial.println("Longitude: " + String(gps.longitude));
      }
    }
  }

  return gps;
}