#include "A9G.h"
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

TinyGPSPlus _gps;
HardwareSerial SerialAT(2);

bool sim = false;
bool internet = false;
bool GPSstate = false;

unsigned long startTime = 0;

bool reset_A9G()
{
  startTime = millis();
  SerialAT.print("AT+RST=1\r");
  bool ready = false;
  while (!ready && (millis() - startTime <= 120000))
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
  delay(500);
  float duration = millis() - startTime;

  if (duration > 120000)
  {
    Serial.println("A9G reset failed");
  }

  Serial.print("Reset duration: ");
  Serial.print(duration);
  Serial.println(" ms");

  return ready;
}

void A9GBegin()
{
  SerialAT.begin(115200);
  delay(500);

  clearScreen();
  println("GPS TRACKER");
  println("FOR DOWN SYNDROME");
  println("=================");
  println("Starting A9G...");

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
    println("Response not detected.");
    println("Resetting A9G...");
    // bool reset = reset_A9G();
    if (reset_A9G())
    {
      println("Reset successful");
    }
    else
    {
      println("Reset failed");
    }
  }
  float start_duration = millis() - boot_start;
  Serial.print("Boot time: ");
  Serial.print(start_duration / 1000);
  Serial.println(" s");
  delay(3000);
  clearScreen();

  sendAT();
  delay(1000);

  // Checking SIM Card Status.
  String cpin = sendAT("AT+CPIN?");
  if (cpin.indexOf("+CPIN:READY") != -1)
  {
    sim = true;
    println("SIM Card ready to use");
  }
  else
  {
    clearScreen();
    println("[ ! ] No SIM Card detected.");
    delay(10000);
    return;
  }

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
  SerialAT.print(command + "\r");

  String response = "";
  bool ok_status = false;
  bool error_status = false;
  while (!ok_status && !error_status)
  {
    while (SerialAT.available())
    {
      String temp = SerialAT.readStringUntil('\n');
      temp.trim();
      Serial.println(temp);
      if (temp.indexOf("OK") != -1)
      {
        response += temp;
        ok_status = true;
      }
      else if (temp.indexOf("ERROR") != -1)
      {
        response += temp;
        error_status = true;
      }
      else if (temp.indexOf(command) == -1)
      {
        response += temp;
      }

      response += "\n";
    }
    delay(500);
  }

  return response;
}

bool GPSbegin()
{
  SerialAT.begin(115200);

  String checkGPS = sendAT("AT+GPS?");

  if (checkGPS.indexOf("+GPS: 1") != -1)
  {
    GPSstate = true;
    return GPSstate;
  }

  String gps_begin = sendAT("AT+GPS=1");

  Serial.println(gps_begin);

  if (gps_begin.indexOf("OK") >= 0)
  {
    GPSstate = true;
  }
  return GPSstate;
}

String splitString(String input, char delimiter, int index = 0)
{
  input.trim();
  String result;
  int prev_delimiter_index = 0;
  int temp_index = 0;

  for (int i = 0; i < input.length(); i++)
  {
    if (input[i] == delimiter)
    {
      if (temp_index == index)
      {
        if (prev_delimiter_index == 0)
        {
          result = input.substring(prev_delimiter_index, i);
        }
        else
        {
          result = input.substring(prev_delimiter_index + 1, i);
        }
      }
      temp_index++;
      prev_delimiter_index = i;
    }
  }

  if (result.isEmpty())
  {
    result = input.substring(prev_delimiter_index + 1, input.length());
  }

  return result;
}

gpsReading getGPS()
{
  gpsReading gps;

  /*
  Use this to get the latitude and longitude coordinate
  AT+LOCATION=2

  return:
  <latitude>,<longitude>

  OK
  */

  String gps_data = sendAT("AT+LOCATION=2");

  if (gps_data.indexOf("GPS NOT FIX NOW") != -1 || gps_data.indexOf("ERROR") != -1)
  {
    return gps;
  }

  gps_data.replace("OK", "");

  String lat = splitString(gps_data, ',', 0);
  String lon = splitString(gps_data, ',', 1);

  gps.latitude = lat;
  gps.longitude = lon;

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
  if (!sim)
  {
    return;
  }
  Serial.println("Connecting to MQTT Broker.");

  bool error_status = false;
  bool ok_status = false;

  // String topic = "test";
  String topic = getData("topic.txt");
  if (topic.length() == 0)
  {
    randomizeMQTTTopic();
    topic = getData("topic.txt");
  }

  SerialAT.print("AT+MQTTPUB=\"" + topic + "\",\"" + payload + "\",0,0,1\r");
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

void oledHeader()
{
  String topic = getData("topic.txt");
  String signal_data = sendAT("AT+CSQ");
  String raw_data = splitString(signal_data, ':', 1);
  raw_data.replace(" ", "");
  int signal_strength = splitString(raw_data, ',').toInt();
  float percent_signal_strength = (signal_strength / 31) * 100;

  clearScreen();
  println("Topic : " + topic + "\n");
}