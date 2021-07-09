#ifndef ESP_HA_COMMONS_OTA_H
#define ESP_HA_COMMONS_OTA_H
void handleFirmwareUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
#endif //ESP_HA_COMMONS_OTA_H
