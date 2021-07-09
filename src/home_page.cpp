#include <dashboard_pages.h>
#include <Dashboard.h>
#include <TimerUtils.h>
#include <utils.h>

const char ROOT_HTML_TEMPLATE[] PROGMEM = R"=====(
<!doctype html>
<html lang="en">
<head>
  <title>%TITLE%</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="Content-Type" content="text/html;charset=utf-8">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-+0n0xVW2eSR5OomGNYDnhzAbDsOXxcvSN1TPprVMTNDbiYZCxYbOOl7+AMvyTG2x" crossorigin="anonymous">
</head>
<body>
<div class="container py-3">
  <header>
    <div class="d-flex flex-column flex-md-row align-items-center pb-3 mb-4 border-bottom">
      <a href="/" class="d-flex align-items-center text-dark text-decoration-none">
        <svg xmlns="http://www.w3.org/2000/svg" width="40" height="32"><path d="M21.88,9.62H10.12a.5.5,0,0,0-.5.5V21.88a.5.5,0,0,0,.5.5h7.75a.47.47,0,0,0,.35-.15l4-4a.47.47,0,0,0,.15-.35V10.12A.5.5,0,0,0,21.88,9.62Zm-.5,8-3.72,3.72h-7V10.62H21.38ZM29,14.17a.5.5,0,0,0,.5-.5.5.5,0,0,0-.5-.5H25.5V9.5H29a.5.5,0,0,0,0-1H25.5V7a.5.5,0,0,0-.5-.5H23.5V3a.5.5,0,0,0-1,0V6.5H18.83V3a.5.5,0,0,0-.5-.5.5.5,0,0,0-.5.5V6.5H14.17V3a.5.5,0,0,0-.5-.5.5.5,0,0,0-.5.5V6.5H9.5V3a.5.5,0,0,0-1,0V6.5H7a.5.5,0,0,0-.5.5V8.5H3a.5.5,0,0,0,0,1H6.5v3.67H3a.5.5,0,0,0-.5.5.5.5,0,0,0,.5.5H6.5v3.66H3a.5.5,0,0,0-.5.5.5.5,0,0,0,.5.5H6.5V22.5H3a.5.5,0,0,0,0,1H6.5V25a.5.5,0,0,0,.5.5H8.5V29a.5.5,0,0,0,1,0V25.5h3.67V29a.5.5,0,0,0,.5.5.5.5,0,0,0,.5-.5V25.5h3.66V29a.5.5,0,0,0,.5.5.5.5,0,0,0,.5-.5V25.5H22.5V29a.5.5,0,0,0,1,0V25.5H25a.5.5,0,0,0,.5-.5V23.5H29a.5.5,0,0,0,0-1H25.5V18.83H29a.5.5,0,0,0,.5-.5.5.5,0,0,0-.5-.5H25.5V14.17ZM24.5"/></svg>
        <span class="fs-4">%TITLE%</span>
      </a>

      <nav class="d-inline-flex mt-2 mt-md-0 ms-md-auto">
        <a class="me-3 py-2 text-dark text-decoration-none" href="/sensors">Sensors</a>
        <a class="me-3 py-2 text-dark text-decoration-none" href="/utils">Utils</a>
      </nav>
    </div>
  </header>

  <main>
    <div class="p-3 pb-md-4 mx-auto text-left">
      <p class="fs-5 text-muted">ESP↺HA platform makes it easy to author and integrate ESP based IoT things into Home Assistant. The platform supports MQTT integration and discovery à la Home Assistant!</p>
    </div>

    <div class="w-75 mx-auto table-responsive">
      <h2 class="display-6 text-center mb-4">Platform</h2>

      <table class="table text-center">
        <tbody>
        <tr>
          <th scope="row" class="text-start">Product:</th>
          <td style="text-align:right">%PRODUCT_NAME%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Model:</th>
          <td style="text-align:right">%PRODUCT_MODEL%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Host Name:</th>
          <td style="text-align:right">%HOSTNAME%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">IP Address:</th>
          <td style="text-align:right">%IP_ADDRESS%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">MAC address:</th>
          <td style="text-align:right">%MAC_ADDRESS%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Gateway:</th>
          <td style="text-align:right">%GATEWAY%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">WiFi RSSI:</th>
          <td style="text-align:right">%RSSI%dBm</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">WiFi Auto Reconnect:</th>
          <td style="text-align:right">%WIFI_AUTORECONNECT%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">WiFi Disconnections:</th>
          <td style="text-align:right">%WIFI_DISCONNECTIONS%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Up Since:</th>
          <td style="text-align:right">%UPSINCE%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Last Reset Reason:</th>
          <td style="text-align:right">%LAST_RESET_REASON%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Version:</th>
          <td style="text-align:right">%SOFTWARE_VERSION%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Built:</th>
          <td style="text-align:right">%SOFTWARE_BUILD_DATE%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">ESP8266 Core Version:</th>
          <td style="text-align:right">%ESP_CODE_VERSION%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Free Sketch Space:</th>
          <td style="text-align:right">%FREE_SKETCH_SPACE%KB</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Free Heap Space:</th>
          <td style="text-align:right">%FREE_HEAP_SPACE%KB</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Max Free Block Size:</th>
          <td style="text-align:right">%MAX_FREE_BLOCK_SIZE%KB</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Heap Fragmentation Ratio:</th>
          <td style="text-align:right">%HEAP_FRAGMENTATION_ISSUE%&percnt;</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">CPU Frequency:</th>
          <td style="text-align:right">%CPU_FREQUENCY%MHz</td>
        </tr>
        </tbody>
      </table>
    </div>

    <div class="w-75 mx-auto table-responsive">
      <h2 style="padding-top: 1em;" class="display-6 text-center mb-4">MQTT Client</h2>
      <table class="table text-center">
        <tbody>
        <tr>
          <th scope="row" class="text-start">MQTT Server:</th>
          <td style="text-align:right">%MQTT_SERVER%<br /><i><small>(%MQTT_CONNECTED_SINCE%)</small></i></td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Disconnections:</th>
          <td style="text-align:right">%MQTT_DISCONNECTIONS%</td>
        </tr>

        <tr>
          <th scope="row" class="text-start">Client Id:</th>
          <td style="text-align:right">%MQTT_CLIENT_ID%</td>
        </tr>

        <tr>
          <th scope="row" class="text-start">LWT Topic:</th>
          <td style="text-align:right">%MQTT_LWT_TOPIC%</td>
        </tr>

        <tr>
          <th scope="row" class="text-start">QoS:</th>
          <td style="text-align:right">%MQTT_QOS%</td>
        </tr>

        <tr>
          <th scope="row" class="text-start">Published:</th>
          <td style="text-align:right">%MQTT_TOTAL_PUBLISHED%<br /><i><small>(%MQTT_ERRORS% erros)</small></i></td>
        </tr>

        <tr>
          <th scope="row" class="text-start">Log Topic:</th>
          <td style="text-align:right">%MQTT_LOG_TOPIC%</td>
        </tr>

        <tr>
          <th scope="row" class="text-start">Logged:</th>
          <td style="text-align:right">%MQTT_TOTAL_LOGGED%<br /><i><small>(%MQTT_LOG_ERRORS% erros)</small></i></td>
        </tr>

        </tbody>
      </table>
    </div>

  </main>

  <footer class="pt-4 my-md-5 pt-md-5 border-top">
    <div class="row text-center">
      <small>
        <i>This page refreshes every 60 seconds<br />
          Copyright © 2021 Ameer Gaafar</i>
      </small>
    </div>
  </footer>

