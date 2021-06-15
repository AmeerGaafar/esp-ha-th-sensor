#ifndef ESP_HA_COMMONS_MQTTCLIENT_H
#define ESP_HA_COMMONS_MQTTCLIENT_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <AsyncMqttClient.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <AsyncTCP.h>
#endif
#include <Ticker.h>

class MQTTClient {
public:
    static MQTTClient *instance();

    static MQTTClient* create(ESP8266WiFiClass* wifi,const String &server, int port, const String &clientId, const String &username,
                              const String &password, const String &root, int qos, const String &logTopic);
    void begin();
    void loop();
    void log(String &logMessage);
    bool publish(const String &topic, const String &payload, boolean retained);

    void initiateConnection();
    String lwtTopic();
    void subscribe(const String &topic_name);
    void unsubscribe(const String &topicName);
    bool isConnected();
    String dataRoot();
    String server(){return _server;}
    int port() const{return _port;}
    String clientId(){return _clientId;}
    int qos() const{return _qos;}
    uint32_t lastConnectedTime() const{return _lastConnectedTime;}
    uint32_t lastDisconnectedTime() const{return _lastDisconnectedTime;}
    int mqttDisconnectCounter() const{return _mqttDisconnectCounter;}
    int mqttSentCounter() const{return _mqttSentCounter;}
    int mqttSendErrors() const{return _mqttSendErrors;}
    int mqttLogCounter() const{return _mqttLogCounter;}
    int mqttLogErrors() const{return _mqttLogErrors;}
    int wifiDisconnectCounter() const {return _wifiDisconnectCounter;}
    ESP8266WiFiClass* wifi() {return _wifi;}
    String logTopic(){return _logTopic;}

private:
    MQTTClient(ESP8266WiFiClass* wifi,const String &server, int port, const String &clientId, const String &username,
               const String &password, const String &root, int qos, const String &logTopic);
    AsyncMqttClient *_mqtt_client;
    String _server;
    int _port;
    String _clientId;
    String _username;
    String _password;
    String _root;
    int _qos;
    String _logTopic;

    uint32_t _lastConnectedTime = 0;
    uint32_t _lastDisconnectedTime = 0;
    int _mqttDisconnectCounter = 0;
    int _mqttSentCounter = 0;
    int _mqttSendErrors = 0;
    int _mqttLogCounter = 0;
    int _mqttLogErrors = 0;
    bool _wasConnected = false;
    String _lwtTopic="";
    ESP8266WiFiClass* _wifi= nullptr;
    Ticker _mqttReconnectTimer;
    WiFiEventHandler _wifiConnectHandler;
    WiFiEventHandler _wifiDisconnectHandler;
    int _wifiDisconnectCounter=0;

    void onConnect(bool sessionPresent);
    void onDisconnect(AsyncMqttClientDisconnectReason reason);
    void onMessageReceived(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index,size_t total);

    void onWifiConnected(const WiFiEventStationModeGotIP &ip);

    void onWifiDisconnected(const WiFiEventStationModeDisconnected &disconnected);
};

#endif //ESP_HA_COMMONS_MQTTCLIENT_H
