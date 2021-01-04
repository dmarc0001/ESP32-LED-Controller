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

void initPrefs( OTASrv::OTAPrefs &prefs );
void initPWM( LedControl::LedControlClass &ledControl );
void initWiFi( OTASrv::OTAPrefs &prefs );
void initMDNS( OTASrv::OTAPrefs &prefs );
AsyncElegantOtaClass *initHttpServer( OTASrv::OTAPrefs &prefs, AsyncWebServer &httpServer, LedControl::LedControlClass *ledControl );