</div>
<script type="text/javascript">
  function Redirect() {window.location="/";}
  setTimeout('Redirect()', 60000);
</script>
</body>
</html>
)=====";

static const keyProcessorEntry pageProcessors[] = {
        {"TITLE", [](const String& var){return Dashboard::instance()->title();}},
        {"PRODUCT_NAME",[](const String& var){ return Dashboard::instance()->product()+" By "+Dashboard::instance()->manufacturer();}},
        {"PRODUCT_MODEL",[](const String& var){ return Dashboard::instance()->model();}},
        {"HOSTNAME", [](const String& var){return Dashboard::instance()->hostname();}},
        {"IP_ADDRESS", [](const String& var){return MQTTClient::instance()->wifi()->localIP().toString();}},
        {"MAC_ADDRESS",[](const String& var){ return MQTTClient::instance()->wifi()->macAddress();}},
        {"GATEWAY",[](const String& var){ return MQTTClient::instance()->wifi()->gatewayIP().toString();}},
        {"RSSI",[](const String& var){ return String(MQTTClient::instance()->wifi()->RSSI());}},
        {"WIFI_AUTORECONNECT",[](const String& var){ return (MQTTClient::instance()->wifi()->getAutoReconnect() ? "Yes" : "No");}},
        {"WIFI_DISCONNECTIONS",[](const String& var){ return String(MQTTClient::instance()->wifiDisconnectCounter());}},
        {"UPSINCE",[](const String& var){ return timeSince(0);}},
        {"LAST_RESET_REASON",[](const String& var){ return ESP.getResetReason();}},
        {"SOFTWARE_VERSION",[](const String& var){ return Dashboard::instance()->swVersion();}},
        {"SOFTWARE_BUILD_DATE",[](const String& var){ return String(__DATE__)+"&nbsp;&nbsp;"+String(__TIME__);}},
        {"ESP_CODE_VERSION",[](const String& var){ return ESP.getCoreVersion();}},
        {"FREE_SKETCH_SPACE",[](const String& var){ return String(maxSketchSpace() >> 10);}},
        {"FREE_HEAP_SPACE",[](const String& var){ return String(ESP.getFreeHeap() >> 10);}},
        {"MAX_FREE_BLOCK_SIZE",[](const String& var){ return String(ESP.getMaxFreeBlockSize() >> 10);}},
        {"HEAP_FRAGMENTATION_ISSUE",[](const String& var){ return String(ESP.getHeapFragmentation());}},
        {"CPU_FREQUENCY",[](const String& var){ return String(ESP.getCpuFreqMHz());}},
        {"MQTT_SERVER",[](const String& var){ return MQTTClient::instance()->server()+":"+String(MQTTClient::instance()->port());}},
        {"MQTT_CONNECTED_SINCE",[](const String& var){ return (MQTTClient::instance()->isConnected() ? "Connected since:" + timeSince(MQTTClient::instance()->lastDisconnectedTime()): "Disconnected since:" + timeSince(MQTTClient::instance()->lastConnectedTime()));}},
        {"MQTT_DISCONNECTIONS",[](const String& var){ return String(MQTTClient::instance()->mqttDisconnectCounter());}},
        {"MQTT_CLIENT_ID",[](const String& var){ return String(MQTTClient::instance()->clientId());}},
        {"MQTT_LWT_TOPIC",[](const String& var){ return String(MQTTClient::instance()->lwtTopic());}},
        {"MQTT_QOS",[](const String& var){ return String(MQTTClient::instance()->qos());}},
        {"MQTT_TOTAL_PUBLISHED",[](const String& var){ return String(MQTTClient::instance()->mqttSentCounter());}},
        {"MQTT_ERRORS",[](const String& var){ return String(MQTTClient::instance()->mqttSendErrors());}},
        {"MQTT_LOG_TOPIC",[](const String& var){ return String(MQTTClient::instance()->logTopic());}},
        {"MQTT_TOTAL_LOGGED",[](const String& var){ return String(MQTTClient::instance()->mqttLogCounter());}},
        {"MQTT_LOG_ERRORS",[](const String& var){ return String(MQTTClient::instance()->mqttLogErrors());}}
};

String rootPageTemplateProcessor(const String& var){
    return substituteKey(var,pageProcessors,sizeof(pageProcessors) / sizeof(pageProcessors[0]));
}
