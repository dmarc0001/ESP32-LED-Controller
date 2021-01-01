#pragma once
#ifndef __PROJECT_DEFAULTS_HPP__
#define __PROJECT_DEFAULTS_HPP__
#include <Arduino.h>

namespace OTASrv
{
  /****************************************************************************
   * Konstanten für das Programm +
   ***************************************************************************/
  //
  // Seriennummer des Programmes
  //
  constexpr char serialStr[] = "20210101-0008";
  //
  // für RGB LED mit gemeinsamer Anode
  //
  constexpr bool rgbInverted = true;
  //
  // Zeit nach der bei WIFI-Verlust WiFi neu gestartet wird
  //
  constexpr unsigned long timeToResetWiFi = 1000 * 20;

  //
  // Version des Speichers bei Versionen, wenn änderungen an speicher
  // muss die Version erhöhr werden und im code reagiert werden
  //
  constexpr int prefsVersion = 1;
  //
  // Konfigurierbare Parameter
  //
  // constexpr char defaultSSID[] = "cryptoWLANGallileo";  //! SSID für WLAN
  // constexpr char defaultPassword[] = "sommerSonne92";   //! Passwort für WLAN
  constexpr char defaultSSID[] = "Controller";                 //! SSID für WLAN
  constexpr char defaultPassword[] = "serverSaturnRinge2610";  //! Passwort für WLAN
  constexpr char hostname[] = "OTA-Server";                    //! Eigener Hostname für MDNS
  constexpr char apiUser[] = "admin";                          //! Name des Users API Schnittstelle
  constexpr char apiPassword[] = "admin";                      //! Passwort Users API Schnittstelle
  constexpr char updateUser[] = "admin";                       //! name des OTA Berechtigten
  constexpr char updatePassword[] = "admin";                   //! Passwort des OTA Berechtigten
  //
  // die Schlüsselnamen für den Speicher
  //
  constexpr char wasInitKey[] = "wasInit";                // wurde schon initialisiert?
  constexpr char prefsVersionKey[] = "prefsVersion";      // welche Version der Einstellungen?
  constexpr char SSIDKey[] = "SSID";                      // an welchem WLAN anmelden?
  constexpr char passwordKey[] = "password";              // welches PW im WLAN
  constexpr char hostnameKey[] = "hostname";              // welcher Hostname für das nDNS
  constexpr char serialKey[] = "serial";                  // Seriennummer / version
  constexpr char apiUserKey[] = "apiUser";                // api user name
  constexpr char apiPasswordKey[] = "apiPassword";        // api passwort
  constexpr char updateUserKey[] = "updateUser";          // update user
  constexpr char updatePasswordKey[] = "updatePassword";  // update password
  constexpr char savedREDValueKey[] = "redValue";         // gespeicherter realer Wert für RED
  constexpr char savedBLUEValueKey[] = "blueValue";       // gespeicherter realer Wert für BLUE
  constexpr char savedGREENValueKey[] = "greenValue";     // gespeicherter realer Wert für GREEN
  constexpr char savedWHITEValueKey[] = "whiteValue";     // gespeicherter realer Wert für WEISS
  //
  // PWM/LED Ausgänge GPIO, nicht konfigurierbar
  //
  constexpr double PWM_LED_FREQ = 10000.0;      //! PWM Frequenz
  constexpr uint8_t PWM_RESOLUTION = 10;        //! PWM Resolution 8, 10, 12, 15 BITS
  constexpr uint32_t PWM_STEPS = 1024;          //! ATTENTION!! == 2^PWM_RESOLUTION
  constexpr uint8_t PWM_LED_RED = 27;           //! GPIO Rote LED
  constexpr uint8_t PWM_LED_CHANNEL_RED = 0;    //! channel für rot
  constexpr uint8_t PWM_LED_GREEN = 26;         //! GPIO Grüne LED
  constexpr uint8_t PWM_LED_CHANNEL_GREEN = 1;  //! Channel für grün
  constexpr uint8_t PWM_LED_BLUE = 25;          //! GPIO blaue LED
  constexpr uint8_t PWM_LED_CHANNEL_BLUE = 2;   //! Channel blaue LED
  constexpr uint8_t PWM_LED_WHITE = 33;         //! Weiße LED
  constexpr uint8_t PWM_LED_CHANNEL_WHITE = 3;  //! Channel weiße LED
  //
  // LED Ausgang GPIO für WLAN CONNECT
  //
  constexpr uint8_t LED_WLANOK = 2;  // "WLAN OK" LED
  //
  // RGB Mode, d.h. nur Rot, Grün, Blau nutzen Weiß AUS
  //
  constexpr bool RGB_MODE = false;

  //
  // Kommandos REST API
  //
  constexpr char CMD_SET_RGBW[] = "set_rgbw";
  constexpr char CMD_GET_RGBW[] = "get_rgbw";
  constexpr char CMD_GET_STANDBY[] = "get_standby";
  constexpr char CMD_SET_STANDBY[] = "set_standby";
  constexpr char CMD_STANDBY[] = "standby";
  constexpr char CMD_RGBW[] = "rgbw";

  constexpr char CMD_COLOR_RED[] = "red";
  constexpr char CMD_COLOR_GREEN[] = "green";
  constexpr char CMD_COLOR_BLUE[] = "blue";
  constexpr char CMD_COLOR_WHITE[] = "white";
  constexpr char CMD_TRUE[] = "true";
  constexpr char CMD_FALSE[] = "false";

}  // namespace OTASrv

namespace DebugData
{
  static const char webSiteText[] =
      "\
<html>\
<header>\
  <title>LED Controller</title>\
  <script>\
function init_page()\
{\
  var stb_button = document.getElementById('led_standby_button');\
  if(stb_button)\
  {\
    stb_button.onclick = onStandbyButtonClick;\
  }\
  stb_button = document.getElementById('led_operate_button');\
  if(stb_button)\
  {\
    stb_button.onclick = onOperateButtonClick;\
  }\
}\
  function onStandbyButtonClick()\
  {\
    setStandby(true);\
  }\
\
  function onOperateButtonClick()\
  {\
    setStandby(false);\
  }\
\
  function setStandby(is_stb)\
  {\
    var xhr = new XMLHttpRequest();\
    var url = '/rest/led';\
    var stb = 'true';\
    if(is_stb==false)\
    {\
      stb = 'false';\
    }\
    xhr.open('POST', url, true);\
    xhr.setRequestHeader('Content-Type', 'application/json');\
    xhr.onreadystatechange = function () {\
        if (xhr.readyState === 4 && xhr.status === 200) {\
            var json = JSON.parse(xhr.responseText);\
            console.log('standby:' + json.standby + ', error:' + json.error);\
        }\
    };\
    var data = JSON.stringify({'set_standby': stb});\
    console.debug(data);\
    xhr.send(data);\
  }\
  </script>\
</header>\
<body onload=\"init_page()\">\
  <div>\
    <h2>LED Config</h2>\
  </div>\
\
  <div>\
    Testbutton: <br />\
    <input class=\"clickbutton\" id=\"led_standby_button\" type=\"button\" value=\"LED STANDBY\" /><br />\
    <input class=\"clickbutton\" id=\"led_operate_button\" type=\"button\" value=\"LED OPERATE\" /><br />\
  </div>\
</body>\
\
</html>\
  ";

}  // namespace DebugData

#endif