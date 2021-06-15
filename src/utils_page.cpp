#include <dashboard_pages.h>
#include <Dashboard.h>
#include <SensorControllers.h>

const char UTILS_HTML_TEMPLATE[] PROGMEM = R"=====(
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
        <a class="me-3 py-2 text-dark text-decoration-none" href="/sensors">Sensors</a>
      </nav>
    </div>
  </header>

  <main>
    <div class="p-3 pb-md-4 mx-auto text-left">
      <p class="fs-5 text-muted">ESP↺HA platform makes it easy to author and integrate ESP based IoT things into Home Assistant. The platform supports MQTT integration and discovery à la Home Assistant!</p>
    </div>

    <div class="card mb-4 rounded-3 shadow-sm">
      <div class="card-header py-3">
        <h5 class="my-0 fw-normal">Send MQTT Discovery Messages</h5>
      </div>
      <div class="card-body">
        <p class="card-text"> Send MQTT discovery messages to Home Assistant. A total of %SENSOR_COUNT% messages will be sent.</p>
        <form method='POST' action='/publish-discovery'>
          <div class="col-md-3 offset-md-9">
            <input type='submit' class="w-100 btn btn-lg btn-primary" value='Send Discovery'>
          </div>
        </form>
      </div>
    </div>

    <div class="card mb-4 rounded-3 shadow-sm">
      <div class="card-header py-3">
        <h5 class="my-0 fw-normal">Reboot Device</h5>
      </div>
      <div class="card-body">
        <p class="card-text">Perform warm reboot. No change to configuration or firmware.</p>
        <form method='POST' action='/reboot'>
          <div class="col-md-3 offset-md-9">
            <input type='submit' class="w-100 btn btn-lg btn-primary" value='Reboot'>
          </div>
        </form>
      </div>
    </div>

    <div class="card mb-4 rounded-3 shadow-sm">
      <div class="card-header py-3">
        <h5 class="my-0 fw-normal">Update Firmware</h5>
      </div>
      <div class="card-body">
        <p class="card-text">Update device firmware from an uploaded binary image. Can brick the device if interrupted or errored! Bricked devices will need to be programmed via serial/usb interface.</p>
        <form method='POST' action='/update' enctype='multipart/form-data'>
          <input type="file" class="w-75 form-control" name="update" />
        <div class="col-md-3 offset-md-9">
          <input type='submit' class="w-100 btn btn-lg btn-primary" value='Update Firmware'>
        </div>
        </form>
      </div>
    </div>
  </main>

  <footer class="pt-4 my-md-5 pt-md-5 border-top">
    <div class="row text-center" >
      <small>
        <i>Copyright © 2021 Ameer Gaafar</i>
      </small>
    </div>
  </footer>

</div>
</body>
</html>
)=====";

static const keyProcessorEntry pageProcessors[] = {
        {"TITLE", [](const String& var){return Dashboard::instance()->title();}},
        {"SENSOR_COUNT", [](const String& var){return String(ControllerGroup::instance()->count());}}
};

String utilsPageTemplateProcessor(const String& var){
    size_t aLen=sizeof(pageProcessors)/sizeof(pageProcessors[0]);
    for (size_t i=0;i< aLen;i++){
        if (var==pageProcessors[i].key){
            return pageProcessors[i].processorFunc(var);
        }
    }
    return var+"?";
}
