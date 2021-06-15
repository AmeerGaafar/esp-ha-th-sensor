#include <AUTH.h>

#include <Arduino.h>

#include <DHT.h>

#include <dns.h>
#include <logging.h>

#include <Dashboard.h>
#include <MQTTClient.h>
#include <SensorControllers.h>

#include <dashboard_pages.h>

const char* THIS_VERSION PROGMEM ="v1.0.7";
static DHT* dht22;

float readDhtTemperature(){
    return dht22->readTemperature();
}

float readDhtHumidity(){
    return dht22->readHumidity();
}

float readDhtHeatIndex(){
    return dht22->computeHeatIndex(dht22->readTemperature(), dht22->readHumidity(), false);
}

void setupSensorControllers(const String& hostname){
    ControllerGroup::create();
    auto* dhtDevice=new HADevice("ESP↺HA Temperature & Humidity Sensor", "Ameer Gaafar", "ESP-HA-H&T", THIS_VERSION);

    ControllerGroup::instance()->add(new HAStabilizedSensorController(hostname, "temperature", "°C", "temperature", "{{value|float|round(1)}}", dhtDevice, MQTTClient::instance(), readDhtTemperature));
    ControllerGroup::instance()->add(new HAStabilizedSensorController(hostname, "humidity", "%", "humidity", "{{value|float|round(1)}}", dhtDevice, MQTTClient::instance(), readDhtHumidity));
    ControllerGroup::instance()->add(new HAStabilizedSensorController(hostname, "heatindex", "°C", "temperature", "{{value|float|round(1)}}", dhtDevice, MQTTClient::instance(), readDhtHeatIndex));
}

void setupWifi(const String &hostname) {
    WiFi.mode(WIFI_STA);
    WiFi.hostname(hostname.c_str());
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\n");

    delay(500);

    if (MDNS.begin(hostname.c_str())) {
        LOG_INFO("MDNS responder started")
    }

    MDNS.addService("http", "tcp", 80);
}

void setup(void) {
    String hostname;
    hostname="esphaht-"+String(ESP.getChipId(), HEX);

    #if LOG_LEVEL > 0
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
    #endif

    LOG_INFO("Host:%s has booted with sw version: %s", hostname.c_str(), THIS_VERSION)
    MQTTClient::create(&WiFi,MQTTSERVER,MQTTPORT,hostname,MQTTUSERNAME,MQTTPASSWORD,MQTTROOT,MQTTQOS,"/esphalogs/");

    dht22=new DHT(14,DHT22);
    dht22->begin();
    LOG_INFO("DHT22 started")
    delay(100);

    setupSensorControllers(hostname);
    LOG_INFO("H&T sensors started")

    Dashboard::create("ESP↺HA Temperature & Humidity Sensor", hostname, HTTPUSERNAME, HTTPPASSWORD, THIS_VERSION,
                      MESSAGE_HTML_TEMPLATE);
    Dashboard::instance()->registerGetHandler("/", ROOT_HTML_TEMPLATE, rootPageTemplateProcessor);
    Dashboard::instance()->registerGetHandler("/sensors", SENSORS_HTML_TEMPLATE, sensorsPageTemplateProcessor);
    Dashboard::instance()->registerGetHandler("/utils", UTILS_HTML_TEMPLATE, utilsPageTemplateProcessor);
    Dashboard::instance()->registerPostHandler("/publish-discovery", MESSAGE_HTML_TEMPLATE, sendDiscoveryTemplateProcessor);

    Dashboard::instance()->begin();
    LOG_INFO("Webserver started")

    setupWifi(hostname);

    LOG_INFO("Host: %s up and running",hostname.c_str())
}


void loop(void) {
    //LOG_INFO("________ Loop ___________")

    MDNS.update();
    MQTTClient::instance()->loop();
    Dashboard::instance()->loop();
    ControllerGroup::instance()->loop();
    delay(10);
}
