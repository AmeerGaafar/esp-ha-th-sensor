#include <dashboard_pages.h>
#include <logging.h>
#include <Dashboard.h>
#include <SensorControllers.h>

const char SENSORS_HTML_TEMPLATE[] PROGMEM = R"=====(
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
        <a class="me-3 py-2 text-dark text-decoration-none" href="/">Home</a>
        <a class="me-3 py-2 text-dark text-decoration-none" href="/utils">Utils</a>
      </nav>
    </div>
  </header>

  <main>
    <div class="p-3 pb-md-4 mx-auto text-left">
      <p class="fs-5 text-muted">Below is latest reported state and statistics for all registered sensor in this %TITLE% device.</p>
    </div>

    <div class="w-75 mx-auto table-responsive">
      <h5 style="padding-top: 1em;" class="display-7 text-left mb-4">%SENSOR_1_NAME%</h5>
      <table class="table text-center">
        <tbody>
        <tr>
          <th scope="row" class="text-start">Id:</th>
          <td style="text-align:right">%SENSOR_1_ID%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">State:</th>
          <td style="text-align:right">%SENSOR_1_STATE%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">State Topic:</th>
          <td style="text-align:right">%SENSOR_1_STATE_TOPIC%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">State Published:</th>
          <td style="text-align:right">%SENSOR_1_STATE_PUBLISH_LATEST_TS%<br /><i><small>(%SENSOR_1_STATE_PUBLISH_COUNT% attempts & %SENSOR_1_STATE_PUBLISH_ERRS% errors)</small></i></td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Discovery Topic:</th>
          <td style="text-align:right">%SENSOR_1_DISCOVERY_TOPIC%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Discovery Published:</th>
          <td style="text-align:right">%SENSOR_1_DISCOVERY_PUBLISH_TS%<br /><i><small>(%SENSOR_1_DISCOVERY_PUBLISH_COUNT% attempts & %SENSOR_1_DISCOVERY_PUBLISH_ERRS% errors)</small></i></td>
        </tr>
        </tbody>
      </table>
    </div>

    <div class="w-75 mx-auto table-responsive">
      <h5 style="padding-top: 1em;" class="display-7 text-left mb-4">%SENSOR_2_NAME%</h5>
      <table class="table text-center">
        <tbody>
        <tr>
          <th scope="row" class="text-start">Id:</th>
          <td style="text-align:right">%SENSOR_2_ID%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">State:</th>
          <td style="text-align:right">%SENSOR_2_STATE%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">State Topic:</th>
          <td style="text-align:right">%SENSOR_2_STATE_TOPIC%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">State Published:</th>
          <td style="text-align:right">%SENSOR_2_STATE_PUBLISH_LATEST_TS%<br /><i><small>(%SENSOR_2_STATE_PUBLISH_COUNT% attempts & %SENSOR_2_STATE_PUBLISH_ERRS% errors)</small></i></td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Discovery Topic:</th>
          <td style="text-align:right">%SENSOR_2_DISCOVERY_TOPIC%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Discovery Published:</th>
          <td style="text-align:right">%SENSOR_2_DISCOVERY_PUBLISH_TS%<br /><i><small>(%SENSOR_2_DISCOVERY_PUBLISH_COUNT% attempts & %SENSOR_2_DISCOVERY_PUBLISH_ERRS% errors)</small></i></td>
        </tr>
        </tbody>
      </table>
    </div>

    <div class="w-75 mx-auto table-responsive">
      <h5 style="padding-top: 1em;" class="display-7 text-left mb-4">%SENSOR_3_NAME%</h5>
      <table class="table text-center">
        <tbody>
        <tr>
          <th scope="row" class="text-start">Id:</th>
          <td style="text-align:right">%SENSOR_3_ID%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">State:</th>
          <td style="text-align:right">%SENSOR_3_STATE%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">State Topic:</th>
          <td style="text-align:right">%SENSOR_3_STATE_TOPIC%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">State Published:</th>
          <td style="text-align:right">%SENSOR_3_STATE_PUBLISH_LATEST_TS%<br /><i><small>(%SENSOR_3_STATE_PUBLISH_COUNT% attempts & %SENSOR_3_STATE_PUBLISH_ERRS% errors)</small></i></td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Discovery Topic:</th>
          <td style="text-align:right">%SENSOR_3_DISCOVERY_TOPIC%</td>
        </tr>
        <tr>
          <th scope="row" class="text-start">Discovery Published:</th>
          <td style="text-align:right">%SENSOR_3_DISCOVERY_PUBLISH_TS%<br /><i><small>(%SENSOR_3_DISCOVERY_PUBLISH_COUNT% attempts & %SENSOR_3_DISCOVERY_PUBLISH_ERRS% errors)</small></i></td>
        </tr>
        </tbody>
      </table>
    </div>
  </main>

  <footer class="pt-4 my-md-5 pt-md-5 border-top">
    <div class="row" style="text-align: center">
      <small>
        <i>This page refreshes every 60 seconds<br />
          Copyright © 2021 Ameer Gaafar</i>
      </small>
    </div>
  </footer>

</div>
<script type="text/javascript">
  function Redirect() {window.location="/sensors";}
  setTimeout('Redirect()', 60000);
</script>
</body>
</html>
)=====";

