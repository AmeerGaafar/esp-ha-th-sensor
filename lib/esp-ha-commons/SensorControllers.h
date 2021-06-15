#ifndef ESP_HA_COMMONS_SENSOR_CONTROLLERS_H
#define ESP_HA_COMMONS_SENSOR_CONTROLLERS_H

#include <Arduino.h>
#include <HACouplers.h>
#include <SensorStabilizer.h>

const int MAX_CONTROLLER_COUNT = 16;
typedef float (*readSensorFuncPtr)();
typedef struct Type_SensorStats {
    String name;
    uint32_t lastPublishedState=0;
    int publishAttempts=0;
    int publishErrors=0;
    uint32_t lastPublishedDiscovery=0;
    int discoveryAttempts=0;
    int discoveryErrors=0;
} SensorStats;

class SensorController {
  public:
    virtual void loop() = 0 ;
    virtual void readState() = 0 ;
    virtual void publishState() = 0;
    virtual void publishDiscovery() = 0;
    virtual void requestDiscovery() = 0;
    virtual String stateTopic() = 0;
    virtual String discoveryTopic() = 0;
    virtual String currentState() = 0;
    virtual String name() = 0;
    virtual SensorStats stats() = 0;
};

class ControllerGroup {
public:
    static ControllerGroup* create();
    static ControllerGroup* instance();
    void add(SensorController* controller);
    size_t count() const{return _count;} ;
    void loop();
    void requestDiscovery();
    SensorController* at(int index){return _controllers[index];}
private:
    ControllerGroup();
    SensorController* _controllers[MAX_CONTROLLER_COUNT]={nullptr};
    size_t _count=0;
    size_t _sensorInTurn=0;
};

class HAStabilizedSensorController : public SensorController {
public:
  HAStabilizedSensorController(const String &hostName, const String &sensorName, const String &unitOfMeasure,
                               const String &deviceClass, const String &valueTpl, HADevice *device, MQTTClient *mqttClient, readSensorFuncPtr readFunc);
  void loop() override;
  void readState() override;
  void publishState() override;
  void publishDiscovery() override;
  void requestDiscovery() override;
  String stateTopic() override;
  String discoveryTopic() override;
  String currentState() override;
  String name() override;
  SensorStats stats() override;
private:
  HASensorCoupler* sensorCoupler;
  String _hostName;
  String _sensorName;
  String _unitOfMeasure;
  String _deviceClass;
  String _valueTpl;
  HADevice* _device;
  MQTTClient* _mqttClient;
  SensorStabilizer* _stabilizer;
  TimerMs _readTimer;
  TimerMs _publishTimer;
  uint32_t _readFrequencySecs=60;
  uint32_t _publishFrequencySecs=60;
  readSensorFuncPtr _readSensorFunc;
  bool _discoveryRequested=false;
};

#endif  //ESP_HA_COMMONS_SENSOR_CONTROLLERS_H
