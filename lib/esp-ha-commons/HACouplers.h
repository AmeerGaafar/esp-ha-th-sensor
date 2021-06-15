#ifndef ESP_HA_COMMONS_HACOUPLERS_H
#define ESP_HA_COMMONS_HACOUPLERS_H

#include <Arduino.h>
#include <MQTTClient.h>
#include <TimerUtils.h>

class HADevice {
public:
    HADevice(const String &name, const String &manufacturer, const String &model, const String &softwareVersion);
    String name();
    String manufacturer();
    String model();
    String softwareVersion();
private:
    String _name;
    String _manufacturer;
    String _model;
    String _softwareVersion;
};

class HASensorCoupler {
public:
    HASensorCoupler(const String &hostName, const String &sensorName, const String &unitOfMeasure,
                    const String &deviceClass, const String &valueTpl, HADevice *device, MQTTClient *mqttClient);
    String discoveryTopic();
    String discoveryMessage();
    String stateTopic();
    bool publishDiscovery();
    bool publishState(float state);
    int publisErrors() const;
    int publisAttempts() const;
    int discoveryErrors() const;
    int discoveryAttempts() const;
    uint32_t lastDiscovery() const;
    uint32_t lastPublish() const;

private:
    String _hostName;
    String _sensorName;
    String _unitOfMeasure;
    String _deviceClass;
    String _valueTpl;
    HADevice* _device;
    MQTTClient* _mqttClient;
    uint32_t _lastPublishedState;
    int _publishAttempts=0;
    int _publishErrors=0;
    uint32_t _lastPublishedDiscovery;
    int _discoveryAttempts=0;
    int _discoveryErrors=0;
};

#endif //ESP_HA_COMMONS_HACOUPLERS_H
