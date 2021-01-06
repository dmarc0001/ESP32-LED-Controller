#include "indexPage.hpp"
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
void initPWM( LedControl::LedControlClass &ledControl, OTASrv::OTAPrefs &prefs )
{
  Serial.println( "init PWM (LED Conrtrol)..." );
  ledControl.init( prefs );
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
  httpServer.on( "/", HTTP_GET, []( AsyncWebServerRequest *request ) {
    AsyncWebServerResponse *response =
        request->beginResponse_P( 200, "text/html", APISrv::INDEX_PAGE_CONTENT, APISrv::INDEX_PAGE_SIZE );
    response->addHeader( "Content-Encoding", "gzip" );
    request->send( response );
  } );

  // httpServer.on( "/", HTTP_GET, []( AsyncWebServerRequest *request ) { request->send( 200, "text/plain", "Hi! I am ESP32." ); } );

  //
  // OTA Server
  //
  AsyncElegantOTA.begin( &httpServer, prefs.getUpdateUser().c_str(), prefs.getUpdatePassword().c_str() );
  //
  // API Server
  //
  ApiJSONServer.begin( &httpServer, prefs, ledControl );
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
