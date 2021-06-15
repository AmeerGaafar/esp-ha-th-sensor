#ifndef ESP_HA_COMMONS_LOGGING_H
#define ESP_HA_COMMONS_LOGGING_H
#include <Arduino.h>

#define LOG_LEVEL 3

#if LOG_LEVEL >=3
#define LOG_INFO(fmt, ...)   {  \
                                char paramBuffer[512]; \
                                char outputBuffer[512]; \
                                uint32_t timestamp=millis();\
                                snprintf_P(paramBuffer,512,fmt,##__VA_ARGS__); \
                                snprintf(outputBuffer,512,"%05u.%03u: [INFO] (%s:%d) %s\n",timestamp / 1000, timestamp % 1000,__FILE__,__LINE__,paramBuffer); \
                                Serial.println(outputBuffer); \
                            }
#else
#define LOG_INFO(fmt, ...)  // empty definition
#endif

#if LOG_LEVEL >=2
#define LOG_WARN(fmt, ...)   { \
                                char paramBuffer[512]; \
                                char outputBuffer[512];\
                                uint32_t timestamp=millis();\
                                snprintf(paramBuffer,512,fmt,##__VA_ARGS__); \
                                snprintf(outputBuffer,512,"%05u.%03u: [WARN] (%s:%d) %s\n",timestamp / 1000, timestamp % 1000,__FILE__,__LINE__,paramBuffer); \
                                Serial.println(outputBuffer); \
                            }
#else
#define LOG_WARN(fmt, ...) // empty definition
#endif

#if LOG_LEVEL >=1
#define LOG_ERROR(fmt, ...)   { \
                                char paramBuffer[512]; \
                                char outputBuffer[512];\
                                uint32_t timestamp=millis();\
                                snprintf(paramBuffer,512,fmt,##__VA_ARGS__); \
                                snprintf(outputBuffer,512,"%05u.%03u: [ERROR] (%s:%d) %s\n",timestamp / 1000, timestamp % 1000,__FILE__,__LINE__,paramBuffer); \
                                Serial.println(outputBuffer); \
                            }
#else
#define LOG_ERROR(fmt, ...) // empty definition
#endif

#endif //ESP_HA_COMMONS_LOGGING_H
