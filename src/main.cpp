#include <AUTH.h>

#include <Arduino.h>

#include <DHT.h>

#include <dns.h>
#include <logging.h>

#include <Dashboard.h>
#include <MQTTClient.h>
#include <SensorControllers.h>

#include <dashboard_pages.h>

const char* THIS_VERSION PROGMEM ="v1.0.15";
const char* THIS_TITLE PROGMEM = "ESP↺HA H&T - Temperature & Humidity Sensor";
const char* THIS_PRODUCT PROGMEM = "ESP↺HA H&T";
const char* THIS_MANUFACTURER PROGMEM = "Ameer Gaafar";
const char* THIS_MODEL PROGMEM ="ESP↺HA-H&T 8266Dev-DHT22";

static DHT* dht22;

void setupSensorControllers(const String &hostname, const String &hostnameUniqSegment) {
    String deviceName= String(THIS_PRODUCT) + " " + hostnameUniqSegment;
    ControllerGroup::create();

    auto* dhtDevice=new HADevice(deviceName, THIS_MANUFACTURER, THIS_MODEL, THIS_VERSION);

    ControllerGroup::instance()->add(
            new HAStabilizedSensorController(hostname, "temperature", "°C", "temperature", "{{value|float|round(1)}}",
                                             dhtDevice, MQTTClient::instance(),
                                             []() { return dht22->readTemperature(); }));
    ControllerGroup::instance()->add(
            new HAStabilizedSensorController(hostname, "humidity", "%", "humidity", "{{value|float|round(1)}}",
                                             dhtDevice, MQTTClient::instance(),
                                             []() { return dht22->readHumidity(); }));
    ControllerGroup::instance()->add(
            new HAStabilizedSensorController(hostname, "heatindex", "°C", "temperature", "{{value|float|round(1)}}",
                                             dhtDevice, MQTTClient::instance(), []() {
                        return dht22->computeHeatIndex(dht22->readTemperature(), dht22->readHumidity(), false);
                    }));
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
        MDNS.addService("http", "tcp", 80);
        LOG_INFO("MDNS responder started")
    }
}

void setupDashboard(const String &hostname) {
    Dashboard::create(THIS_TITLE, hostname, HTTPUSERNAME, HTTPPASSWORD, THIS_VERSION, THIS_PRODUCT,
                      THIS_MODEL, THIS_MANUFACTURER, MESSAGE_HTML_TEMPLATE);
    Dashboard::instance()->registerGetHandler("/", ROOT_HTML_TEMPLATE, rootPageTemplateProcessor);
    Dashboard::instance()->registerGetHandler("/sensors", SENSORS_HTML_TEMPLATE, sensorsPageTemplateProcessor);
    Dashboard::instance()->registerGetHandler("/utils", UTILS_HTML_TEMPLATE, utilsPageTemplateProcessor);
    Dashboard::instance()->registerPostHandler("/publish-discovery", MESSAGE_HTML_TEMPLATE, sendDiscoveryTemplateProcessor);

    Dashboard::instance()->begin();
}

void setup(void) {
    const String &hostnameUniqSegment = String(ESP.getChipId(), HEX);
    const String hostname= "esphaht-" + hostnameUniqSegment;

    #if LOG_LEVEL > 0
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
    #endif

    LOG_INFO("Host:%s has booted with sw version: %s", hostname.c_str(), THIS_VERSION)

    MQTTClient::create(&WiFi,MQTTSERVER,MQTTPORT,hostname,MQTTUSERNAME,MQTTPASSWORD,MQTTROOT,MQTTQOS,"/esphalogs/");
    LOG_INFO("MQTT Client created")

    dht22=new DHT(14,DHT22);
    dht22->begin();
    LOG_INFO("DHT22 started")

    setupSensorControllers(hostname, hostnameUniqSegment);
    LOG_INFO("H&T sensors started")

    setupDashboard(hostname);
    LOG_INFO("Webserver started")

    setupWifi(hostname);
    LOG_INFO("Host: %s up and running",hostname.c_str())
}


void loop(void) {
    MDNS.update();
    MQTTClient::instance()->loop();
    Dashboard::instance()->loop();
    ControllerGroup::instance()->loop();
}
