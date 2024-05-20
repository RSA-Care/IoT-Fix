#include "WiFiconn.h"

void WiFibegin(const char *SSID, const char *PASS)
{
  clearScreen();
  println("WiFi.");
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
}

bool checkConnection()
{
  if (WiFi.isConnected())
  {
    return true;
  }
  else
  {
    return false;
  }
}