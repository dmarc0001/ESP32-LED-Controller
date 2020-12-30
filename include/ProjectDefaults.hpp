#pragma once

namespace OTASrv
{
  // constexpr char* defaultSSID{"campus"};
  // constexpr char* defaultPassword{"geheim.."};
  constexpr int prefsVersion = 1;
  constexpr char defaultSSID[] = "Controller";
  constexpr char defaultPassword[] = "serverSaturnRinge2610";
  constexpr char hostname[] = "OTA-Server";
  constexpr char apiUser[] = "admin";
  constexpr char apiPassword[] = "admin";
  constexpr char updateUser[] = "admin";
  constexpr char updatePassword[] = "admin";
  //
  // Seriennummer des Programmes
  //
  constexpr char serialStr[] = "20201230-0001";

  //
  // PWM/LED Ausgänge
  //
  constexpr double PWM_LED_FREQ = 10000.0;
  constexpr uint8_t PWM_RESOLUTION = 12;  // Resolution 8, 10, 12, 15 BITS
  constexpr uint8_t PWM_LED_RED = 27;     // RET LED
  constexpr uint8_t PWM_LED_CHANNEL_RED = 0;
  constexpr uint8_t PWM_LED_GREEN = 26;  // GREEN LED
  constexpr uint8_t PWM_LED_CHANNEL_GREEN = 1;
  constexpr uint8_t PWM_LED_BLUE = 25;  // BLUE LED
  constexpr uint8_t PWM_LED_CHANNEL_BLUE = 3;
  constexpr uint8_t PWM_LED_WHITE = 33;  // WHITE LED
  constexpr uint8_t PWM_LED_CHANNEL_WHITE = 4;
  //
  // LED Ausgang für WLAN CONNECT
  //
  constexpr uint8_t LED_WLANOK = 2;  // WLAN OK

  //
  // die Schlüssel
  //

  // wurde schon initialisiert?
  constexpr char wasInitKey[] = "wasInit";
  // welche Version der Einstellungen?
  constexpr char prefsVersionKey[] = "prefsVersion";
  // an welchem WLAN anmelden?
  constexpr char SSIDKey[] = "SSID";
  // welches PW im WLAN
  constexpr char passwordKey[] = "password";
  // welcher Hostname für das nDNS
  constexpr char hostnameKey[] = "hostname";
  // Seriennummer / version
  constexpr char serialKey[] = "serial";
  // api user name
  constexpr char apiUserKey[] = "apiUser";
  // api passwort
  constexpr char apiPasswordKey[] = "apiPassword";
  // update user
  constexpr char updateUserKey[] = "updateUser";
  // update password
  constexpr char updatePasswordKey[] = "updatePassword";

}  // namespace OTASrv