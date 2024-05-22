#include "MQTT.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

String errorStatus;

void MQTTbegin()
{
  Serial.println("Connecting to MQTT...");
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected");
    return;
  }
  client.setServer(broker_url, port);

  if (client.connect(clientId, user, pass))
  {
    Serial.println("[ + ] Connected to MQTT");
  }
  else
  {
    Serial.println("[ - ] Failed to connect to MQTT");
    MQTTReconnect();
  }

  statusCheck();
}

void MQTTReconnect()
{
  Serial.print("Attempting MQTT reconnect...");
  int counter = 0;
  while (!client.connected() && counter <= 30)
  {
    if (client.connect(clientId, user, pass))
    {
      Serial.println("\n[ + ] Reconnected to MQTT");
    }
    else
    {
      Serial.print(".");
    }

    counter++;
    delay(500);
  }

  Serial.println("\n" + statusCheck());
}

bool MQTTConnection()
{
  return client.connected();
}

String statusCheck()
{
  /* ERROR STATUS:

  -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
  -3 : MQTT_CONNECTION_LOST - the network connection was broken
  -2 : MQTT_CONNECT_FAILED - the network connection failed
  -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
  0 : MQTT_CONNECTED - the client is connected
  1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
  2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
  3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
  4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
  5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect
  */
  int state = client.state();

  switch (state)
  {
  case -4:
    errorStatus = "MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time";
    break;

  case -3:
    errorStatus = "MQTT_CONNECTION_LOST - the network connection was broken";
    break;

  case -2:
    WiFiReconnect();
    errorStatus = "MQTT_CONNECT_FAILED - the network connection failed";
    break;

  case -1:
    errorStatus = "MQTT_DISCONNECTED - the client is disconnected cleanly";
    break;

  case 0:
    errorStatus = "MQTT_CONNECTED - the client is connected";
    break;

  case 1:
    errorStatus = "MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT";
    break;

  case 2:
    errorStatus = "MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier";
    break;

  case 3:
    errorStatus = "MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection";
    break;

  case 4:
    errorStatus = "MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected";
    break;

  case 5:
    errorStatus = "MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect";
    break;

  default:
    Serial.println("Unknown pubsub error.");
    break;
  }

  return errorStatus;
}

void publish(const char *payload)
{
  if (!client.connected())
  {
    clearScreen();
    println("Reconnecting MQTT Client.");
    MQTTReconnect();
    delay(1000);
    return;
  }

  if (client.publish(topic, payload, true))
  {
    println("Message published");
  }
  else
  {
    println("Message not published");
  }
}