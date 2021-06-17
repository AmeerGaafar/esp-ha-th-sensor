#include <WiFiClient.h>
#include <AsyncMqttClient.h>
#include <MQTTClient.h>
#include <TimerUtils.h>
#include <logging.h>

static MQTTClient *mqttClientSingletonInstance = nullptr;

static const char OFFLINE_STRING[] = "Offline"; // cannot be PROGMEM
static const char ONLINE_STRING [] = "Online"; // // cannot be PROGMEM

static const char MQTT_LWT[] PROGMEM = "status";

MQTTClient *MQTTClient::instance() { return mqttClientSingletonInstance; }

MQTTClient *MQTTClient::create(ESP8266WiFiClass* wifi, const String &server, int port, const String &clientId, const String &username,
                               const String &password, const String &root, int qos, const String &logTopic) {
    if (mqttClientSingletonInstance== nullptr) {
        mqttClientSingletonInstance = new MQTTClient(wifi, server, port, clientId, username, password, root, qos,logTopic);
        return mqttClientSingletonInstance;
    }
    else {
        return mqttClientSingletonInstance;
    }
}

MQTTClient::MQTTClient(ESP8266WiFiClass* wifi,const String &server, int port, const String &clientId, const String &username,
                       const String &password, const String &root, int qos, const String &logTopic) {

    this->_wifi=wifi;
    this->_server = server;
    this->_clientId = clientId;
    this->_port = port;
    this->_username = username;
    this->_password = password;
    this->_qos = qos;
    this->_root = root;
    this->_lwtTopic=_root + "/" + _clientId + "/" + MQTT_LWT;
    this->_logTopic = logTopic;

    this->_mqtt_client = new AsyncMqttClient();
    this->_mqtt_client->setServer(this->_server.c_str(), this->_port);
    this->_mqtt_client->setCredentials(this->_username.c_str(),this->_password.c_str());
    this->_mqtt_client->setClientId(this->_clientId.c_str());
    this->_mqtt_client->setWill(this->_lwtTopic.c_str(),_qos,true,OFFLINE_STRING);
    this->_mqtt_client->onConnect([this](bool sessionPresent){this->onConnect(sessionPresent);});
    this->_mqtt_client->onDisconnect([this](AsyncMqttClientDisconnectReason reason){return this->onDisconnect(reason);});
    this->_mqtt_client->onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total){return this->onMessageReceived(topic,payload,properties,len,index,total);});

    // we need to holdon to the handlers or they get removed!
    this->_wifiConnectHandler=_wifi->onStationModeGotIP([this](const WiFiEventStationModeGotIP& event){return this->onWifiConnected(event);});
    this->_wifiDisconnectHandler=_wifi->onStationModeDisconnected([this](const WiFiEventStationModeDisconnected& event){return this->onWifiDisconnected(event);});
}

void MQTTClient::onConnect(bool sessionPresent) {
    LOG_INFO("client %s connected to %s...", _clientId.c_str(), _server.c_str())
    _lastConnectedTime=millis();
    _wasConnected=true;
    publish(_lwtTopic,onlineKeyword(),true);
}

void MQTTClient::onDisconnect(AsyncMqttClientDisconnectReason reason) {
    _lastDisconnectedTime=millis();
    if (_wasConnected) {
        _mqttDisconnectCounter++;
        _wasConnected=false;
    }
    LOG_INFO("client %s disconnected from %s reason: %d", _clientId.c_str(), _server.c_str(), reason)
    if (_wifi->isConnected()) {
        LOG_INFO("Attempting to re-establish mqtt connection to server:%s in 2 seconds",_server.c_str())
        _mqttReconnectTimer.once(2, [this](){return this->initiateConnection();});
    } else {
        LOG_INFO("Wifi is NOT connected, will not try to re-establish connection to mqtt server")
    }
}

void MQTTClient::onMessageReceived(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len,size_t index, size_t total) {
    LOG_INFO("message received in topic %s", topic)
    _lastConnectedTime=millis();
}

void MQTTClient::onWifiConnected(const WiFiEventStationModeGotIP &event) {
    LOG_INFO("WiFi connected. IP address: %s Hostname: %s",_wifi->localIP().toString().c_str(),_wifi->hostname().c_str())
    this->initiateConnection();
}

void MQTTClient::onWifiDisconnected(const WiFiEventStationModeDisconnected &event) {
    LOG_INFO("Disconnected from WiFi...")
    _wifiDisconnectCounter++;
    _mqttReconnectTimer.detach();
}

String MQTTClient::dataRoot() {
    return this->_root;
}


String MQTTClient::lwtKeyword(){
    return String(MQTT_LWT);
}

String MQTTClient::onlineKeyword(){
    return String(ONLINE_STRING);
}

String MQTTClient::offlineKeyword() {
    return String(OFFLINE_STRING);
}

bool MQTTClient::isConnected() {
    bool res = _mqtt_client->connected();
    if (res) _lastConnectedTime=millis();
    return res;
}

void MQTTClient::initiateConnection() {
    LOG_INFO("Initiating connection to mqtt server:%s ...",_server.c_str())
    this->_mqtt_client->connect();
}

void MQTTClient::loop() {
    // async libs do not need to loop!
}

void MQTTClient::begin() {

}

void MQTTClient::subscribe(const String &topic_name) {
    if (!isConnected()) return;
    _lastConnectedTime=millis();
    if (_mqtt_client->subscribe(topic_name.c_str(), _qos)) {
        LOG_INFO("client %s subscribed to %s", _clientId.c_str(), topic_name.c_str())
    } else {
        LOG_INFO("client %s FAILED to subscribe to %s", _clientId.c_str(), topic_name.c_str())
    }
}

void MQTTClient::unsubscribe(const String &topicName) {
    if (!isConnected()) return;
    _lastConnectedTime=millis();
    if (_mqtt_client->unsubscribe(topicName.c_str())) {
        LOG_INFO("client %s unsubscribed to %s", _clientId.c_str(), topicName.c_str())
    } else {
        LOG_INFO("client %s FAILED to unsubscribe to %s", _clientId.c_str(), topicName.c_str())
    }
}

String MQTTClient::lwtTopic() {
    return _lwtTopic;
}

void MQTTClient::log(String &logMessage) {
    if (!isConnected()) return;
    bool result = publish(_logTopic, logMessage, false);
    if (result) {
        _mqttLogCounter++;
        _mqttSentCounter--;
    }
    else {
        _mqttLogErrors++;
        _mqttSendErrors--;
    }
}

bool MQTTClient::publish(const String &topic, const String &payload, boolean retained) {
    if (!isConnected()) return false;
    _lastConnectedTime=millis();
    bool result = _mqtt_client->publish(topic.c_str(), _qos,retained,payload.c_str());

    if (result) _mqttSentCounter++;
    else _mqttSendErrors++;

    return result;
}
