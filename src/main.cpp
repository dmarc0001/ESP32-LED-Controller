#include "main.hpp"

OTASrv::OTAPrefs prefs;
AsyncWebServer httpServer( 80 );
LedControl::LedControlClass ledControl;
AsyncElegantOtaClass *otaServer;

void setup( void )
{
  initPWM( ledControl );
  //
  // für das Debugging
  //
  Serial.begin( 115200 );
  Serial.println( "" );
  Serial.println( "controller is starting..." );
  //
  // alles im Programm initialisieren
  //
  initPrefs( prefs );
  initWiFi( prefs );
  // signalisieren
  digitalWrite( OTASrv::LED_WLANOK, HIGH );
  initMDNS( prefs );
  otaServer = initHttpServer( prefs, httpServer );
}

void loop( void )
{
  //
  // warten auf neustart nach Flash
  //
  otaServer->loop();
}