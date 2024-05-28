# IoT

This repo is the clean up version and optimize code for ESP32 GPS Tracker for Down Syndrome.

The end goal for this project is to be able to help parents who had kids with down syndrome to feel at ease when going out with their kids.

> # The project is divided into 3 parts
>
> 1.  [ESP32 GPS Tracker](https://github.com/RSA-Care/IoT-Fix.git)
> 2.  [Mobile App](https://github.com/RSA-Care/Mobapp.git)
> 3.  [Web App](https://github.com/RSA-Care/Website.git)

## Important functions

1. `A9GBegin();` → A9G Starting sequence
2. `GPRSMQTTConnect();` → MQTT module connection sequence using A9G GSM connection
3. `WiFibegin();` → WiFi starting sequence
4. `MQTTBegin();` → MQTT module connecting sequence using WiFi connection
