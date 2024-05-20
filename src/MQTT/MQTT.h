#if !defined(MQTT_h)
#define MQTT_h

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "OLED/OLED.h"

#define broker_url "35.209.3.73"
#define port 1883
#define user "admin"
#define pass "hivemq"
#define topic "test"

#define clientId "ESP32-Client-test" // need to create a randomize id for each device

void MQTTbegin();
void MQTTReconnect();
bool MQTTConnection();
String statusCheck();
void publish(const char *payload);

#endif // MQTT_h
