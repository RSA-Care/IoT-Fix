#include "A9G.h"
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

TinyGPSPlus _gps;
HardwareSerial SerialAT(2);

bool CGATT = false;
bool CGACT = false;

unsigned long startTime = 0;

void reset_A9G()
{
  startTime = millis();
  SerialAT.print("AT+RST=1\r");
  bool ready = false;
  while (!ready)
  {
    while (SerialAT.available())
    {
      String response = SerialAT.readString();
      Serial.println(response);
      if (response.indexOf("READY") != -1)
      {
        ready = true;
      }
    }
    delay(500);
  }
  float duration = millis() - startTime;
  Serial.print("Reset duration: ");
  Serial.print(duration);
  Serial.println(" ms");
}

void A9GBegin()
{
  SerialAT.begin(115200);
  delay(500);

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

  clearScreen();
  println("GPS TRACKER");
  println("FOR DOWN SYNDROME");
  println("=================");
  println("Starting A9G...");
  // sendAT("AT+RST=1");
  // delay(120000);
  bool ready = false;
  unsigned long boot_start = millis();
  while (!ready && (millis() - boot_start) < 120000)
  {
    while (SerialAT.available())
    {
      String response = SerialAT.readString();
      Serial.println(response);
      if (response.indexOf("READY") != -1)
      {
        ready = true;
      }
    }
    delay(500);
  }

  if (!ready)
  {
    reset_A9G();
  }
  float start_duration = millis() - boot_start;
  Serial.print("Boot time: ");
  Serial.print(start_duration / 1000);
  Serial.println(" s");
  clearScreen();

  Serial.println("A9G module started");

  sendAT();
  delay(1000);

  // sendAT("AT+RST=1");
  // delay(1000);

  sendAT("ATI");

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
  else if (cops.indexOf("+COPS: 2"))
  {
    println("Operator selection: Deregistered");
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

  SerialAT.print(command + "\r");

  String response;
  bool ok_status = false;

  while (!ok_status)
  {
    while (SerialAT.available())
    {
      response = response + SerialAT.readString();
      println(response);

      if (response.indexOf("OK") >= 0 || response.indexOf("+CME ERROR") >= 0)
      {
        ok_status = true;
        break;
      }
      else if (response.indexOf("COMMAND NO RESPONSE!") >= 0)
      {
        response = sendAT(command);
      }
    }
    delay(500);
  }

  unsigned long duration = millis() - startTime;
  float total_time = (float)duration / 1000;
  Serial.print("response time: ");
  Serial.print(total_time);
  Serial.println(" S");
  Serial.println("==== END OF COMMAND ====");
  return response;
}

bool GPSbegin()
{
  SerialAT.begin(115200);

  bool state = false;

  String gps_begin = sendAT("AT+GPS=1");

  Serial.println(gps_begin);
  if (gps_begin.indexOf("OK") >= 0)
  {
    state = true;
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
  SerialAT.print("AT+LOCATION=2\r");
  delay(500);

  bool ok_status = false;
  bool error_status = false;
  gpsReading gps;

  while (!ok_status && !error_status)
  {
    while (SerialAT.available())
    {
      String response = SerialAT.readString();
      Serial.println(response);
      if (response.indexOf("+LOCATION: GPS NOT FIX NOW") >= 0 || response.indexOf("+CME ERROR:") >= 0)
      {
        error_status = true;
      }
      else
      {
        gps.latitude = response.substring(0, response.indexOf(","));
        gps.longitude = response.substring(response.indexOf(",") + 1, response.indexOf("\n"));
        ok_status = true;
      }
    }
  }

  // delay(500);
  // if (SerialAT.available())
  // {
  //   String response = SerialAT.readString();

  //   if (response.indexOf("+LOCATION") == -1 && response.indexOf("ERROR") == -1 && response.indexOf("INVALID") == -1)
  //   {
  //     response.replace("\n", "");
  //     response.replace("OK", "");
  //     response.trim();

  //     Serial.println(response);

  //     gps.latitude = response.substring(0, response.indexOf(","));
  //     gps.longitude = response.substring(response.indexOf(",") + 1);
  //   }
  //   else
  //   {
  //     Serial.println("\n=== UNEXPECTED ERROR ===");
  //     Serial.println(response);
  //     Serial.println("No GPS data recieved.");
  //     Serial.println("========================\n");

  //     // Reset the coordinate to 0,0 when error occured
  //     gps.latitude = "0.00";
  //     gps.longitude = "0.00";
  //   }
  // }

  return gps;
}

bool GPRScheckConnection()
{
  Serial.println("Checking connection...");
  SerialAT.print("AT+PING=1.1.1.1\r");

  bool packets = false;
  bool error = false;
  int lose;

  while (!packets && !error)
  {
    while (SerialAT.available())
    {
      String response = SerialAT.readString();
      Serial.println(response);
      if (response.indexOf("Packets: ") >= 0)
      {
        packets = true;
        lose = response.substring(response.indexOf("<") + 1, response.indexOf("%")).toInt();
      }
      if (response.indexOf("+CME ERROR:") >= 0)
      {
        lose = 100;
        error = true;
      }
    }
    delay(500);
  }

  Serial.println("Package lose: " + String(lose) + "%");
  return packets;
}

bool MQTT_Connection()
{
  bool error_status = false;
  bool ok_status = false;

  SerialAT.print("AT+MQTTCONN=\"34.30.152.206\",1883,\"ESP32-Client-test\",15000,0,\"admin\",\"hivemq\"\r");

  while (!ok_status && !error_status)
  {
    while (SerialAT.available())
    {
      String response = SerialAT.readString();
      Serial.println(response);
      if (response.indexOf("OK") >= 0)
      {
        ok_status = true;
      }
      else if (response.indexOf("+CME ERROR:") >= 0 || response.indexOf("COMMAND NO RESPONSE!") >= 0)
      {
        error_status = true;
      }
    }
  }

  return ok_status;
}

void GPRSMQTTPublish(String payload)
{
  Serial.println("Connecting to MQTT Broker.");

  bool error_status = false;
  bool ok_status = false;

  SerialAT.print("AT+MQTTPUB=\"test\",\"" + payload + "\",0,0,1\r");
  ok_status = false;
  while (!ok_status && !error_status)
  {
    while (SerialAT.available())
    {
      String response = SerialAT.readString();
      Serial.println(response);
      if (response.indexOf("OK") >= 0)
      {
        Serial.println("Published successfully");
        ok_status = true;
      }
      else if (response.indexOf("+CME ERROR:") >= 0)
      {
        error_status = true;
      }
    }
    delay(500);
  }

  if (error_status)
  {
    bool connection = MQTT_Connection();

    if (connection)
    {
      GPRSMQTTPublish(payload);
    }
  }
}