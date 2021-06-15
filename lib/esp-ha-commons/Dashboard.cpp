#include <Dashboard.h>
#include <TimerUtils.h>
#include <logging.h>
#include <utils.h>
#include <ota.h>
static Dashboard* dashboardSingletonInstance = nullptr;

Dashboard* Dashboard::instance() {
    return dashboardSingletonInstance;
}

RoutingTable::RoutingTable(const String &username, const String &password) {
    _username=username;
    _password=password;
    _count=0;
}

void RoutingTable::reset(){
    _count=0;
}

bool RoutingTable::add(const String &url, const char *htmlTemplate, const templateProcessingFunc &processor,
                       bool authenticate) {
    if (_count>=MaxRoutes-1) return false;
    _routes[_count]=url;
    _templates[_count]=htmlTemplate;
    _processors[_count]=processor;
    _authenticate[_count]=authenticate;
    _count++;
    return true;
}

int RoutingTable::entryOf(const String &url){
    for (int i=0;i<_count;i++){
        if (_routes[i]==url) return i;
    }
    return -1;
}

const char* RoutingTable::templateOf(int entryId){
    return _templates[entryId];
}

templateProcessingFunc RoutingTable::processorOf(int entryId){
    return _processors[entryId];
}

bool RoutingTable::authenticateOf(int entryId){
    return _authenticate[entryId];
}

Dashboard * Dashboard::create(const String &title, const String &hostname, const String &httpUserName,
                              const String &httpPassword,
                              const String &swVersion, const char *messageHtmlTemplate) {
    if (!dashboardSingletonInstance) {
        dashboardSingletonInstance = new Dashboard(title, hostname, httpUserName, httpPassword, swVersion,messageHtmlTemplate);
        return dashboardSingletonInstance;
    }
    else {
        return dashboardSingletonInstance;
    }
}

Dashboard::Dashboard(const String &title, const String &hostname, const String &httpUserName,
                     const String &httpPassword, const String &swVersion, const char *messageHtmlTemplate) {
    _routingTable=new RoutingTable(httpUserName,httpPassword);
    _server=new AsyncWebServer(HTTP_PORT);
    _title=title;
    _hostname=hostname;
    _swVersion=swVersion;
    _messageHtmlTemplate=messageHtmlTemplate;
    register404Handler();
    registerRebootRoute();
    registerOTARoute();
}

void Dashboard::begin() {
    _server->begin();
    LOG_INFO("HTTP server started")
}

void Dashboard::registerPostHandler(const String &url, const char *htmlTemplate, const templateProcessingFunc &processor){
    _routingTable->add(url,htmlTemplate,processor,true);
    _server->on(url.c_str(), HTTP_POST , [this](AsyncWebServerRequest *request){return this->requestRouter(request);});
}

void Dashboard::registerGetHandler(const String &url, const char* htmlTemplate, const templateProcessingFunc &processor){
    _routingTable->add(url,htmlTemplate,processor,true);
    _server->on(url.c_str(), HTTP_GET , [this](AsyncWebServerRequest *request){return this->requestRouter(request);});
}

void Dashboard::loop() const{
    if (_reboot) restart();
}

void Dashboard::requestReboot(){
    _reboot=true;
}

void Dashboard::register404Handler() {
    _server->onNotFound([](AsyncWebServerRequest *request){
        request->send(404);
    });
}

void Dashboard::registerOTARoute() {
    _server->on("/update", HTTP_POST,
                [](AsyncWebServerRequest *request) {},
                [this](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,
                   size_t len, bool final) {
                    if (!request->authenticate(routingTable()->username().c_str(), routingTable()->password().c_str())) {
                        return request->requestAuthentication();
                    }
                    handleFirmwareUpdate(request, filename, index, data, len, final);
                }
    );
}

void Dashboard::registerRebootRoute() {
    _server->on("/reboot",
                [this](AsyncWebServerRequest *request) {
                    if (!request->authenticate(routingTable()->username().c_str(), routingTable()->password().c_str())) {
                        return request->requestAuthentication();
                    }
                    return request->send_P(200, "text/html", _messageHtmlTemplate, [this](const String& var){
                        if (var=="TITLE") return title();
                        if (var=="HOSTNAME") return hostname();
                        if (var=="MESSAGE") {
                            requestReboot();
                            return String(title() + " device <i>" + hostname() + "</i> is restarting. You will be redirected to the home page in a few seconds...");
                        }
                        return String(var+"?");
                    });
                }
    );
}

void Dashboard::requestRouter(AsyncWebServerRequest *request){
    LOG_INFO("entering handler func: %s",request->url().c_str())

    int entryId=this->routingTable()->entryOf(request->url());
    if (entryId==-1) return request->send(404,"text/html", "<h1>Not Found </h1>");

    if (this->routingTable()->authenticateOf(entryId)) {
        if (!request->authenticate(this->routingTable()->username().c_str(),this->routingTable()->password().c_str())) {
            LOG_ERROR("leaving handler func after false authenticate: %s",request->url().c_str())
            return request->requestAuthentication();
        }
    }

    request->send_P(200, "text/html", this->routingTable()->templateOf(entryId),[entryId,this](const String& var){return this->routingTable()->processorOf(entryId)(var);});

    LOG_INFO("leaving handler func: %s",request->url().c_str())
}

