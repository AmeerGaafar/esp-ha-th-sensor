#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#ifdef ESP8266
#include <Updater.h>
#include <ESP8266mDNS.h>
#define U_PART U_FS
#elif defined(ESP32)
#include <Update.h>
#include <ESPmDNS.h>
#define U_PART U_SPIFFS
#endif

#include <logging.h>
#include <Dashboard.h>
static size_t contentLen;

void handleFirmwareUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index){
        Serial.println("Update");
        contentLen = request->contentLength();
        // if filename includes spiffs, update the spiffs partition
        int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
#ifdef ESP8266
        Update.runAsync(true);
        if (!Update.begin(contentLen, cmd)) {
#else
            if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
#endif
            Update.printError(Serial);
        }
    }

    if (Update.write(data, len) != len) {
        Update.printError(Serial);
#ifdef ESP8266
    } else {
        LOG_INFO("Progress: %d%%\n", (Update.progress()*100)/Update.size())
#endif
    }

    if (final) {
        AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while page redirects...");
        response->addHeader("Location", "/reboot");
        request->send(response);
        if (!Update.end(true)){
            Update.printError(Serial);
        } else {
            Serial.println("Update complete");
            Serial.flush();
            Dashboard::instance()->requestReboot();
        }
    }
}