const keyProcessorEntry pageProcessors[] = {
        {"TITLE", [](const String& var){return Dashboard::instance()->title();}},
        {"HOSTNAME", [](const String& var){return Dashboard::instance()->hostname();}},

        {"SENSOR_1_NAME",[](const String& var){return ControllerGroup::instance()->at(0)->name();}},
        {"SENSOR_1_ID",[](const String& var){return ControllerGroup::instance()->at(0)->id();}},
        {"SENSOR_1_STATE",[](const String& var){return ControllerGroup::instance()->at(0)->currentState();}},
        {"SENSOR_1_STATE_TOPIC",[](const String& var){return ControllerGroup::instance()->at(0)->stateTopic();}},
        {"SENSOR_1_STATE_PUBLISH_LATEST_TS",[](const String& var){return timeSinceOrNever(ControllerGroup::instance()->at(0)->stats().lastPublishedState);}},
        {"SENSOR_1_STATE_PUBLISH_COUNT",[](const String& var){return String(ControllerGroup::instance()->at(0)->stats().publishAttempts);}},
        {"SENSOR_1_STATE_PUBLISH_ERRS",[](const String& var){return String(ControllerGroup::instance()->at(0)->stats().publishErrors);}},
        {"SENSOR_1_DISCOVERY_TOPIC",[](const String& var){return ControllerGroup::instance()->at(0)->discoveryTopic();}},
        {"SENSOR_1_DISCOVERY_PUBLISH_TS",[](const String& var){return timeSinceOrNever(ControllerGroup::instance()->at(0)->stats().lastPublishedDiscovery);}},
        {"SENSOR_1_DISCOVERY_PUBLISH_COUNT",[](const String& var){return String(ControllerGroup::instance()->at(0)->stats().discoveryAttempts);}},
        {"SENSOR_1_DISCOVERY_PUBLISH_ERRS",[](const String& var){return String(ControllerGroup::instance()->at(0)->stats().discoveryErrors);}},

        {"SENSOR_2_NAME",[](const String& var){return ControllerGroup::instance()->at(1)->name();}},
        {"SENSOR_2_ID",[](const String& var){return ControllerGroup::instance()->at(1)->id();}},
        {"SENSOR_2_STATE",[](const String& var){return ControllerGroup::instance()->at(1)->currentState();}},
        {"SENSOR_2_STATE_TOPIC",[](const String& var){return ControllerGroup::instance()->at(1)->stateTopic();}},
        {"SENSOR_2_STATE_PUBLISH_LATEST_TS",[](const String& var){return timeSinceOrNever(ControllerGroup::instance()->at(1)->stats().lastPublishedState);}},
        {"SENSOR_2_STATE_PUBLISH_COUNT",[](const String& var){return String(ControllerGroup::instance()->at(1)->stats().publishAttempts);}},
        {"SENSOR_2_STATE_PUBLISH_ERRS",[](const String& var){return String(ControllerGroup::instance()->at(1)->stats().publishErrors);}},
        {"SENSOR_2_DISCOVERY_TOPIC",[](const String& var){return ControllerGroup::instance()->at(1)->discoveryTopic();}},
        {"SENSOR_2_DISCOVERY_PUBLISH_TS",[](const String& var){return timeSinceOrNever(ControllerGroup::instance()->at(1)->stats().lastPublishedDiscovery);}},
        {"SENSOR_2_DISCOVERY_PUBLISH_COUNT",[](const String& var){return String(ControllerGroup::instance()->at(1)->stats().discoveryAttempts);}},
        {"SENSOR_2_DISCOVERY_PUBLISH_ERRS",[](const String& var){return String(ControllerGroup::instance()->at(1)->stats().discoveryErrors);}},

        {"SENSOR_3_NAME",[](const String& var){return ControllerGroup::instance()->at(2)->name();}},
        {"SENSOR_3_ID",[](const String& var){return ControllerGroup::instance()->at(2)->id();}},
        {"SENSOR_3_STATE",[](const String& var){return ControllerGroup::instance()->at(2)->currentState();}},
        {"SENSOR_3_STATE_TOPIC",[](const String& var){return ControllerGroup::instance()->at(2)->stateTopic();}},
        {"SENSOR_3_STATE_PUBLISH_LATEST_TS",[](const String& var){return timeSinceOrNever(ControllerGroup::instance()->at(2)->stats().lastPublishedState);}},
        {"SENSOR_3_STATE_PUBLISH_COUNT",[](const String& var){return String(ControllerGroup::instance()->at(2)->stats().publishAttempts);}},
        {"SENSOR_3_STATE_PUBLISH_ERRS",[](const String& var){return String(ControllerGroup::instance()->at(2)->stats().publishErrors);}},
        {"SENSOR_3_DISCOVERY_TOPIC",[](const String& var){return ControllerGroup::instance()->at(2)->discoveryTopic();}},
        {"SENSOR_3_DISCOVERY_PUBLISH_TS",[](const String& var){return timeSinceOrNever(ControllerGroup::instance()->at(2)->stats().lastPublishedDiscovery);}},
        {"SENSOR_3_DISCOVERY_PUBLISH_COUNT",[](const String& var){return String(ControllerGroup::instance()->at(2)->stats().discoveryAttempts);}},
        {"SENSOR_3_DISCOVERY_PUBLISH_ERRS",[](const String& var){return String(ControllerGroup::instance()->at(2)->stats().discoveryErrors);}},
};

String sensorsPageTemplateProcessor(const String& var){
    return substituteKey(var,pageProcessors,sizeof(pageProcessors) / sizeof(pageProcessors[0]));
}
