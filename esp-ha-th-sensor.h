#ifndef ESP_HA_TH_SENSOR
#define ESP_HA_TH_SENSOR

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif  // ESP8266

const char* WIFI_SSID = "tmp8c9dd94616321df0c467"; //your WiFi Name
const char* WIFI_PASSWORD = "2714y41E76jn7KBi3nC060Yd5T9ChH1i";  //Your Wifi Password
const char* HOSTNAME = "esphahts";

const int   HTTP_PORT = 80;
const char* HTTPUSERNAME = "V8Hq7ui626F8h0t";
const char* HTTPPASSWORD = "XYZ2iSF72U445WS";

const char* MQTTSERVER = "192.168.1.244";
const char* MQTTPORT = "1883";
const char* MQTTUSERNAME = "mosquitto_client";
const char* MQTTPASSWORD = "XYZ2iSF72U445WS";

const char* CURRENT_TEMP_SENSOR_TOPIC = "shellies/shellyht-F3ABF0/sensor/temperature";
const char* CURRENT_HMDT_SENSOR_TOPIC = "shellies/shellyht-F3ABF0/sensor/humidity";

#ifndef DEBUG
#define DEBUG true  // Change to 'true' for serial debug output.
#endif  // DEBUG

#define BAUD_RATE 115200  // Serial port Baud rate.

const int8_t kDefaultIrLed = 4;  // <=- CHANGE_ME (optional)

// --------------------- Network Related Settings ------------------------------
const uint16_t kHttpPort = HTTP_PORT;  // The TCP port the HTTP server is listening on.

// ----------------------- MQTT Related Settings -------------------------------
const uint32_t kMqttReconnectTime = 5000;  // Delay(ms) between reconnect tries.

#define MQTT_ACK "sent"  // Sub-topic we send back acknowledgements on.
#define MQTT_SEND "sensor"  // Sub-topic we get new commands from.
#define MQTT_LWT "status"  // Topic for the Last Will & Testament.

#define MQTTbroadcastInterval 1 * 60  // Seconds between rebroadcasts.

//#define QOS 1  // MQTT broker should queue up any unreceived messages for us
#define QOS 0  // MQTT broker WON'T queue up messages for us. Fire & Forget.

// Text for Last Will & Testament status messages.

const uint8_t kHostnameLength = 30;
const uint8_t kPortLength = 5;  // Largest value of uint16_t is "65535".
const uint8_t kUsernameLength = 20;
const uint8_t kPasswordLength = 20;

// Constants
#define _MY_VERSION_ "v1.0.1"

const uint8_t kRebootTime = 15;  // Seconds
const uint8_t kQuickDisplayTime = 2;  // Seconds

// Common bit sizes for the simple protocols.
const char* kCommandDelimiter = ",";

// URLs
const char* kUrlRoot = "/";
const char* kUrlAdmin = "/admin";
const char* kUrlSendDiscovery = "/send_discovery";
const char* kUrlReboot = "/reboot";

const uint32_t kBroadcastPeriodMs = MQTTbroadcastInterval * 1000;  // mSeconds.
// How long should we listen to recover for previous states?
// Default is 5 seconds per IR TX GPIOs (channels) used.
const uint32_t kStatListenPeriodMs = 5 * 1000 * 1;  // mSeconds
const int32_t kMaxPauseMs = 10000;  // 10 Seconds.
const char* kSequenceDelimiter = ";";
const char kPauseChar = 'P';
#if defined(ESP8266)
const uint32_t kChipId = ESP.getChipId();
#endif  // ESP8266
#if defined(ESP32)
const uint32_t kChipId = ESP.getEfuseMac();  // Discard the top 16 bits.
#endif  // ESP32

class TimerMs {
 public:
  TimerMs();
  void reset();
  uint32_t elapsed();
 private:
  uint32_t start;
};

void mqttCallback(char* topic, byte* payload, unsigned int length);
void handleSendMqttDiscovery(void);
void subscribing(const String topic_name);
void unsubscribing(const String topic_name);

bool reconnect(void);
//void receivingMQTT(String const topic_name, String const callback_str);
void callback(char* topic, byte* payload, unsigned int length);
void sendMQTTDiscovery();
void doBroadcast(TimerMs *timer, const uint32_t interval,const bool retain,const bool force);
void debug(const char *str);
void doRestart(const char* str, const bool serial_only = false);
String uint64ToString(uint64_t input);
String msToHumanString(uint32_t const msecs);
String timeElapsed(uint32_t const msec);
String timeSince(uint32_t const start);
String msToString(uint32_t const msecs);
String gpioToString(const int16_t gpio);
bool hasUnsafeHTMLChars(String input);
String htmlHeader(const String title, const String h1_text = "");
String htmlEnd(void);
String htmlButton(const String url, const String button,
                  const String text = "");
String htmlMenu(void);
String addJsReloadUrl(const String url, const uint16_t timeout_s,
                      const bool notify);
String htmlSelectBool(const String name, const bool def);
void handleAdmin(void);
void handleInfo(void);
void handleReset(void);
void handleReboot(void);
void setupOTA();

uint16_t countValuesInStr(const String str, char sep);

void handleNotFound(void);
void setup_wifi(void);
void init_vars(void);
void setup(void);
void loop(void);
uint32_t maxSketchSpace(void);
#endif  // ESP_HA_TH_SENSOR
