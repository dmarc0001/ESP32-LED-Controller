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
  constexpr char serialStr[] = "20201228-0027";

  //
  // PWM Ausgänge
  //
  //constexpr 

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