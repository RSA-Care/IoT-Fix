#if !defined(MQTT_h)
#define MQTT_h

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "WiFi/WiFiconn.h"
#include "OLED/OLED.h"

// #define broker_url "http://f6dea6546f094ce29468848d85af5038.s1.eu.hivemq.cloud"
// #define user "satria"
// #define pass "gogombc23"

#define broker_url "35.209.3.73"
#define user "admin"
#define pass "hivemq"

#define port 1883
// #define port 8883
#define topic "test"

#define clientId "ESP32-Client-test" // need to create a randomize id for each device

void MQTTbegin();
void MQTTReconnect();
bool MQTTConnection();
String statusCheck();
void publish(const char *payload);

#endif // MQTT_h
