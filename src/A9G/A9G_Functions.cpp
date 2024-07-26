#include "A9G.h"
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

TinyGPSPlus _gps;
HardwareSerial SerialAT(2);

bool sim = true;
bool internet = false;
bool GPSstate = false;

unsigned long startTime = 0;

bool internetCheck()
{
  if (!internet)
  {
    return GPRScheckConnection();
  }

  return true;
}

bool reset_A9G()
{
  startTime = millis();
  SerialAT.print("AT+RST=1\r");
  bool ready = false;
  while (!ready && (millis() - startTime <= 240000))
  {
    while (SerialAT.available())
    {
      String response = SerialAT.readString();
      Serial.println(response);
      if (response.indexOf("READY") != -1)
      {
        sim = true;
        ready = true;
      }
      else if (response.indexOf("NO SIM CARD") != -1)
      {
        Serial.println("NO SIM CARD");
        sim = false;
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
      else if (response.indexOf("NO SIM CARD") != -1)
      {
        Serial.println("NO SIM CARD");
        sim = false;
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

  // Set result error message.
  sendAT("AT+CMEE=2");

  if (!sim)
  {
    return;
  }

  sendAT("ATI");

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

  // Checking for network connection.
  GPRScheckConnection();
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
    Serial.println(gps_data);
    Serial.println("Defaulting to last known location.");

    String data = getData("gps.txt");
    String temp_data = splitString(data, '\n', 1);
    gps.latitude = splitString(temp_data, ',');
    gps.longitude = splitString(temp_data, ',', 1);
    return gps;
  }

  gps_data.replace("OK", "");

  String lat = splitString(gps_data, ',', 0);
  String lon = splitString(gps_data, ',', 1);

  gps.latitude = lat;
  gps.longitude = lon;

  saveGPSData(gps);

  return gps;
}

bool GPRScheckConnection()
{
  bool state = false;

  sim = sendAT("AT+CPIN?").indexOf("READY") != -1 ? true : false;

  String operatorRegistration = sendAT("AT+COPS?");
  if (operatorRegistration.indexOf("+COPS: 2") != -1)
  {
    state = false;
    Serial.println("=== Deregistered from Network ===");
    Serial.println("+COPS: 2");
    return state;
  }
  else if (operatorRegistration.indexOf("+COPS: 3") != -1)
  {
    state = false;
    Serial.println("=== Set only, do not attempt registration or deregistration ===");
    Serial.println("+COPS: 3");
    return state;
  }
  else if (operatorRegistration.indexOf("+COPS: 1") != -1)
  {
    state = false;
    Serial.println("=== Manual mode ===");
    Serial.println("+COPS: 1");
    Serial.println("Using mode 4, if manual mode failed will switch to automatic mode");
    sendAT("AT+COPS=4");
    return state;
  }

  String registerStatus = sendAT("AT+CREG?");
  if (registerStatus.indexOf("1,1") != -1)
  {
    Serial.println("[ + ] Network registration enabled.");
    Serial.println("[ + ] Home network registered.");
  }
  else
  {
    state = false;
    Serial.println("[ - ] Network not registered.");
    return state;
  }

  String pdpContext = sendAT("AT+CGACT?");
  if (pdpContext.indexOf("1,1") != -1)
  {
    Serial.println("[ + ] PDP context activated.");
    state = true;
  }
  else if (pdpContext.indexOf("0,1") != -1 || pdpContext.indexOf("0,0") != -1)
  {
    Serial.println("[ - ] PDP context not activated.");
    String activatePDP = sendAT("AT+CGACT=1,1");
    Serial.println(activatePDP);
    if (activatePDP.indexOf("OK") != -1)
    {
      Serial.println("[ + ] PDP context activated.");
      state = true;
    }
    else
    {
      Serial.println("[ - ] PDP context not activated.");
      state = false;
    }
  }

  internet = state;
  return state;
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

A9G_Data deviceInfo()
{
  A9G_Data device;

  String raw_data = sendAT("AT+CSQ");
  raw_data.replace("+CSQ: ", "");

  device.signalStrength = splitString(raw_data, ',').toInt();
  device.errorRate = splitString(raw_data, ',').toInt();

  return device;
}

bool saveGPSData(gpsReading gps)
{
  String data = "latitude, longitude\n";
  data += String(gps.latitude) + ", " + String(gps.longitude);
  return saveData(data, "gps.txt");
}