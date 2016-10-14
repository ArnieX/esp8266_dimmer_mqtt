#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// Constants
const char* autoconf_ssid  = "ESP8266_DEVICENAME"; //AP name for WiFi setup AP which your ESP will open when not able to connect to other WiFi
const char* autoconf_pwd   = "CONFIG_WIFI_PWD"; // AP password so noone else can connect to the ESP in case your router fails
const char* mqtt_server    = "XX.XX.XX.XX"; //MQTT Server IP, your home MQTT server eg Mosquitto on RPi, or some public MQTT
const int mqtt_port        = 1883; //MQTT Server PORT, default is 1883 but can be anything.

// MQTT Constants
const char* mqtt_devicestatus_set_topic              = "home/room/device_name/devicestatus";
const char* mqtt_dimlightstatus_set_topic            = "home/room/ledstrip/status";
const char* mqtt_dimlightbrightnessstatus_set_topic  = "home/room/ledstrip/brightness/status";
const char* mqtt_pingallresponse_set_topic           = "home/pingallresponse";
const char* mqtt_dimlight_get_topic                  = "home/room/ledstrip";
const char* mqtt_dimlightbrightness_get_topic        = "home/room/ledstrip/brightness";
const char* mqtt_pingall_get_topic                   = "home/pingall";

// Global
int current_brightness = 100; // LED STRIP OFF (100), LED STRIP ON (0) My dimmer module is driven LOW so 100% is 0, 0% is 100
byte state = 1;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_ota() {

  // Set OTA Password, and change it in platformio.ini
  ArduinoOTA.setPassword("ESP8266_PASSWORD");
  ArduinoOTA.onStart([]() {});
  ArduinoOTA.onEnd([]() {});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});
  ArduinoOTA.onError([](ota_error_t error) {
    if (error == OTA_AUTH_ERROR);          // Auth failed
    else if (error == OTA_BEGIN_ERROR);    // Begin failed
    else if (error == OTA_CONNECT_ERROR);  // Connect failed
    else if (error == OTA_RECEIVE_ERROR);  // Receive failed
    else if (error == OTA_END_ERROR);      // End failed
  });
  ArduinoOTA.begin();

}

void reconnect() {

  // Loop until we're reconnected
  while (!client.connected()) {

    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect

    if (client.connect(clientId.c_str())) {

      // Once connected, publish an announcement...
      client.publish(mqtt_devicestatus_set_topic, "connected");
      // ... and resubscribe
      client.subscribe(mqtt_dimlight_get_topic);
      client.subscribe(mqtt_dimlightbrightness_get_topic);
      client.subscribe(mqtt_pingall_get_topic);

    } else {

      // Wait 5 seconds before retrying
      delay(5000);

    }

  }

}

void callback(char* topic, byte* payload, unsigned int length) {

    char c_payload[length];
    memcpy(c_payload, payload, length);
    c_payload[length] = '\0';

    String s_topic = String(topic);
    String s_payload = String(c_payload);

  // Handling incoming messages

    if ( s_topic == mqtt_dimlight_get_topic ) {

      if (s_payload == "1") {

        if (state != 1) {

          // Turn ON function will set last known brightness

          client.publish(mqtt_dimlightstatus_set_topic, "1");
          state = 1;
          blink();

          setBrightness(current_brightness);


        }

      } else if (s_payload == "0") {

        if (state != 0) {

          // Turn OFF function

          client.publish(mqtt_dimlightstatus_set_topic, "0");
          state = 0;
          blink();

          setBrightness(100);


        }

      }

    } else if ( s_topic == mqtt_dimlightbrightness_get_topic ) {

      int receivedBrightness = s_payload.toInt();

      if (receivedBrightness <= 100) {

        int brightness = 100-receivedBrightness;  // Invert brightness received to reflect that the module I have is driven LOW

        setBrightness(brightness);
        client.publish(mqtt_dimlightbrightnessstatus_set_topic,c_payload);

      }

    } else if ( s_topic == mqtt_pingall_get_topic ) {

      client.publish(mqtt_pingallresponse_set_topic, "{\"livingroom_ledstrip\":\"connected\"}");
      blink();

    }


}

void setBrightness(int newbrightness) {

  // This function will animate brightness change from last known brightness to the new one
  // It takes already inverted value so 100 is OFF, 0 is full brightness

  if (newbrightness > current_brightness) {

    for (int i=current_brightness; newbrightness>i; i++) {

      analogWrite(D7, i);
      delay(10);
      current_brightness = i;

    }

  } else if (newbrightness < current_brightness) {

    for (int i=current_brightness; newbrightness<i; i--) {

      analogWrite(D7, i);
      delay(10);
      current_brightness = i;

    }

  } else if (newbrightness == current_brightness) {

    analogWrite(D7, newbrightness);

  }

}

void blink() {

  //Blink on received MQTT message
  digitalWrite(BUILTIN_LED, LOW);
  delay(25);
  digitalWrite(BUILTIN_LED, HIGH);

}

void setup() {

  pinMode(D7, OUTPUT);              //Setup pin for MOSFET
  analogWriteRange(100);            //This should set PWM range not 1023 but 100 as is %
  analogWrite(D7,100);              //Turn OFF by default
  pinMode(BUILTIN_LED, OUTPUT);     //Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.autoConnect(autoconf_ssid,autoconf_pwd);
  setup_ota();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  digitalWrite(BUILTIN_LED, HIGH);  //Turn off led as default

}

void loop() {

  if (!client.connected()) {
      reconnect();
    }
    client.loop();
    ArduinoOTA.handle();

}