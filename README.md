# ESP8266 (NodeMCU) dimmer control

## Dependencies
- Some MQTT server (If you have Raspberry Pi use Mosquitto)
- Optional is [Homebridge](https://github.com/nfarina/homebridge) with [MQTT Plugin](https://github.com/cflurin/homebridge-mqtt) to control the relay from iDevices
- Optional that will make your life easier with IoT is Node-RED, plus you can get decent dashboard with Node-RED Dashboard plugin
- [PlatformIO](https://github.com/platformio/platformio) best Arduino IDE available, hacked from ATOM text editor

- [WiFi Manager](https://github.com/tzapu/WiFiManager)
- [PubSubClient (MQTT)](https://github.com/knolleary/pubsubclient)
- ESP8266 WiFi library

Install using PlatformIO Library Manager

```
pio lib -g install WiFiManager
pio lib -g install PubSubClient
pio lib -g install ESP8266wifi
```

## Getting started

Update main.ino with your custom preferences



Update platformio.ini with your custom preferences (Do not change unless you want to turn OTA ON)

[14 and 15] To enable OTA for next updates uncomment these lines and change values to reflect your enviroment

To turn OTA OFF any time, just comment these lines again with ;

## Result

**Click the image to play video on YouTube:**

[![LED Strip dimming with Siri, Homebridge and NodeMCU (ESP8266)](http://img.youtube.com/vi/KB4Ermphibo/0.jpg)](https://www.youtube.com/watch?v=KB4Ermphibo)
