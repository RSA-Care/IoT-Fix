#include "A9G.h"
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

TinyGPSPlus _gps;
HardwareSerial SerialAT(2);

bool CGATT = false;
bool CGACT = false;

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

  // checking network attachment
  SerialAT.println("AT+CGATT?");
  clearScreen();
  println("AT+CGATT?");
  delay(1000);
  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    response.replace("\n", "");
    response.replace("OK", "");
    response.replace(" ", "");
    response.trim();

    println(response);
    if (response.equals("+CGATT:1") >= 0)
    {
      println("Network attached.");
      CGATT = true;
    }
  }

  Serial.println("Configuring GPRS.");
  sendAT("AT+CGDCONT=1, \"IP\", \"internet\"");
  sendAT("AT+CGDCONT?");

  Serial.println("Activating and Checking GPRS PDP Context.");
  clearScreen();
  sendAT("AT+CGACT=1,1");
  SerialAT.println("AT+CGACT?");
  println("AT+CGACT?");
  delay(1000);
  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    Serial.println(response);
    response.replace("\n", "");
    response.replace("OK", "");
    response.replace(" ", "");

    println(response);
    if (response.equals("+CGACT: 0,0") >= 0)
    {
      Serial.println("Failed to register PDP Context.");
    }
    else
    {
      Serial.println("PDP Context registered.");
      CGACT = true;
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

String getValue(String data, char separator, int index) // the same as split function in python or nodejs #Currently not used
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
    response.trim();

    Serial.println(response);

    gps.latitude = response.substring(0, response.indexOf(","));
    gps.longitude = response.substring(response.indexOf(",") + 1);
  }

  return gps;
}

bool GPRScheckConnection()
{
  Serial.println("Checking connection...");
  if (CGACT && CGATT)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool GPRSMQTTConnect()
{
  println("[ GPRS ] Connecting to MQTT server...");
  SerialAT.println("AT+MQTTCONN=\"35.209.3.73\",1883,\"ESP32-Client-test\",120,0,\"admin\",\"hivemq\"");
  delay(500);

  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    println(response);

    response.trim();

    if (response == "OK")
    {
      return true;
    }

    return false;
  }
}

void GPRSMQTTPublish(String payload)
{
  println("[ GPRS ] Publishing to MQTT server...");
  SerialAT.println("AT+MQTTPUB=\"test\",0,\"" + payload + "\"");
  delay(500);

  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    println(response);
  }
}

void GPRSMQTTReconnect()
{
  println("[ GPRS ] Reconnecting to MQTT server...");
  SerialAT.println("AT+MQTTCONN=\"35.209.3.73\",1883,\"ESP32-Client-test\",120,0,\"admin\",\"hivemq\"");
  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    println(response);

    response.trim();

    // Check for connected response if connected then return to main loop, loop for 30 times
  }
}

bool GPRSMQTTConnectionCheck()
{
  /*
  0: Connection to the MQTT broker is successful.
  2: Connection failed due to invalid parameters.
  3: Connection failed due to network error.
  4: Connection failed due to authentication error (if username/password was used).
  5: Connection is already active (if you try to connect when already connected).
  */

  println("[ GPRS ] Checking MQTT connection...");
  SerialAT.println("AT+MQTTCONNSTAT?");
  delay(500);
  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    response.trim();
    println(response);

    // check for response and if necessary loop to reconnect
  }

  return false;
}