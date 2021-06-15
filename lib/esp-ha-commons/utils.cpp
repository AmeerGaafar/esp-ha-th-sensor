#include <Arduino.h>
#include <logging.h>
#include <utils.h>

uint32_t maxSketchSpace() {
#if defined(ESP8266)
    return (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
#else  // defined(ESP8266)
    return UPDATE_SIZE_UNKNOWN;
#endif  // defined(ESP8266)
}

void restart() {
    LOG_INFO("Restarting...")
    delay(2000);  // Enough time for messages to be sent.
    ESP.restart();
    delay(5000);  // Enough time to ensure we don't return.
}


