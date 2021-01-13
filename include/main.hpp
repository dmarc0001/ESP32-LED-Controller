#pragma once

#include <ESPAsyncWebServer.h>
#include <Esp.h>
#include <WiFi.h>
#include <mDNS.h>
#include "ApiJSONServer.hpp"
#include "AsyncElegantOTA.h"
#include "LEDPrefs.hpp"
#include "LedContol.hpp"
#include "nvs_flash.h"

extern AsyncElegantOtaClass AsyncElegantOTA;      // der OTA Server
extern APISrv::ApiJSONServerClass ApiJSONServer;  //! mein REST Server

void initPrefs( LEDSrv::LEDPrefs &prefs );
void initPWM( LedControl::LedControlClass &ledControl, LEDSrv::LEDPrefs &prefs );
void initWiFi( LEDSrv::LEDPrefs &prefs );
void initMDNS( LEDSrv::LEDPrefs &prefs );
void initHttpServer( LEDSrv::LEDPrefs &prefs, AsyncWebServer &httpServer, LedControl::LedControlClass *ledControl );
