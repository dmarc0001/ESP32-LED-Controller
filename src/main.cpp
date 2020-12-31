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
  otaServer = initHttpServer( prefs, httpServer );
}

void loop( void )
{
  static volatile bool isOnline = false;
  static volatile unsigned long lastTimer = 0;
  //
  // checke WiFi Status
  //
  if ( !isOnline )
  {
    //
    // flag für offline war gesetzt
    //
    if ( WiFi.status() == WL_CONNECTED )
    {
      //
      // WiFi is Online (wieder?)
      //
      Serial.print( "WiFi connected to <" );
      Serial.print( prefs.getSSID() );
      Serial.println( ">..." );
      Serial.print( "IP address: " );
      Serial.println( WiFi.localIP() );
      initMDNS( prefs );
      // otaServer = initHttpServer( prefs, httpServer );
      // signalisieren
      digitalWrite( OTASrv::LED_WLANOK, HIGH );
      isOnline = true;
      lastTimer = millis();
    }
    else
    {
      //
      // WiFi ist weiter offline
      // zähle Zeit zum WiFiReset runter
      //
      unsigned long timeDelta = millis() - lastTimer;
      if ( ( timeDelta % 1000 ) == 0 )
      {
        //
        // Jede Sekunde Meldung
        //
        Serial.print( "Disconnected: time to reset WiFi: " );
        Serial.print( int( ( OTASrv::timeToResetWiFi - timeDelta ) / 1000 ) );
        Serial.print( "   \r" );
      }
      if ( timeDelta >= OTASrv::timeToResetWiFi )
      {
        Serial.println( "\rWiFi reconnecting...               " );
        WiFi.reconnect();
        delay( 500 );
        lastTimer = millis();
      }
    }
  }
  else
  {
    //
    // Flag "gerät ist online" ist gesettz
    // Ist das nch so?
    //
    if ( WiFi.status() != WL_CONNECTED )
    {
      //
      // Status grändert!
      //
      // signalisieren
      digitalWrite( OTASrv::LED_WLANOK, LOW );
      isOnline = false;
    }
    else
    {
      //
      // den aktuellen zeitwert setzen
      //
      lastTimer = millis();
    }
  }
  //
  // warten auf neustart nach Flash
  //
  otaServer->loop();
}