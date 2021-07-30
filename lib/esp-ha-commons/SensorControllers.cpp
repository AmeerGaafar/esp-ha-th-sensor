#include <SensorControllers.h>
#include <logging.h>

HAStabilizedSensorController::HAStabilizedSensorController(const String &hostName, const String &sensorName,
                                                           const String &unitOfMeasure, const String &deviceClass,
                                                           const String &valueTpl, HADevice *device, MQTTClient *mqttClient, readSensorFuncPtr readFunc) {
    this->_hostName=hostName;
    this->_sensorName=sensorName;
    this->_unitOfMeasure=unitOfMeasure;
    this->_deviceClass=deviceClass;
    this->_valueTpl=valueTpl;
    this->_device=device;
    this->_mqttClient=mqttClient;
    this->_readSensorFunc=readFunc;

    _stabilizer= new SensorStabilizer(hostName + "-" + sensorName);
    sensorCoupler=new HASensorCoupler(this->_hostName,this->_sensorName,this->_unitOfMeasure,this->_deviceClass,this->_valueTpl,this->_device,this->_mqttClient);
}

void HAStabilizedSensorController::loop(){
    if (_discoveryRequested){
        _discoveryRequested=false;
        publishDiscovery();
    } else {
        readState();
        publishState();
    }
}

void HAStabilizedSensorController::readState() {
  if (_readTimer.elapsed() > (_readFrequencySecs * 1000) / _stabilizer->bufferSize()) {
      LOG_INFO("Reading %s-%s sensor data..", _hostName.c_str(), _sensorName.c_str())

    float reading=_readSensorFunc();
    if (!isnan(reading)) {
        _stabilizer->ingest(reading);
    }
    else {
        _errorCount++;
        _lastErrorTS=millis();
        LOG_ERROR("Reading %s-%s returned an error!", _hostName.c_str(), _sensorName.c_str())
    }
    _readTimer.reset();
  }
}

void HAStabilizedSensorController::publishState() {
  if (_publishTimer.elapsed() > (_publishFrequencySecs * 1000)) {
      LOG_INFO("About to publish %s-%s reading", _hostName.c_str(), _sensorName.c_str())
    
    if (_stabilizer->isReadingRecent()) {
        float reading=_stabilizer->stableReading();
        sensorCoupler->publishState(reading);
    }
    else {
        LOG_ERROR("%s-%s reading too old to be published!", _hostName.c_str(), _sensorName.c_str())
    }

    _publishTimer.reset();
  }    
}

String HAStabilizedSensorController::currentState() {
    float reading= _stabilizer->latestStableReading();
    uint32_t sinceTS=_stabilizer->latestStableReadingSince();
    char buffer[1024];
    if (sinceTS>0) {
        String since = timeSinceOrNever(sinceTS);
        since = " (" + since + ")";
        sprintf(buffer, "%.2f %s%s", reading, _unitOfMeasure.c_str(), since.c_str());
    } else {
        sprintf(buffer, "Not Available!");
    }
    return String(buffer);
}

void HAStabilizedSensorController::publishDiscovery(){
  sensorCoupler->publishDiscovery();
}

String HAStabilizedSensorController::stateTopic(){
  return sensorCoupler->stateTopic();
}

String HAStabilizedSensorController::discoveryTopic(){
  return sensorCoupler->discoveryTopic();
}

void HAStabilizedSensorController::requestDiscovery() {
    _discoveryRequested=true;
}

String HAStabilizedSensorController::name(){
    //return _hostName + "-" + _sensorName;
    return _device->name() + " (" + _sensorName + ")";
}

String HAStabilizedSensorController::id(){
    return _hostName + "-" + _sensorName;
}
SensorStats HAStabilizedSensorController::stats(){
SensorStats stats;
    stats.name=name();
    stats.discoveryAttempts= sensorCoupler->discoveryAttempts();
    stats.discoveryErrors=sensorCoupler->discoveryErrors();
    stats.lastPublishedDiscovery=sensorCoupler->lastDiscovery();
    stats.publishAttempts=sensorCoupler->publisAttempts();
    stats.publishErrors=sensorCoupler->publisErrors();
    stats.lastPublishedState=sensorCoupler->lastPublish();
    stats.acquisitionErrors=this->_errorCount;
    stats.lastAcquisitionError=this->_lastErrorTS;
    return stats;
}

static ControllerGroup * controllerGroupSingletonInstance = nullptr;

ControllerGroup * ControllerGroup::create(){
    if (controllerGroupSingletonInstance == nullptr){
        controllerGroupSingletonInstance=new ControllerGroup();
    }
    return controllerGroupSingletonInstance;
}

ControllerGroup * ControllerGroup::instance(){
    return controllerGroupSingletonInstance;
}

ControllerGroup::ControllerGroup(){
    _count=0;
}

void ControllerGroup::add(SensorController* controller){
    if (_count>=MAX_CONTROLLER_COUNT-1) return;
    _controllers[_count]=controller;
    _count++;
}

void ControllerGroup::loop(){
    if (_count==0) return;
    if (_sensorInTurn>=_count) _sensorInTurn=0;
    _controllers[_sensorInTurn]->loop();
    _sensorInTurn++;
}

void ControllerGroup::requestDiscovery(){
    for (size_t i=0;i<_count;i++) _controllers[i]->requestDiscovery();
}
