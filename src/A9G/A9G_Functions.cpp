#include "A9G.h"
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

TinyGPSPlus _gps;
HardwareSerial SerialAT(2);

void A9GBegin()
{
  SerialAT.begin(115200);
  Serial.println("A9G module started");

  sendAT();

  Serial.println("Checking SIM Card Status.");
  sendAT("AT+CPIN?");

  Serial.println("Set result error message.");
  sendAT("AT+CMEE=2");

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
      Serial.println("GPS started.");
    }
    else
    {
      Serial.println("GPS not started.");
    }
  }

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
      Serial.println("Network registered.");
    }
    else
    {
      Serial.println("Network not registered.");
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

  Serial.println("Testing Network.");
  sendAT("AT+PING=1.1.1.1");
  sendAT("AT+PINGSTOP");

  // Serial.println("Set GPS interval to 5 Second.");
  // SerialAT.println("AT+GPSRD=5");
}

void getInfo()
{
  sendAT("ATI");
}

void sendAT(String command)
{
  Serial.println(command);
  SerialAT.println(command);
  delay(500);

  while (!SerialAT.available())
  {
    Serial.println(command);
    SerialAT.println(command);
    delay(1000);
  }

  String response = SerialAT.readString();
  Serial.println(response);
}

gpsReading getGPS()
{
  Serial.println("Checking GPS data.");
  gpsReading gps;

  delay(500);
  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    Serial.println(response);

    // _gps.encode(response);
  }

  return gps;
}