#include <Arduino.h>
#include <ArduinoJson.h>
#include <HACouplers.h>
#include <logging.h>

HADevice::HADevice(const String &name, const String &manufacturer, const String &model, const String &softwareVersion){
    this->_name=name;
    this->_manufacturer=manufacturer;
    this->_model=model;
    this->_softwareVersion=softwareVersion;
}

String HADevice::manufacturer(){ return _manufacturer;}
String HADevice::name(){ return _name;}
String HADevice::model(){ return _model;}
String HADevice::softwareVersion(){ return _softwareVersion;}


HASensorCoupler::HASensorCoupler(const String &hostName, const String &sensorName, const String &unitOfMeasure,
                                 const String &deviceClass, const String &valueTpl, HADevice *device, MQTTClient *mqttClient){
    this->_hostName=hostName;
    this->_sensorName=sensorName;
    this->_unitOfMeasure=unitOfMeasure;
    this->_deviceClass=deviceClass;
    this->_valueTpl=valueTpl;
    this->_device=device;
    this->_mqttClient=mqttClient;
    this->_lastPublishedDiscovery=0;
    this->_lastPublishedState=0;
}

String HASensorCoupler::discoveryTopic(){
    return "homeassistant/sensor/" + _hostName + "-" + _sensorName + "/config";
}

String HASensorCoupler::stateTopic(){
    return _mqttClient->dataRoot() + "/" + _hostName + "/" + "sensor" + "/" + _sensorName;
}

String HASensorCoupler::discoveryMessage(){
    DynamicJsonDocument jsonDoc(1024);

    jsonDoc["~"]= _mqttClient->dataRoot() + "/" + _hostName + "/";
    jsonDoc["name"]= _device->name() + " (" + _sensorName + ")";
    jsonDoc["stat_t"]= "~sensor/" + _sensorName;
    jsonDoc["unit_of_meas"]=_unitOfMeasure;
    jsonDoc["dev_cla"]=_deviceClass;
    jsonDoc["val_tpl"]=_valueTpl;
    jsonDoc["exp_aft"]=43200;
    jsonDoc["frc_upd"]="False";
    jsonDoc["avty_t"]="~"+_mqttClient->lwtKeyword();
    jsonDoc["pl_avail"]=_mqttClient->onlineKeyword();
    jsonDoc["pl_not_avail"]=_mqttClient->offlineKeyword();
    jsonDoc["qos"]=0;
    jsonDoc["uniq_id"]= _hostName + "-" + _sensorName;
    jsonDoc["dev"]["ids"][0]= _hostName;
    jsonDoc["dev"]["mf"]=_device->manufacturer();
    jsonDoc["dev"]["mdl"]=_device->model();
    jsonDoc["dev"]["name"]=_device->name();
    jsonDoc["dev"]["sw"]=_device->softwareVersion();

    String discoveryMessage = "";
    discoveryMessage.reserve(1024);
    serializeJson(jsonDoc, discoveryMessage);

return String(discoveryMessage);  
}

bool HASensorCoupler::publishDiscovery(){
    bool result=true;
    LOG_INFO("Publish discovery topic: %s",discoveryTopic().c_str())
    LOG_INFO("Publish discovery message: %s",discoveryMessage().c_str())
    result=_mqttClient->publish(discoveryTopic(),discoveryMessage(),true);
    _discoveryAttempts++;
    if (result){
        _lastPublishedDiscovery=millis();
    } else {
        _discoveryErrors++;
    }
    return result;
}

bool HASensorCoupler::publishState(float state){
bool result=true;
char buffer[255];

    snprintf(buffer,255,"%.2f",state);
    //LOG_INFO("---->Publish state topic: [%s] state:[%s]",stateTopic().c_str(),buffer)
    result=_mqttClient->publish(stateTopic(),String(buffer),false);
    _publishAttempts++;
    if (result){
        _lastPublishedState=millis();
    } else {
        _publishErrors++;
    }
    return result;
}

int HASensorCoupler::publisErrors() const{return _publishErrors;}
int HASensorCoupler::publisAttempts() const{return _publishAttempts;}
int HASensorCoupler::discoveryErrors() const {return _discoveryErrors;}
int HASensorCoupler::discoveryAttempts() const {return _discoveryAttempts; }
uint32_t HASensorCoupler::lastDiscovery() const{return _lastPublishedDiscovery;}
uint32_t HASensorCoupler::lastPublish() const{return _lastPublishedState;}

