#include <dashboard_pages.h>
#include <logging.h>
#include <Dashboard.h>

static void sendDiscovery(){
    LOG_INFO("Requesting Controllers to publishing Discovery messages...")
    ControllerGroup::instance()->requestDiscovery();
}

String sendDiscoveryTemplateProcessor(const String& var){

    if (var=="TITLE") return Dashboard::instance()->title();
    if (var=="HOSTNAME") return Dashboard::instance()->hostname();
    if (var=="MESSAGE") {
        sendDiscovery();
        char buffer[1024];
        snprintf(buffer,1024,"%s device <i>%s</i> sending discovery messages for %d sensors.<br />You will be redirected to the home page in a few seconds...",
                 Dashboard::instance()->title().c_str(),Dashboard::instance()->hostname().c_str(),ControllerGroup::instance()->count());
        return String(buffer);
    }
    return var+"?";
}

