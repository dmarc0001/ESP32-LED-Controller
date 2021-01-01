#include "main.hpp"

APISrv::ApiJSONServerClass ApiJSONServer;
AsyncElegantOtaClass AsyncElegantOTA;

/**
 * Initialisiere den NVM Speicher
 */
void initPrefs( OTASrv::OTAPrefs &prefs )
{
  Serial.println( "init non volatile store..." );
  esp_err_t ret = nvs_flash_init();
  if ( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND )
  {
    Serial.println( "erase store...." );
    ESP_ERROR_CHECK( nvs_flash_erase() );
    Serial.println( "init non volatile store again..." );
    ret = nvs_flash_init();
  }
  //
  // hier stoppt es, wenn es schief geht
  //
  ESP_ERROR_CHECK( ret );
  //
  // Storage starten
  //
  if ( prefs.begin( false ) )
  {
    Serial.println( "init non volatile store...OK" );
  }
}

/**
 * Initialisiere PWM für LED
 */
void initPWM( LedControl::LedControlClass &ledControl )
{
  Serial.println( "init PWM (LED Conrtrol)..." );
  ledControl.init();
  Serial.println( "init PWM (LED Control)...OK" );
}

/**
 * Initialisiere WLAN NEtzwerk
 */
void initWiFi( OTASrv::OTAPrefs &prefs )
{
  Serial.println( "init WIFI... " );
  WiFi.mode( WIFI_STA );
  Serial.print( "Connecting to <" );
  Serial.print( prefs.getSSID() );
  Serial.println( ">..." );
  WiFi.begin( prefs.getSSID().c_str(), prefs.getPassword().c_str() );
}

/**
 * initialisiere mDNS
 */
void initMDNS( OTASrv::OTAPrefs &prefs )
{
  Serial.println( "start mDNS Service..." );
  if ( ESP_OK == mdns_init() )
  {
    Serial.println( "start mDNS Service...OK" );
    Serial.print( "set mDNS hostname to <" );
    Serial.print( prefs.getHostname() );
    Serial.println( ">" );
    if ( ESP_OK == mdns_hostname_set( prefs.getHostname().c_str() ) )
    {
      Serial.println( "set mDNS hostname...OK" );
      mdns_service_add( nullptr, "_http", "_tcp", 80, nullptr, 0 );
    }
    else
    {
      Serial.println( "set Mdns hostname...ERROR" );
      mdns_free();
    }
  }
  else
  {
    Serial.println( "start mDNS Service...ERROR" );
    mdns_free();
  }
}

/**
 * initialisiere den Webserver
 */
AsyncElegantOtaClass *initHttpServer( OTASrv::OTAPrefs &prefs, AsyncWebServer &httpServer, LedControl::LedControlClass *ledControl )
{
  //
  // der root zugriff
  //
  httpServer.on( "/", HTTP_GET, []( AsyncWebServerRequest *request ) { request->send( 200, "text/plain", "Hi! I am ESP32." ); } );
  //
  // OTA Server
  //
  AsyncElegantOTA.begin( &httpServer, prefs.getUpdateUser().c_str(), prefs.getUpdatePassword().c_str() );
  //
  // API Server
  //
  ApiJSONServer.begin( &httpServer, prefs.getApiUser().c_str(), prefs.getApiPassword().c_str(), ledControl );
  //
  // Webserver starten
  //
  Serial.print( "http server is starting, serial: " );
  Serial.print( prefs.getSerialStr() );
  Serial.println( "..." );
  httpServer.begin();
  Serial.println( "HTTP httpServer started" );
  return ( &AsyncElegantOTA );
}

void demo_rgb( LedControl::LedControlClass &ledCtrl )
{
  static LedControl::LedStatusClass status;
  static bool count_down = false;
  //
  // demo der RGB Funktionen
  //
  if ( count_down )
  {
    //
    // Abwärts zählen
    //
    if ( status.red > 0.0 )
    {
      status.red -= 1.5;
    }
    else if ( status.white > 0.0 )
    {
      status.white -= 1.5;
    }
    else if ( status.green > 0.0 )
    {
      status.green -= 1.5;
    }
    else if ( status.blue > 0.0 )
    {
      status.blue -= 1.5;
    }
    else
    {
      count_down = false;
    }
  }
  else
  {
    //
    // aufwärts zählen
    //
    if ( status.red < 100.0 )
    {
      status.red += 1.0;
    }
    else if ( status.white < 100.0 )
    {
      status.white += 1.0;
    }
    else if ( status.green > 100.0 )
    {
      status.green += 1.0;
    }
    else if ( status.blue < 100.0 )
    {
      status.blue += 1.0;
    }
    else
    {
      count_down = true;
    }
  }
  /*
  Serial.print( "VALUES R:" );
  Serial.print( status.red );
  Serial.print( ", G:" );
  Serial.print( status.green );
  Serial.print( ", B:" );
  Serial.print( status.blue );
  Serial.print( ", W:" );
  Serial.print( status.white );
  Serial.println( "." );
  */
  ledCtrl.setPercentStatus( status );
}