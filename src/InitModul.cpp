#include "indexPage.hpp"
#include "main.hpp"

AsyncElegantOtaClass AsyncElegantOTA;      // der OTA Server
APISrv::ApiJSONServerClass ApiJSONServer;  //! mein REST Server

bool checkReset()
{
  int val = digitalRead( LEDSrv::RESET_SWITCH_PIN );
  if ( val == 0 )
    return ( true );
  return ( false );
}

/**
 * Initialisiere den NVM Speicher
 */
void initPrefs( LEDSrv::LEDPrefs &prefs )
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
void initPWM( LedControl::LedControlClass &ledControl, LEDSrv::LEDPrefs &prefs )
{
  Serial.println( "init PWM (LED Control)..." );
  if ( prefs.getIsLEDInvers() )
  {
    Serial.println( "PWM signal is inverted..." );
  }
  ledControl.init( prefs );
  Serial.println( "init PWM (LED Control)...OK" );
}

/**
 * Initialisiere WLAN NEtzwerk
 */
void initWiFi( LEDSrv::LEDPrefs &prefs )
{
  Serial.println( "init WIFI... " );
  WiFi.mode( WIFI_STA );
  Serial.print( "Connecting to <" );
  Serial.print( prefs.getSSID() );
  Serial.print( "> Secret: <" );
  Serial.print( prefs.getPassword() );
  Serial.println( ">..." );
  WiFi.begin( prefs.getSSID().c_str(), prefs.getPassword().c_str() );
  prefs.setApIsRunning( false );
}

/**
 * init as accesspoint
 */
void initWiFiAp( LEDSrv::LEDPrefs &prefs, String ssid, String pw )
{
  Serial.println( "init WIFI as access point... " );
  WiFi.mode( WIFI_AP );
  IPAddress _loc;
  _loc.fromString( "192.168.0.1" );
  IPAddress _gw;
  _gw.fromString( "255.255.255.0" );
  WiFi.softAPConfig( _loc, _loc, _gw );
  WiFi.softAP( ssid.c_str(), pw.c_str(), 1, 0, 2 );
  IPAddress IP = WiFi.softAPIP();
  Serial.print( "AP IP address: <" );
  Serial.print( IP );
  Serial.println( ">..." );
  prefs.setApIsRunning( true );
}

/**
 * initialisiere mDNS
 */
void initMDNS( LEDSrv::LEDPrefs &prefs )
{
  Serial.println( "start mDNS Service..." );
  mdns_free();
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
void initHttpServer( LEDSrv::LEDPrefs &prefs, AsyncWebServer &httpServer, LedControl::LedControlClass *ledControl )
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
}
