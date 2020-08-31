#include "esp-ha-th-sensor.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#endif  // ESP8266
#if defined(ESP32)
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Update.h>
#endif  // ESP32
#include <WiFiClient.h>
#include <DNSServer.h>
#include <PubSubClient.h>

#define ___HTTP_PROTECT___  { if (!server.authenticate(HttpUsername, HttpPassword)) {           \
                                    debug("Basic HTTP authentication failure for /.");          \
                                    return server.requestAuthentication();                      \
                                 }                                                              \
                            }

#include <algorithm>  // NOLINT(build/include)
#include <memory>
#include <string>

// Globals
#if defined(ESP8266)
ESP8266WebServer server(kHttpPort);
#endif  // ESP8266
#if defined(ESP32)
WebServer server(kHttpPort);
#endif  // ESP32
WiFiClient espClient;

char HttpUsername[kUsernameLength + 1] = "admin";  // Default HTT username.
char HttpPassword[kPasswordLength + 1] = "";  // No HTTP password by default.
char Hostname[kHostnameLength + 1] = "iotdevice";  // Default hostname.

bool boot = true;
int8_t offset;  // The calculated period offset for this chip and library.

PubSubClient mqtt_client(espClient);
uint32_t lastReconnectAttempt = 0;
uint32_t lastConnectedTime = 0;
uint32_t lastDisconnectedTime = 0;
uint32_t mqttDisconnectCounter = 0;
uint32_t mqttSentCounter = 0;
bool wasConnected = true;

char MqttServer[kHostnameLength + 1] = "10.0.0.4";
char MqttPort[kPortLength + 1] = "1883";
char MqttUsername[kUsernameLength + 1] = "";
char MqttPassword[kPasswordLength + 1] = "";
char MqttPrefix[kHostnameLength + 1] = "";

String MqttLwt;  // Topic for the Last Will & Testament.

String MqttUniqueId;
String MqttHAName;
String MqttClientId;

// Primative lock file for gating MQTT state broadcasts.
TimerMs lastBroadcast = TimerMs();  // When we last sent a broadcast.
bool hasBroadcastBeenSent = false;
TimerMs lastDiscovery = TimerMs();  // When we last sent a Discovery.
bool hasDiscoveryBeenSent = false;
TimerMs statListenTime = TimerMs();  // How long we've been listening for.

// Debug messages get sent to the serial port.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void debug(const char *str) {
#if DEBUG
  uint32_t now = millis();
  Serial.printf("%07u.%03u: %s\n", now / 1000, now % 1000, str);
#endif  // DEBUG
}
#pragma GCC diagnostic pop

