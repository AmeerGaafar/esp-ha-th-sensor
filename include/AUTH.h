#ifndef ESP_HA_TH_SENSOR_AUTH_H
#define ESP_HA_TH_SENSOR_AUTH_H

#include <Arduino.h>

const char* WIFI_SSID PROGMEM = "tmp8c9dd94616321df0c467"; //your WiFi Name
const char* WIFI_PASSWORD PROGMEM = "2714y41E76jn7KBi3nC060Yd5T9ChH1i";  //Your Wifi Password

const char* HTTPUSERNAME PROGMEM = "V8Hq7ui626F8h0t";
const char* HTTPPASSWORD PROGMEM = "XYZ2iSF72U445WS";

const char* MQTTROOT PROGMEM = "esphadevs";
//const char* MQTTSERVER PROGMEM = "192.168.1.242"; // wrong server for testing
const char* MQTTSERVER PROGMEM = "192.168.1.244";
const int MQTTPORT PROGMEM = 1883;
const char* MQTTUSERNAME PROGMEM = "mosquitto_client";
const char* MQTTPASSWORD PROGMEM = "XYZ2iSF72U445WS";
const int MQTTQOS PROGMEM = 0;

#endif //ESP_HA_TH_SENSOR_AUTH_H
