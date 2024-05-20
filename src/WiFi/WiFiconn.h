#if !defined(WiFiconn_h)
#define WiFiconn_h

#include <Arduino.h>
#include <WiFi.h>
#include "OLED/OLED.h"

void WiFibegin(const char *SSID, const char *PASS);
bool checkConnection();

#endif // WiFiconn_h