String uint64ToString(uint64_t input) {
  String result = "";
  
  uint8_t base = 10;

  // Reserve some string space to reduce fragmentation.
  // 16 bytes should store a uint64 in hex text which is the likely worst case.
  // 64 bytes would be the worst case (base 2).
  result.reserve(16);

  do {
    char c = input % base;
    input /= base;

    if (c < 10)
      c += '0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}

String msToString(uint32_t const msecs) {
  uint32_t totalseconds = msecs / 1000;
  if (totalseconds == 0) return "now";

  // Note: uint32_t can only hold up to 45 days, so uint8_t is safe.
  uint8_t days = totalseconds / (60 * 60 * 24);
  uint8_t hours = (totalseconds / (60 * 60)) % 24;
  uint8_t minutes = (totalseconds / 60) % 60;
  uint8_t seconds = totalseconds % 60;

  String result = "";
  if (days)
    result += uint64ToString(days) + ' ' + String((days > 1) ? "days": "day");
  if (hours) {
    if (result.length()) result += ' ';
    result += uint64ToString(hours) + ' ' + String((hours > 1) ? "hours": "hour");
  }
  if (minutes) {
    if (result.length()) result += ' ';
    result += uint64ToString(minutes) + ' ' + String((minutes > 1) ? "minutes" : "minute");
  }
  if (seconds) {
    if (result.length()) result += ' ';
    result += uint64ToString(seconds) + ' ' + String((seconds > 1) ? "seconds" : "second");
  }
  return result;
}

String timeElapsed(uint32_t const msec) {
  String result = msToString(msec);
  if (result.equalsIgnoreCase("now"))
    return result;
  else
    return result + F(" ago");
}

String timeSince(uint32_t const start) {
  if (start == 0)
    return F("Never");
  uint32_t diff = 0;
  uint32_t now = millis();
  if (start < now)
    diff = now - start;
  else
    diff = UINT32_MAX - start + now;
  return msToString(diff) + F(" ago");
}

String htmlMenu(void) {
  String html = F("<center>");
  html += htmlButton(kUrlRoot, F("Home"));
  html += htmlButton(kUrlAdmin, F("Admin"));
  html += F("</center><hr>");
  return html;
}

String addJsReloadUrl(const String url, const uint16_t timeout_s,
                      const bool notify) {
  String html = F(
      "<script type=\"text/javascript\">\n"
      "<!--\n"
      "  function Redirect() {\n"
      "    window.location=\"");
  html += url;
  html += F("\";\n"
      "  }\n"
      "\n");
  if (notify && timeout_s) {
    html += F("  document.write(\"You will be redirected to the main page in ");
    html += String(timeout_s);
    html += F(" seconds.\");\n");
  }
  html += F("  setTimeout('Redirect()', ");
  html += String(timeout_s * 1000);  // Convert to mSecs
  html += F(");\n"
      "//-->\n"
      "</script>\n");
  return html;
}

String htmlOptionItem(const String value, const String text, bool selected) {
  String html = F("<option value='");
  html += value + '\'';
  if (selected) html += F(" selected='selected'");
  html += '>' + text + F("</option>");
  return html;
}

String htmlHeader(const String title, const String h1_text) {
  String html = F("<html><head><title>");
  html += title;
  html += F("</title><meta http-equiv=\"Content-Type\" "
            "content=\"text/html;charset=utf-8\">"
            "</head><body><center><h1>");
  if (h1_text.length())
    html += h1_text;
  else
    html += title;
  html += F("</h1></center>");
  return html;
}

String htmlEnd(void) {
  return F("</body></html>");
}

String htmlButton(const String url, const String button, const String text) {
  String html = F("<button type='button' onclick='window.location=\"");
  html += url;
  html += F("\"'>");
  html += button;
  html += F("</button> ");
  html += text;
  return html;
}

// Admin web page
void handleAdmin(void) {

  ___HTTP_PROTECT___
  
  String html = htmlHeader(F("Administration"));
  html += htmlMenu();
  html += F("<h3>Special commands</h3>");
  html += htmlButton(
      kUrlSendDiscovery, F("Send MQTT Discovery"),
      F("Send an MQTT discovery message to Home Assistant.<br><br>"));
  html += htmlButton(
      kUrlReboot, F("Reboot"),
      F("A simple reboot of the ESP. <small>ie. No changes</small><br>"
        "<br>"));
  html += F("<hr><h3>Update firmware</h3><p>"
            "<b><mark>Warning:</mark></b><br> ");
    html += F(
        "<i>Updating your firmware may screw up your access to the device. "
        "If you are going to use this, know what you are doing first "
        "(and you probably do).</i><br>"
        "<form method='POST' action='/update' enctype='multipart/form-data'>"
          "Firmware to upload: <input type='file' name='update'>"
          "<input type='submit' value='Update'>"
        "</form>");
  html += htmlEnd();
  server.send(200, "text/html", html);
}

uint32_t maxSketchSpace(void) {
#if defined(ESP8266)
  return (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
#else  // defined(ESP8266)
  return UPDATE_SIZE_UNKNOWN;
#endif  // defined(ESP8266)
}

// Info web page
void handleInfo(void) {
 
___HTTP_PROTECT___

  String html = htmlHeader(F("ESP-HA Temperature & Humidity Sensor"));
  html += htmlMenu();
  html +=
    "<h3>General</h3>"
    "<p>Hostname: " + String(Hostname) + "<br>"
    "IP address: " + WiFi.localIP().toString() + "<br>"
    "MAC address: " + WiFi.macAddress() + "<br>"
    "Booted: " + timeSince(1) + "<br>" +
    "Version: " _MY_VERSION_ "<br>"
    "Built: " __DATE__
      " " __TIME__ "<br>"
    "Period Offset: " + String(offset) + "us<br>"
#if defined(ESP8266)
    "ESP8266 Core Version: " + ESP.getCoreVersion() + "<br>"
    "Free Sketch Space: " + String(maxSketchSpace() >> 10) + "k<br>"
#endif  // ESP8266
#if defined(ESP32)
    "ESP32 SDK Version: " + ESP.getSdkVersion() + "<br>"
#endif  // ESP32
    "Cpu Freq: " + String(ESP.getCpuFreqMHz()) + "MHz<br>"
    "Serial debugging: "
#if DEBUG
        + String(false ? "off" : "on") +
#else  // DEBUG
        D_STR_OFF
#endif  // DEBUG
        "<br>"
    "</p>"
    "<h4>MQTT Information</h4>"
    "<p>Server: " + String(MqttServer) + ":" + String(MqttPort) + " <i>(" +
    (mqtt_client.connected() ? "Connected " + timeSince(lastDisconnectedTime)
                             : "Disconnected " + timeSince(lastConnectedTime)) +
    ")</i><br>"
    "Disconnections: " + String(mqttDisconnectCounter - 1) + "<br>"
    "Max Packet Size: " + MQTT_MAX_PACKET_SIZE + " bytes<br>"
    "Client id: " + MqttClientId + "<br>"
    "LWT topic: " + MqttLwt + "<br>"
    "QoS: " + String(QOS) + "<br>"

    "Total published: " + String(mqttSentCounter) + "<br>"
    "</p>"
    "<p>"
    "Last discovery sent: " + (hasDiscoveryBeenSent ? timeElapsed(lastDiscovery.elapsed()) :String("<i>Never</i>")) +"<br>"
    "Discovery topic: " + "MqttDiscovery(fixme)" + "<br>" +
    "State topics: " + "fixme" + '/' + "YYYYYY" +
        '/' + "XXXXXXXX" +
    "</p>"
    // Page footer
    "<hr><p><small><center>"
      "<i>(Note: Page will refresh every 60 seconds.)</i>"
    "<centre></small></p>";
  html += addJsReloadUrl("/", 60, false);
  html += htmlEnd();
  server.send(200, "text/html", html);

}

void doRestart(const char* str, const bool serial_only) {
  debug(str);
  delay(2000);  // Enough time for messages to be sent.
  ESP.restart();
  delay(5000);  // Enough time to ensure we don't return.
}

// Reboot web page
void handleReboot() {

  ___HTTP_PROTECT___
  
  server.send(200, "text/html",
    htmlHeader(F("Device restarting.")) +
    "<p>Try connecting in a few seconds.</p>" +
    addJsReloadUrl(kUrlRoot, kRebootTime, true) +
    htmlEnd());
  doRestart("Reboot requested");
}

void handleNotFound(void) {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup_wifi(void) {
  WiFi.mode(WIFI_STA);
  WiFi.hostname(Hostname);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  debug(String(String("WiFi connected. IP address: ")+WiFi.localIP().toString()+String(" Hostname: ")+String(Hostname)).c_str());

}

void init_vars(void) {
  // If we have a prefix already, use it. Otherwise use the hostname.
  if (!strlen(MqttPrefix)) strncpy(MqttPrefix, Hostname, kHostnameLength);
  // Topic for the Last Will & Testament.
  MqttLwt = String(MqttPrefix) + '/' + MQTT_LWT;

  //MqttDiscovery = "homeassistant/sensor/" + String(MqttPrefix) + "/config";
  MqttUniqueId = WiFi.macAddress();
  MqttUniqueId.replace(":", "");
  MqttHAName = String(Hostname) + " Temperature and Humidity Sensor";
  // Create a unique MQTT client id.
  MqttClientId = String(Hostname);
}

void setup(void) {

  strncpy(Hostname, HOSTNAME, kHostnameLength);
  strncat(Hostname,String(kChipId, HEX).c_str(),kHostnameLength);
  strncpy(HttpUsername, HTTPUSERNAME, kUsernameLength);
  strncpy(HttpPassword, HTTPPASSWORD, kPasswordLength);
  strncpy(MqttServer, MQTTSERVER, kHostnameLength);
  strncpy(MqttPort, MQTTPORT, kPortLength);
  strncpy(MqttUsername, MQTTUSERNAME, kUsernameLength);
  strncpy(MqttPassword, MQTTPASSWORD, kPasswordLength);
  strncpy(MqttPrefix, "espha-sensors/", kHostnameLength);
  strncat(MqttPrefix, Hostname, kHostnameLength);
  
#if DEBUG
#if defined(ESP8266)
    Serial.begin(BAUD_RATE, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
    Serial.begin(BAUD_RATE, SERIAL_8N1);
#endif  // ESP8266
    while (!Serial)  // Wait for the serial connection to be establised.
      delay(50);
    Serial.println();
    debug(String(String(Hostname)+"  has booted with sw version:"+String(_MY_VERSION_)).c_str());
#endif  // DEBUG

  setup_wifi();

  // Wait a bit for things to settle.
  delay(500);

  lastReconnectAttempt = 0;

  if (MDNS.begin(Hostname)) {
    debug(String(String(Hostname)+" MDNS responder started").c_str());
  }

  // Setup the root web page.
  server.on(kUrlRoot, handleInfo);
  // Setup the admin page.
  server.on(kUrlAdmin, handleAdmin);
  // Reboot url
  server.on(kUrlReboot, handleReboot);
  // MQTT Discovery url
  server.on(kUrlSendDiscovery, handleSendMqttDiscovery);
  // Finish setup of the mqtt clent object.
  mqtt_client.setServer(MqttServer, atoi(MqttPort));
  mqtt_client.setCallback(mqttCallback);
  
  init_vars();

  setupOTA(); 

  // Set up an error page.
  server.onNotFound(handleNotFound);

  server.begin();
  debug("HTTP server started");

  MDNS.addService("http", "tcp", 80);
}

void setupOTA() {
    // Setup the URL to allow Over-The-Air (OTA) firmware updates.
    server.on("/update", HTTP_POST, [](){
        delay(1000);
        server.send(200, "text/html",
            htmlHeader(F("Updating firmware")) +
            "<hr>"
            "<h3>Warning! Don't power off the device for "
            "60 seconds!</h3>"
            "<p>The firmware is uploading and will try to flash itself. "
            "It is important to not interrupt the process.</p>"
            "<p>The firmware upload seems to have " +
            String(Update.hasError() ? "FAILED!" : "SUCCEEDED!") +
            " Rebooting! </p>" +
            addJsReloadUrl(kUrlRoot, 20, true) +
            htmlEnd());
        doRestart("Post firmware reboot.");
      }, [](){
        if (!server.authenticate(HttpUsername, HttpPassword)) {
          debug("Basic HTTP authentication failure for /update.");
          return server.requestAuthentication();
        }
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
          debug("Update:");
          debug(upload.filename.c_str());
#if defined(ESP8266)
          WiFiUDP::stopAll();
#endif  // defined(ESP8266)
          if (!Update.begin(maxSketchSpace())) {  // start with max available
#if DEBUG
              Update.printError(Serial);
#endif  // DEBUG
          }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          if (Update.write(upload.buf, upload.currentSize) !=
              upload.currentSize) {
#if DEBUG
              Update.printError(Serial);
#endif  // DEBUG
          }
        } else if (upload.status == UPLOAD_FILE_END) {
          // true to set the size to the current progress
          if (Update.end(true)) {
            debug("Update Success:");
            debug(String(upload.totalSize).c_str());
            debug("Rebooting...");
          }
        }
        yield();
      });
}

// MQTT subscribing to topic
void subscribing(const String topic_name) {
  // subscription to topic for receiving data with QoS.
  if (mqtt_client.subscribe(topic_name.c_str(), QOS))
    debug("Subscription OK to:");
  else
    debug("Subscription FAILED to:");
  debug(topic_name.c_str());
}

// Un-subscribe from a MQTT topic
void unsubscribing(const String topic_name) {
  // subscription to topic for receiving data with QoS.
  if (mqtt_client.unsubscribe(topic_name.c_str()))
    debug("Unsubscribed OK from:");
  else
    debug("FAILED to unsubscribe from:");
  debug(topic_name.c_str());
}

bool reconnect(void) {
  // Loop a few times or until we're reconnected
  uint16_t tries = 1;
  while (!mqtt_client.connected() && tries <= 3) {
    int connected = false;
    // Attempt to connect
    debug(("Attempting MQTT connection to " + String(MqttServer) + ":" +
           String(MqttPort) + "... ").c_str());
    if (strcmp(MqttUsername, "") && strcmp(MqttPassword, "")) {
      debug("Using mqtt username/password to connect.");
      connected = mqtt_client.connect(MqttClientId.c_str(),
                                      MqttUsername, MqttPassword,
                                      MqttLwt.c_str(),
                                      QOS, true, "Offline");

    } else {
      debug("Using password-less mqtt connection.");
      connected = mqtt_client.connect(MqttClientId.c_str(), MqttLwt.c_str(),
                                      QOS, true, "Offline");
    }
    if (connected) {
      // Once connected, publish an announcement...
      debug("(Re)Connected.");

      // Update Last Will & Testament to say we are back online.
      mqtt_client.publish(MqttLwt.c_str(), "Online", true);
      mqttSentCounter++;
    } else {
      debug(("failed, rc=" + String(mqtt_client.state()) +
            " Try again in a bit.").c_str());
      // Wait for a bit before retrying
      delay(tries << 7);  // Linear increasing back-off (x128)
    }
    tries++;
  }
  return mqtt_client.connected();
}

// MQTT Discovery web page
void handleSendMqttDiscovery(void) {

  ___HTTP_PROTECT___

  server.send(200, "text/html",
      htmlHeader(F("Sending MQTT Discovery message")) +
      htmlMenu() +
      "<p>The Home Assistant MQTT Discovery message is being sent." +
      " It will show up in Home Assistant in a few seconds."
      "</p>"
      "<h3>Warning!</h3>"
      "<p>Home Assistant's config for this device is reset each time this is "
      " is sent.</p>" +
      addJsReloadUrl(kUrlRoot, kRebootTime, true) +
      htmlEnd());
  sendMQTTDiscovery();
}

void doBroadcast(TimerMs *timer, const uint32_t interval,const bool retain,const bool force) {
  if (force || (timer->elapsed() > interval)) {
    debug("======== Sending MQTT sensor reading.========");
    timer->reset();  // It's been sent, so reset the timer.
    hasBroadcastBeenSent = true;
  }
}

// Callback function, when we receive an MQTT value on the topics
// subscribed this function is called
void mqttCallback(char* topic, byte* payload, unsigned int length) { /*
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  // Allocate the correct amount of memory for the payload copy
  byte* payload_copy = reinterpret_cast<byte*>(malloc(length + 1));
  if (payload_copy == NULL) {
    debug("Can't allocate memory for `payload_copy`. Skipping callback!");
    return;
  }
  // Copy the payload to the new buffer
  memcpy(payload_copy, payload, length);

  // Conversion to a printable string
  payload_copy[length] = '\0';
  String callback_string = String(reinterpret_cast<char*>(payload_copy));
  String topic_name = String(reinterpret_cast<char*>(topic));

  // launch the function to treat received data
  // receivingMQTT(topic_name, callback_string);

  // Free the memory
  free(payload_copy);
*/}

void sendMQTTDiscovery() {
  String humidity_discovery_message=String(
      "{"
      "\"~\":\"" + String(MqttPrefix) +"/"+ "\","
      "\"name\":\"" + MqttHAName + " (humidity)" + "\","
      "\"stat_t\":\"" + "~sensor/humidity" + "\","
      "\"unit_of_meas\":\"" + "%" + "\","      
      "\"dev_cla\":\"" + "humidity" + "\","
      "\"val_tpl\":\"" + "{{value|float|round(1)}}" + "\","
      "\"exp_aft\":" + "43200" + ","
      "\"frc_upd\":\"" + "false" + "\","
      "\"qos\":" + "0" + ","
      "\"uniq_id\":\"" + MqttUniqueId + "-humidity" + "\","
      "\"device\":{"
        "\"ids\":[\"" + MqttUniqueId + "\"],"
        "\"connections\":[[\"mac\",\"" + WiFi.macAddress() + "\"]],"
        "\"mf\":\"Ameer Gaafar\","
        "\"model\":\"ESP-HA-H&T\","
        "\"name\":\"" + "ESP-HA Temperature & Humidity Sensor " + Hostname + "\","
        "\"sw_version\":\"" _MY_VERSION_ "\""
        "}"
      "}");

  String humidity_topic = String("homeassistant/sensor/"+MqttUniqueId+"-humidity"+"/config");
  debug(String(String("sending the following discovery message to: ")+String(humidity_topic)).c_str());
  debug(humidity_discovery_message.c_str());  
  
  /*  if (mqtt_client.publish(humidity_topic, humidity_discovery_message.c_str(),true)) {
    debug("MQTT humidity discovery successful sent.");
    hasDiscoveryBeenSent = true;
    lastDiscovery.reset();
    mqttSentCounter++;
  } else {
    debug("MQTT humidity discovery FAILED to send.");
  }
*/
  String temperature_discovery_message=String(
      "{"
      "\"~\":\"" + String(MqttPrefix) +"/"+ "\","
      "\"name\":\"" + MqttHAName + " (temperature)" + "\","
      "\"stat_t\":\"" + "~sensor/temperature" + "\","
      "\"unit_of_meas\":\"" + "°C" + "\","      
      "\"dev_cla\":\"" + "temperature" + "\","
      "\"val_tpl\":\"" + "{{value|float|round(1)}}" + "\","
      "\"exp_aft\":" + "43200" + ","
      "\"frc_upd\":\"" + "false" + "\","
      "\"qos\":" + "0" + ","
      "\"uniq_id\":\"" + MqttUniqueId + "-temperature" + "\","
      "\"device\":{"
        "\"ids\":[\"" + MqttUniqueId + "\"],"
        "\"connections\":[[\"mac\",\"" + WiFi.macAddress() + "\"]],"
        "\"mf\":\"Ameer Gaafar\","
        "\"model\":\"ESP-HA-H&T\","
        "\"name\":\"" + "ESP-HA Temperature & Humidity Sensor " + Hostname + "\","
        "\"sw_version\":\"" _MY_VERSION_ "\""
        "}"
      "}");

  String temperature_topic = String("homeassistant/sensor/"+MqttUniqueId+"-temperature"+"/config");
  debug(String(String("sending the following discovery message to: ")+String(temperature_topic)).c_str());
  debug(temperature_discovery_message.c_str());        
/*  if (mqtt_client.publish(topic, temperature_discovery_message.c_str(),true)) {
    debug("MQTT temperature discovery successful sent.");
    hasDiscoveryBeenSent = true;
    lastDiscovery.reset();
    mqttSentCounter++;
  } else {
    debug("MQTT temperature discovery FAILED to send.");
  }
*/
}

void loop(void) {
  server.handleClient();  // Handle any web activity

  uint32_t now = millis();
  
  MDNS.update();
  
  // MQTT client connection management
  if (!mqtt_client.connected()) {
    if (wasConnected) {
      lastDisconnectedTime = now;
      wasConnected = false;
      mqttDisconnectCounter++;
    }
    // Reconnect if it's longer than kMqttReconnectTime since we last tried.
    if (now - lastReconnectAttempt > kMqttReconnectTime) {
      lastReconnectAttempt = now;
      debug("client mqtt not connected, trying to connect");
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
        wasConnected = true;
        if (boot) {
          debug(String(String(Hostname)+ " connected for the first time").c_str());
          boot = false;
        } else {
          debug(String(
              String(Hostname)+" (re)connected to MQTT. Lost connection about "
              + timeSince(lastConnectedTime)).c_str());
        }
        lastConnectedTime = now;
        debug("successful client mqtt connection");
      }
    }
  } else {
    // MQTT loop
    lastConnectedTime = now;
    mqtt_client.loop();
    // Publish sensor readings
    doBroadcast(&lastBroadcast, kBroadcastPeriodMs,false, false);
  }
  delay(100);
}

// Timer Class
TimerMs::TimerMs() { reset(); }
void TimerMs::reset() {start = millis();}
uint32_t TimerMs::elapsed() {uint32_t now = millis();if (start <= now) return now - start; else return UINT32_MAX - start + now;}
