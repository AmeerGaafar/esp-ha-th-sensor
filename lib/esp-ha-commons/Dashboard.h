#ifndef ESP_HA_COMMONS_DASHBOARD_H
#define ESP_HA_COMMONS_DASHBOARD_H
#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

const int HTTP_PORT = 80;
const int MaxRoutes=16;

typedef String (*templateProcessingFunc)(const String& var);

class RoutingTable {
public:
    RoutingTable(const String &username, const String &password);
    void reset();
    bool add(const String &url, const char* htmlTemplate, const templateProcessingFunc &processor,
             bool authenticate= false);
    int entryOf(const String &url);
    const char* templateOf(int entryId);
    templateProcessingFunc processorOf(int entryId);
    bool authenticateOf(int entryId);
    String& username(){return _username;}
    String& password(){return _password;}
    int count() const{return _count;}

private:
    String _routes[MaxRoutes];
    const char* _templates[MaxRoutes]={nullptr};
    templateProcessingFunc _processors[MaxRoutes]={nullptr};
    bool _authenticate[MaxRoutes]={false};
    int _count;
    String _username;
    String _password;
};

class Dashboard {
public:
    static Dashboard* instance();
    static Dashboard *
    create(const String &title, const String &hostname, const String &httpUserName, const String &httpPassword,
           const String &swVersion, const String &product, const String &model, const String &manufacturer,
           const char *messageHtmlTemplate);
    void registerGetHandler(const String &url, const char *htmlTemplate, const templateProcessingFunc &processor);
    void registerPostHandler(const String &url, const char *htmlTemplate, const templateProcessingFunc &processor);
    void begin();
    RoutingTable* routingTable() {return _routingTable;}
    String title(){return _title;}
    String hostname(){return _hostname;}
    String swVersion(){return _swVersion;}
    String product(){ return _product;}
    String model(){ return _model;}
    String manufacturer(){ return _manufacturer;}
    void requestReboot();
    void loop() const;
    void requestRouter(AsyncWebServerRequest *request);

private:
    Dashboard(const String &title, const String &hostname, const String &httpUserName,
              const String &httpPassword, const String &swVersion, const String &product, const String &model, const String &manufacturer, const char *messageHtmlTemplate);
    void registerRebootRoute();
    void registerOTARoute();
    AsyncWebServer *_server;
    String _title;
    String _hostname;
    String _swVersion;
    String _product;
    String _model;
    String _manufacturer;
    RoutingTable *_routingTable;
    const char *_messageHtmlTemplate;
    bool _reboot=false;

    void register404Handler();
};

typedef struct keyProcessorEntry_Type{
    const char* key;
    std::function<String(const String&)> processorFunc;
} keyProcessorEntry;

String substituteKey(const String& var,const keyProcessorEntry processors[],size_t processorCount);

#endif //ESP_HA_COMMONS_DASHBOARD_H
