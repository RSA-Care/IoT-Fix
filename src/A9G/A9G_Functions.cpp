#include "A9G.h"
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

TinyGPSPlus _gps;
HardwareSerial SerialAT(2);

bool CGATT = false;
bool CGACT = false;

unsigned long startTime = 0;

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

  // Checking SIM Card Status.
  String cpin = sendAT("AT+CPIN?");
  if (cpin.indexOf("+CPIN:READY") >= 0)
  {
    println("SIM Card ready to use");
  }
  else
  {
    clearScreen();
    println("No SIM Card detected.");
    delay(10000);
    return;
  }

  // Set result error message.
  sendAT("AT+CMEE=2");

  // Checking signal
  String csq = sendAT("AT+CSQ");
  if (csq.indexOf("+CSQ") >= 0)
  {
    int index = csq.indexOf(":");
    int separator = csq.indexOf(",");
    String rssi = csq.substring(index + 2, separator);
    String ber = csq.substring(separator + 1, csq.indexOf("\n"));
    Serial.println("Signal Strength: " + rssi);
    Serial.println("Bit Error Rate: " + ber);
  }

  // Checking Operator Selection.
  String cops = sendAT("AT+COPS?");
  if (cops.indexOf("+COPS: 0") >= 0)
  {
    println("Operator selection: Automatic");
  }
  else if (cops.indexOf("+COPS: 1") >= 0)
  {
    println("Operator selection: Manual");
    sendAT("AT+COPS=0");
  }
  else
  {
    println("Operator selection: Unknown");
  }

  // Checking for network connection.
  String creg = sendAT("AT+CREG?");
  if (creg.indexOf("+CREG: 1,1") >= 0)
  {
    Serial.println("[ = ] INFO: SIM registered.");
    String cgatt = sendAT("AT+CGATT?");
    // if (cgatt.indexOf("+CGATT: 1") >= 0)
    // {
    //   Serial.println("[ = ] INFO: GPRS attached.");
    // }
    // else
    // {
    //   cgatt = sendAT("AT+CGATT=1");
    //   if (cgatt.indexOf("ERROR") >= 0)
    //   {
    //     Serial.println("[ = ] ERROR: GPRS attach failed.");
    //   }
    // }

    String cgact = sendAT("AT+CGACT?"); // Check if PDP context is active
    if (cgact.indexOf("+CGACT: 0,0") >= 0)
    {
      Serial.println("[ = ] INFO: PDP context is not active.");
      String cgact_set = sendAT("AT+CGACT=1,1"); // Activate
    }

    return;
  }
  else
  {
    Serial.println("[ = ] ERROR: SIM unregistered.");
  }
}

void getInfo()
{
  sendAT("ATI");
}

String sendAT(String command)
{
  startTime = millis();
  clearScreen();
  Serial.print("\n==== ");
  Serial.print(command);
  Serial.println(" ====");

  println(command);
  SerialAT.println(command);
  delay(500);
  Serial.print("Waiting response..");
  while (!SerialAT.available())
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println();

  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    response.trim();
    println(response);

    if (response.indexOf("COMMAND NO RESPONSE!") != -1)
    {
      response = sendAT(command);
    }

    unsigned long duration = millis() - startTime;
    Serial.print("response time: ");
    Serial.println(duration);
    Serial.println("==== END OF COMMAND ====\n");

    delay(2000);

    return response;
  }

  Serial.println("[ ? ] ERROR: Unknown error on sendAT() function.");
  unsigned long duration = millis() - startTime;
  float total_time = (float)duration / 1000;
  Serial.print("response time: ");
  Serial.println(total_time);
  Serial.println("==== END OF COMMAND ====");
  return "[ ? ] ERROR: command failed to response.";
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

bool GPSbegin()
{
  SerialAT.begin(115200);
  println("=== GPS ===");
  sendAT("AT+CREG=1");

  bool state = false;

  sendAT("AT+GPS=1");

  SerialAT.println("AT+GPS?");
  delay(500);
  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    response.trim();
    Serial.println(response);

    if (response.indexOf("+GPS: 1") != -1)
    {
      state = true;
      println("GPS started.");
    }
    else
    {
      state = false;
      println("GPS not started.");
    }
  }
  return state;
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

    if (response.indexOf("+LOCATION") == -1 && response.indexOf("ERROR") == -1 && response.indexOf("INVALID") == -1)
    {
      response.replace("\n", "");
      response.replace("OK", "");
      response.trim();

      Serial.println(response);

      gps.latitude = response.substring(0, response.indexOf(","));
      gps.longitude = response.substring(response.indexOf(",") + 1);
    }
    else
    {
      Serial.println("\n=== UNEXPECTED ERROR ===");
      Serial.println(response);
      Serial.println("No GPS data recieved.");
      Serial.println("========================\n");

      // Reset the coordinate to 0,0 when error occured
      gps.latitude = "0.00";
      gps.longitude = "0.00";
    }
  }

  return gps;
}

bool GPRScheckConnection()
{
  Serial.println("Checking connection...");
  String ping = sendAT("AT+PING=1.1.1.1");

  if (ping.indexOf("ERROR") >= 0)
  {
    return false;
  }

  String package_lose = ping.substring(ping.indexOf("<") + 1, ping.indexOf(">") - 1);
  Serial.println("Package lose: " + package_lose);
  return true;
}

void GPRSMQTTPublish(String payload)
{
  Serial.println("Connecting to MQTT Broker.");
  String mqtt = sendAT("AT+MQTTCONN=\"34.30.152.206\",1883,\"ESP32-Client-test\",5,0,\"admin\",\"hivemq\"");
  Serial.println("Publishing to MQTT server...");
  String publish = sendAT("AT+MQTTPUB=\"test\"," + payload + ",0,0,1");
  if (publish.indexOf("OK") >= 0)
  {
    Serial.println("Published successfully");
  }
}