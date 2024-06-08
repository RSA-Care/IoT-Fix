#include "WiFiconn.h"

const char *_SSID;
const char *_PASS;

void WiFibegin(const char *SSID, const char *PASS)
{
  _SSID = SSID;
  _PASS = PASS;

  clearScreen();
  println("WiFi.");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  print("Connecting...");

  int counter = 0;
  while (!WiFi.isConnected() && counter < 20)
  {
    counter++;
    print(".");
    delay(500);
  }

  if (WiFi.isConnected())
  {
    println("\nConnected");
  }
  else
  {
    println("\nWiFi not found.");
  }
}

void WiFiReconnect()
{
  if (strlen(_SSID) == 0 || strlen(_PASS) == 0)
  {
    WiFi.begin(_SSID, _PASS);
  }

  Serial.print("WiFi reconnecting..");
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED && counter <= 30)
  {
    counter++;
    Serial.print(".");
    delay(500);
  }
}

bool checkWiFiConnection()
{
  if (WiFi.isConnected())
  {
    return true;
  }
  else
  {
    // WiFiReconnect();
    return false;
  }
}