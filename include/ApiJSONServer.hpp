#pragma once

#include "Arduino.h"
#include "stdlib_noniso.h"

#if defined( ESP8266 )
#include "ESP8266WiFi.h"
#include "ESPAsyncTCP.h"
#include "FS.h"
#include "flash_hal.h"
#elif defined( ESP32 )
#include "AsyncTCP.h"
#include "Update.h"
#include "WiFi.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#endif

#include <ArduinoJson.h>
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include "Hash.h"

namespace APISrv
{
  class ApiJSONServerClass
  {
    private:
    AsyncWebServer *_server;
    String _id = _getID();
    String _username = "";
    String _password = "";
    bool _authRequired = false;
    bool restartRequired = false;

    public:
    void setID( const char *id );
    String getID();

    void begin( AsyncWebServer *server, const char *username = "", const char *password = "" );

    private:
    String _getID();
    AsyncWebServerRequest *onGetIdentity( AsyncWebServerRequest *request );
    AsyncWebServerRequest *onGetValues( AsyncWebServerRequest *request );
    AsyncWebServerRequest *onSetValues( AsyncWebServerRequest *request );
  };

}  // namespace APISrv