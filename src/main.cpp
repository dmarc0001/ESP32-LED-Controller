#include "main.hpp"

OTASrv::OTAPrefs prefs;                  //! Preferenzen
AsyncWebServer httpServer( 80 );         //! der eigentliche Webserver Prozess
LedControl::LedControlClass ledControl;  //! Objekt zur Kontrolle der LED
AsyncElegantOtaClass *otaServer;         //! Zeiger auf den OTA Serverprozess für loop()
LedControl::LedStatusClass ledPrefs;     //! aktuelle Preferenzwerte für LED (in Prozenzen)

void setup( void )
{
  //
  // alles im Programm initialisieren
  //
  Serial.begin( 115200 );
  Serial.println( "" );
  Serial.println( "controller is starting..." );
  // Einstellungen
  initPrefs( prefs );
  initPWM( ledControl, prefs );
  //
  // für das Debugging
  //
  prefs.getLedStats( ledPrefs );
  ledControl.standBy( false );
  ledControl.setPercentStatus( ledPrefs );
  initWiFi( prefs );
  otaServer = initHttpServer( prefs, httpServer, &ledControl );
}

void loop( void )
{
  static volatile bool isOnline = false;
  static volatile unsigned long lastTimer = 0;
  //
  // warte immer etwa 2 Sekunden
  //
  if ( ( 0x07ffUL & millis() ) == 0 )
  {
    LedControl::LedStatusClass ledCurrent;
    ledControl.getPercentStatus( ledCurrent );
    if ( ledPrefs != ledCurrent )
    {
      //
      // Da war was geändert => Sichern
      //
      Serial.println( "changed LED preerences: save to store..." );
      prefs.setLedStats( ledCurrent );
      prefs.getLedStats( ledPrefs );
      Serial.println( "changed LED preerences: save to store...OK" );
    }
  }
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
      Serial.print( WiFi.localIP() );
      Serial.print( "         " );
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
        delay( 300 );
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
