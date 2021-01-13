#include "main.hpp"

LEDSrv::LEDPrefs prefs;                  //! Preferenzen
AsyncWebServer httpServer( 80 );         //! der eigentliche Webserver Prozess
LedControl::LedControlClass ledControl;  //! Objekt zur Kontrolle der LED
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
  initHttpServer( prefs, httpServer, &ledControl );
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
      Serial.println( "         " );
      initMDNS( prefs );
      // otaServer = initHttpServer( prefs, httpServer );
      // signalisieren
      digitalWrite( LEDSrv::LED_WLANOK, HIGH );
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
        Serial.print( int( ( LEDSrv::timeToResetWiFi - timeDelta ) / 1000 ) );
        Serial.print( "   \r" );
      }
      if ( timeDelta >= LEDSrv::timeToResetWiFi )
      {
        Serial.println( "\rWiFi reconnecting...               " );
        Serial.print( "Reconnect to: <" );
        Serial.print( prefs.getSSID() );
        Serial.println( ">..." );
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
      digitalWrite( LEDSrv::LED_WLANOK, LOW );
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
  AsyncElegantOTA.loop();
  ApiJSONServer.loop();
}
