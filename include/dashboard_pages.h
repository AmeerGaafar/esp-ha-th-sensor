#ifndef ESP_HA_TH_SENSOR_DASHBOARD_PAGES_H
#define ESP_HA_TH_SENSOR_DASHBOARD_PAGES_H

#include <Arduino.h>
#include <SensorControllers.h>

String rootPageTemplateProcessor(const String& var);
String sensorsPageTemplateProcessor(const String& var);
String utilsPageTemplateProcessor(const String& var);
String sendDiscoveryTemplateProcessor(const String& var);

extern const char ROOT_HTML_TEMPLATE[] PROGMEM;
extern const char SENSORS_HTML_TEMPLATE[] PROGMEM;
extern const char UTILS_HTML_TEMPLATE[] PROGMEM;
extern const char MESSAGE_HTML_TEMPLATE[] PROGMEM;

typedef struct keyProcessorEntry_Type{
    const char* key;
    std::function<String(const String&)> processorFunc;
} keyProcessorEntry;


#endif //ESP_HA_TH_SENSOR_DASHBOARD_PAGES_H
