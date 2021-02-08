#pragma once

#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <Esp.h>
#include <WiFi.h>
#include <mDNS.h>
#include "ApiJSONServer.hpp"
#include "LEDPrefs.hpp"
#include "LedContol.hpp"
#include "ModifiedOTA.hpp"
#include "nvs_flash.h"

extern APISrv::ApiJSONServerClass ApiJSONServer;  //! mein REST Server

bool checkReset();
void initPrefs( LEDSrv::LEDPrefs &prefs );
void initPWM( LedControl::LedControlClass &ledControl, LEDSrv::LEDPrefs &prefs );
void initWiFi( LEDSrv::LEDPrefs &prefs );
void initWiFiAp( LEDSrv::LEDPrefs &prefs, String ssid, String pw );
void initMDNS( LEDSrv::LEDPrefs &prefs );
void initHttpServer( LEDSrv::LEDPrefs &prefs, AsyncWebServer &httpServer, LedControl::LedControlClass *ledControl );
void initOTAServer( LEDSrv::LEDPrefs &prefs );
