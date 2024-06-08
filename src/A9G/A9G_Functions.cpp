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
  sendAT("AT+CPIN?");

  // Set result error message.
  sendAT("AT+CMEE=2");

  // Checking Operator Selection.
  // sendAT("AT+COPS?");
  String cops = sendAT("AT+COPS?");
  if (cops.indexOf("1") >= 0)
  {
    Serial.println("[ = ] INFO: SIM registered.");
  }
  else if (cops.indexOf("0") >= 0)
  {
    Serial.println("[ = ] INFO: SIM unregistered.");
    Serial.println("[ = ] INFO: Registering SIM..");
    // String cops_reg = sendAT("AT+COPS=0");
  }
  else
  {
    Serial.println("[ = ] ERROR: SIM unregistered.");
    Serial.println("[ = ] INFO: Registering SIM..");
    String cops_reg = sendAT("AT+COPS=0");
  }

  // checking network attachment
  String cgatt = sendAT("AT+CGATT=1");
  if (cgatt.indexOf("ERROR") != -1)
  {
    println("Network failed to attach.");
  }
  else
  {
    println("Network attached.");
    CGATT = true;
  }

  // Configuring GPRS.
  sendAT("AT+CGDCONT=1, \"IP\", \"internet\"");
  sendAT("AT+CGDCONT?");

  Serial.println("Activating and Checking GPRS PDP Context.");
  clearScreen();
  SerialAT.println("AT+CGACT=1,1");
  println("AT+CGACT=1,1");
  delay(1000);
  if (SerialAT.available())
  {
    String response = SerialAT.readString();

    if (response.indexOf("ERROR") == -1)
    {
      println("PDP Context registered.");
      CGACT = true;
    }
    else
    {
      println("Failed to register PDP Context.");
      Serial.println(response);

      delay(5000);
    }
  }

  // Checking Network Registration.
  SerialAT.println("AT+CREG?");
  delay(500);
  while (!SerialAT.available())
  {
    if (SerialAT.available())
    {
      String response = SerialAT.readString();
      Serial.println(response);
      response.trim();
      if (response.indexOf("+CREG: 1,1") != -1)
      {
        println("Network registered.");
      }
      else
      {
        println("Network not registered.");
      }
      delay(5000);
    }
  }

  Serial.println("Checking chip status");
  sendAT("AT+CIPSTATUS?");
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
  Serial.println("Waiting response..");
  while (!SerialAT.available())
  {
    Serial.println(".");
    delay(500);
  }

  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    response.trim();
    println(response);

    if (response.indexOf("COMMAND NOT FOUND") != -1)
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
  Serial.print("response time: ");
  Serial.println(duration);
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

      // Set the coordinate to 0,0 when error occured
      gps.latitude = "0.00";
      gps.longitude = "0.00";
    }
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
  // SerialAT.println("AT+MQTTCONN=\"35.209.3.73\",1883,\"ESP32-Client-test\",120,0,\"admin\",\"hivemq\"");
  SerialAT.println("AT+MQTTCONN=\"https://hivemq-bzfymzxv6a-uc.a.run.app\",1883,\"ESP32-Client-test\",120,0,\"admin\",\"hivemq\"");
  delay(500);

  bool state = false;

  if (SerialAT.available())
  {
    String response = SerialAT.readString();
    println(response);

    response.trim();

    if (response == "OK")
    {
      state = true;
    }

    state = false;
  }

  return state;
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