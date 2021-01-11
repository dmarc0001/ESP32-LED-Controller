#pragma once

#include <ESPAsyncWebServer.h>
#include <Esp.h>
#include <WiFi.h>
#include <mDNS.h>
#include "ApiJSONServer.hpp"
#include "AsyncElegantOTA.h"
#include "LedContol.hpp"
#include "OTAPrefs.hpp"
#include "nvs_flash.h"

extern AsyncElegantOtaClass AsyncElegantOTA;      // der OTA Server
extern APISrv::ApiJSONServerClass ApiJSONServer;  //! mein REST Server

void initPrefs( OTASrv::OTAPrefs &prefs );
void initPWM( LedControl::LedControlClass &ledControl, OTASrv::OTAPrefs &prefs );
void initWiFi( OTASrv::OTAPrefs &prefs );
void initMDNS( OTASrv::OTAPrefs &prefs );
void initHttpServer( OTASrv::OTAPrefs &prefs, AsyncWebServer &httpServer, LedControl::LedControlClass *ledControl );
