#include "main.hpp"

OTASrv::OTAPrefs prefs;
AsyncWebServer httpServer( 80 );
LedControl::LedControlClass ledControl;
AsyncElegantOtaClass *otaServer;

void setup( void )
{
  ledControl.standBy();
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
  initPWM( prefs, ledControl );
  initWiFi( prefs );
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