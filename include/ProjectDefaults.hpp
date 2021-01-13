#pragma once
#include <Arduino.h>

namespace OTASrv
{
  /****************************************************************************
   * Konstanten für das Programm +
   ***************************************************************************/
  //
  // für RGB LED mit gemeinsamer Anode
  //
  constexpr bool rgbInverted = true;
  //
  // Zeit nach der bei WIFI-Verlust WiFi neu gestartet wird
  //
  constexpr unsigned long timeToResetWiFi = 1000 * 10;

  //
  // Version des Speichers bei Versionen, wenn änderungen an speicher
  // muss die Version erhöhr werden und im code reagiert werden
  //
  constexpr int prefsVersion = 2;
  //
  // Konfigurierbare Parameter
  //
  // constexpr char defaultSSID[] = "cryptoWLANGallileo";  //! SSID für WLAN
  // constexpr char defaultPassword[] = "sommerSonne92";   //! Passwort für WLAN
  constexpr char defaultSSID[] = "Controller";                 //! SSID für WLAN
  constexpr char defaultPassword[] = "serverSaturnRinge2610";  //! Passwort für WLAN
  constexpr char hostname[] = "OTA-Server";                    //! Eigener Hostname für MDNS
  constexpr char updateUser[] = "admin";                       //! name des OTA Berechtigten
  constexpr char updatePassword[] = "admin";                   //! Passwort des OTA Berechtigten
  constexpr double PWM_LED_FREQ = 10000.0;                     //! PWM Frequenz default
  constexpr uint8_t PWM_RESOLUTION = 10;                       //! PWM Resolution 8, 10, 12, 15 BITS default
  //
  // die Schlüsselnamen für den Speicher
  // Einstelungen für den Controller
  //
  constexpr char wasInitKey[] = "wasInit";                //! wurde schon initialisiert?
  constexpr char prefsVersionKey[] = "prefsVersion";      //! welche Version der Einstellungen?
  constexpr char SSIDKey[] = "SSID";                      //! an welchem WLAN anmelden (konfigurierbar)?
  constexpr char wlanPasswordKey[] = "wlanPw";            //! welches PW im WLAN (konfigurierbar)
  constexpr char hostnameKey[] = "hostname";              //! welcher Hostname für das nDNS (konfigurierbar)
  constexpr char apiUserKey[] = "apiUser";                //! api user name (konfigurierbar, default null)
  constexpr char apiPasswordKey[] = "apiPassword";        //! api passwort (konfigurierbar, default null)
  constexpr char updateUserKey[] = "updateUser";          //! update user (konfigurierbar)
  constexpr char updatePasswordKey[] = "updatePassword";  //! update password (konfigurierbar)
  constexpr char savedREDValueKey[] = "redValue";         //! gespeicherter prozentualer Wert für RED als double
  constexpr char savedBLUEValueKey[] = "blueValue";       //! gespeicherter prozentualer Wert für BLUE als double
  constexpr char savedGREENValueKey[] = "greenValue";     //! gespeicherter prozentualer Wert für GREEN als double
  constexpr char savedWHITEValueKey[] = "whiteValue";     //! gespeicherter prozentualer Wert für WEISS als double
  constexpr char invertedLEDKey[] = "ledInvers";          //! LED invers (evtl wg. gemeinsamer Anode nötig)
  constexpr char pwmResolutionKey[] = "pwmRes";           //! PWM Auflösung (8, 10, 12, oder 15 Bit)
  constexpr char pwmFreqKey[] = "pwmFreq";                //! PWM Frequenz (konfigurierbar)

  //
  // PWM/LED Ausgänge GPIO, nicht konfigurierbar
  //
  constexpr uint8_t PWM_LED_RED = 27;           //! GPIO Rote LED
  constexpr uint8_t PWM_LED_CHANNEL_RED = 0;    //! channel für rot
  constexpr uint8_t PWM_LED_GREEN = 26;         //! GPIO Grüne LED
  constexpr uint8_t PWM_LED_CHANNEL_GREEN = 1;  //! Channel für grün
  constexpr uint8_t PWM_LED_BLUE = 25;          //! GPIO blaue LED
  constexpr uint8_t PWM_LED_CHANNEL_BLUE = 2;   //! Channel blaue LED
  constexpr uint8_t PWM_LED_WHITE = 33;         //! Weiße LED
  constexpr uint8_t PWM_LED_CHANNEL_WHITE = 3;  //! Channel weiße LED
  constexpr double PWM_MIN_FREQENCE = 400.0;    //! minimale erlaubtre Frequenz
  constexpr double PWM_MAX_FREQENCE = 50000.0;  //! maximale erlaubtre Frequenz

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

  constexpr char CMD_PWM_RESOLUTION[] = "pwm_res";
  constexpr char CMD_PWM_FREQUENCE[] = "pwm_freq";
  constexpr char CMD_PWM_INVERSE[] = "pwm_inv";

  constexpr char CMD_FW_USERID[] = "access_id";
  constexpr char CMD_FW_PASSWD[] = "access_pw";

  constexpr char CMD_API_USERID[] = "api_id";
  constexpr char CMD_API_PASSWD[] = "api_pw";

  constexpr char CMD_WLAN_SSID[] = "wlan_ssid";
  constexpr char CMD_WLAN_PASSWD[] = "wlan_pw";
  constexpr char CMD_WLAN_MDNSHOST[] = "mdns_host";

}  // namespace OTASrv
