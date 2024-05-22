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

  Serial.println("Checking chip status");
  sendAT("AT+CIPSTATUS?");

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

String getValue(String data, char separator, int index) // the same as split function in python or nodejs
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

gpsReading getGPS()
{
  /*
  Use this to get the latitude and longitude coordinate
  AT+LOCATION=2

  return:
  <latitude>,<longitude>

  OK
  */

  Serial.println("Checking GPS data.");
  SerialAT.println("AT+LOCATION=2");
  delay(500);
  gpsReading gps;

  delay(500);
  if (SerialAT.available())
  {
    String response = SerialAT.readString();

    response.replace("\n", "");
    response.replace("OK", "");

    Serial.println(response);

    gps.latitude = getValue(response, ',', 0).toFloat();
    gps.longitude = getValue(response, ',', 1).toFloat();
  }

  return gps;
}