#ifndef ESP_HA_COMMONS_DNS_H
#define ESP_HA_COMMONS_DNS_H

#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <ESPmDNS.h>
#endif  // ESP32

#include <DNSServer.h>

#endif //ESP_HA_COMMONS_DNS_H
