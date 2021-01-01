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
#include "AsyncJson.h"
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include "Hash.h"
#include "LedContol.hpp"

namespace APISrv
{
  class ApiJSONServerClass
  {
    private:
    AsyncWebServer *_server;
    LedControl::LedControlClass *_ledControl;
    String _id = _getID();
    String _username = "";
    String _password = "";
    bool _authRequired = false;
    bool restartRequired = false;
    String cmd_true{ OTASrv::CMD_TRUE };
    String cmd_false{ OTASrv::CMD_FALSE };

    public:
    void setID( const char *id );
    String getID();

    void begin( AsyncWebServer *server,
                const char *username = "",
                const char *password = "",
                LedControl::LedControlClass *ledControl = nullptr );

    private:
    String _getID();
    AsyncWebServerRequest *onLedCommandPost( AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total );

    AsyncWebServerRequest *onGetIdentity( AsyncWebServerRequest *request );
    AsyncWebServerRequest *onGetValues( AsyncWebServerRequest *request );

    void setLedValues( const JsonObject &jobj );
    void getLedValues( JsonObject jobj );

    private:
    static const JsonString cmd_set_rgbw;
    static const JsonString cmd_get_rgbw;
    static const JsonString cmd_set_standby;
    static const JsonString cmd_get_standby;
    //
    static const JsonString cmd_standby;
    static const JsonString cmd_rgbw;
    //
    static const JsonString cmd_color_red;
    static const JsonString cmd_color_green;
    static const JsonString cmd_color_blue;
    static const JsonString cmd_color_white;
    // static const char *cmd_true;
    // static const char *cmd_false;
  };  // namespace APISrv

}  // namespace APISrv