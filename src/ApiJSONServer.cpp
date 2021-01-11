#include "ApiJSONServer.hpp"
#include "configPage.hpp"

namespace APISrv
{
  //
  // Konstenate Strings initialisieren
  //
  const JsonString ApiJSONServerClass::cmd_set_rgbw{ OTASrv::CMD_SET_RGBW };
  const JsonString ApiJSONServerClass::cmd_get_rgbw{ OTASrv::CMD_GET_RGBW };
  const JsonString ApiJSONServerClass::cmd_set_standby{ OTASrv::CMD_SET_STANDBY };
  const JsonString ApiJSONServerClass::cmd_get_standby{ OTASrv::CMD_GET_STANDBY };
  //
  const JsonString ApiJSONServerClass::cmd_rgbw{ OTASrv::CMD_RGBW };
  const JsonString ApiJSONServerClass::cmd_standby{ OTASrv::CMD_STANDBY };
  //
  const JsonString ApiJSONServerClass::cmd_color_red{ OTASrv::CMD_COLOR_RED };
  const JsonString ApiJSONServerClass::cmd_color_green{ OTASrv::CMD_COLOR_GREEN };
  const JsonString ApiJSONServerClass::cmd_color_blue{ OTASrv::CMD_COLOR_BLUE };
  const JsonString ApiJSONServerClass::cmd_color_white{ OTASrv::CMD_COLOR_WHITE };
  //
  const JsonString ApiJSONServerClass::cmd_pwm_resolution{ OTASrv::CMD_PWM_RESOLUTION };
  const JsonString ApiJSONServerClass::cmd_pwm_frequence{ OTASrv::CMD_PWM_FREQUENCE };
  const JsonString ApiJSONServerClass::cmd_pwm_is_inverse{ OTASrv::CMD_PWM_INVERSE };
  //
  //
  void ApiJSONServerClass::begin( AsyncWebServer *server, OTASrv::OTAPrefs &prefs, LedControl::LedControlClass *ledControl )
  {
    _server = server;
    _ledControl = ledControl;
    _prefs = &prefs;

    if ( prefs.getApiUser().isEmpty() )
    {
      _apiAuthRequired = false;
      _apiUserName = "";
      _apiPassword = "";
    }
    else
    {
      _apiAuthRequired = true;
      _apiUserName = prefs.getApiUser().c_str();
      _apiPassword = prefs.getApiPassword().c_str();
    }

    if ( prefs.getUpdateUser().isEmpty() )
    {
      _configAuthRequired = false;
      _configUserName = "";
      _configPassword = "";
    }
    else
    {
      _configAuthRequired = true;
      _configUserName = prefs.getUpdateUser().c_str();
      _configPassword = prefs.getUpdatePassword().c_str();
    }
    //
    // Wer bist Du?
    //
    _server->on( "/rest/identity", HTTP_GET, [ & ]( AsyncWebServerRequest *request ) { onGetIdentity( request ); } );
    //
    // Controller config
    //
    _server->on( "/rest/config", HTTP_GET, [ & ]( AsyncWebServerRequest *request ) {
      AsyncWebServerResponse *response =
          request->beginResponse_P( 200, "text/html", APISrv::CONFIG_PAGE_CONTENT, APISrv::CONFIG_PAGE_SIZE );
      if ( _configAuthRequired )
      {
        if ( !request->authenticate( _configUserName.c_str(), _configPassword.c_str() ) )
        {
          return request->requestAuthentication();
        }
      }
      response->addHeader( "Content-Encoding", "gzip" );
      request->send( response );
    } );

    _server->on(
        "/rest/pwm", HTTP_POST,
        []( AsyncWebServerRequest *request ) {
          // nothing and dont remove it
        },
        NULL,
        [ this ]( AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total ) {
          return ( onPwmCommandPost( request, data, len, index, total ) );
        } );

    //
    // LED Statusabfrage
    //
    _server->on( "/rest/led", HTTP_GET, [ & ]( AsyncWebServerRequest *request ) { onGetValues( request ); } );
    //
    // Handler für LED Command (JSON POST)
    //
    _server->on(
        "/rest/led", HTTP_POST,
        []( AsyncWebServerRequest *request ) {
          // nothing and dont remove it
        },
        NULL,
        [ this ]( AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total ) {
          return ( onLedCommandPost( request, data, len, index, total ) );
        } );
  }
  AsyncWebServerRequest *ApiJSONServerClass::onPwmCommandPost( AsyncWebServerRequest *request,
                                                               uint8_t *data,
                                                               size_t len,
                                                               size_t index,
                                                               size_t total )
  {
    DynamicJsonDocument doc( 1024 );
    DynamicJsonDocument answerDoc( 1024 );
    JsonObject answer = answerDoc.to< JsonObject >();
    DeserializationError err = deserializeJson( doc, ( const char * ) data );
    String content;
    bool have_to_boot = false;
    if ( err )
    {
      // das ging schief
      answer[ "error" ] = err.c_str();
      serializeJson( doc, content );
      Serial.println( content );
      request->send( 404, "application/json", content );
      return request;
    }
    String result;
    JsonObject jobj = doc.as< JsonObject >();
    //
    // Auflösung?
    //
    if ( jobj.containsKey( cmd_pwm_resolution ) )
    {
      uint8_t pwm_res = jobj[ cmd_pwm_resolution ].as< uint8_t >();
      if ( pwm_res >= 8 )
      {
        Serial.print( "pwm cmd: <" );
        Serial.print( cmd_pwm_resolution.c_str() );
        Serial.print( "> : " );
        Serial.println( pwm_res );
        if ( _prefs->getPwmResolution() != pwm_res )
        {
          //
          // geändert, das Gerät muss neu gestartet werden
          //
          have_to_boot = true;
          _prefs->setPwmResolution( pwm_res );
        }
        answer[ cmd_pwm_resolution ] = "OK";
      }
    }
    //
    // PWM Frequenz
    //
    if ( jobj.containsKey( cmd_pwm_frequence ) )
    {
      String pwmValStr = jobj[ cmd_pwm_frequence ].as< String >();
      pwmValStr.trim();
      if ( 0 != pwmValStr.compareTo( "null" ) )
      {
        double pwm_freq = jobj[ cmd_pwm_frequence ].as< double >();
        //
        if ( pwm_freq >= OTASrv::PWM_MIN_FREQENCE && pwm_freq <= OTASrv::PWM_MAX_FREQENCE )
        {
          Serial.print( "pwm cmd: <" );
          Serial.print( cmd_pwm_frequence.c_str() );
          Serial.print( "> : " );
          Serial.println( pwm_freq );
          if ( _prefs->getPwmFreq() != pwm_freq )
          {
            //
            // geändert, das Gerät muss neu gestartet werden
            //
            have_to_boot = true;
            _prefs->setPwmFreq( pwm_freq );
          }
          answer[ cmd_pwm_frequence ] = "OK";
        }
      }
      else
      {
        answer[ "status" ] = "frequency too high!";
      }
    }
    //
    // LED's invertiert?
    //
    if ( jobj.containsKey( cmd_pwm_is_inverse ) )
    {
      String pwmValStr = jobj[ cmd_pwm_is_inverse ].as< String >();
      pwmValStr.trim();
      if ( 0 != pwmValStr.compareTo( "null" ) )
      {
        bool pwm_inv = jobj[ cmd_pwm_is_inverse ].as< bool >();
        Serial.print( "pwm cmd: <" );
        Serial.print( cmd_pwm_is_inverse.c_str() );
        Serial.print( "> : " );
        Serial.println( pwm_inv );
        //
        if ( _prefs->getIsLEDInvers() != pwm_inv )
        {
          Serial.print( "pwm cmd: <" );
          Serial.print( cmd_pwm_is_inverse.c_str() );
          Serial.print( "> : " );
          Serial.println( pwm_inv );
          //
          // geändert, das Gerät muss neu gestartet werden
          //
          have_to_boot = true;
          _prefs->setIsLEDInvers( pwm_inv );
        }

        answer[ cmd_pwm_is_inverse ] = "OK";
      }
    }
    //
    // Generelle Rückantwort
    //
    if ( !answer.containsKey( "status" ) )
    {
      answer[ "status" ] = "OK";
    }
    // serializeJson( answer, content );
    serializeJsonPretty( answer, content );
    request->send( 200, "application/json", content );
    //
    // wenn PWM geändert wurde muss der Controller neu starten
    //
    if ( have_to_boot )
    {
      yield();
      restartRequired = true;
    }
    // Ende Gelände, nur einen beantworten
    return request;
  }

  AsyncWebServerRequest *ApiJSONServerClass::onLedCommandPost( AsyncWebServerRequest *request,
                                                               uint8_t *data,
                                                               size_t len,
                                                               size_t index,
                                                               size_t total )
  {
    DynamicJsonDocument doc( 1024 );
    DynamicJsonDocument answerDoc( 1024 );
    JsonObject answer = answerDoc.to< JsonObject >();
    DeserializationError err = deserializeJson( doc, ( const char * ) data );
    /*
    AsyncWebServerResponse *response = request->beginResponse( 200, "application/json", content );
    response->addHeader( "Access-Control-Allow-Origin", "*" );
    response->addHeader( "Vary", "Origin" );
    request->send( response );
    */
    String content;
    if ( err )
    {
      // das ging schief
      answer[ "error" ] = err.c_str();
      serializeJson( doc, content );
      Serial.println( content );
      request->send( 404, "application/json", content );
      return request;
    }
    // AsyncWebServerResponse *response = request->beginResponse( 200, "application/json", content );
    // response->addHeader( "Access-Control-Allow-Origin", "*" );
    // response->addHeader( "Vary", "Origin" );
    // request->send( response );
    //
    // Serial.println( "api rest recived..." );
    String result;
    JsonObject jobj = doc.as< JsonObject >();
    //
    // Alle Keys erfragen (im Allgemeinen sollte es nur einer sein)
    // p ist JSonPair
    //
    for ( auto p : jobj )
    {
      Serial.print( "api rest cmd: <" );
      Serial.print( p.key().c_str() );
      Serial.println( ">" );
      // is a JsonString
      if ( p.key() == cmd_set_rgbw )
      {
        //
        // SETZE LED auf Werte...
        //
        setLedValues( p.value().as< JsonObject >() );
        answer[ cmd_set_rgbw ] = "OK";
        // serializeJson( answer, content );
        serializeJsonPretty( answer, content );
        request->send( 200, "application/json", content );
        // Ende Gelände, nur einen beantworten
        return request;
      }
      else if ( p.key() == cmd_get_rgbw )
      {
        //
        // gib LED Werte zurück
        //
        answer[ cmd_standby ] = _ledControl->isStandBy();
        JsonObject ledValObj = answer.createNestedObject( cmd_rgbw );
        getLedValues( ledValObj );
        serializeJsonPretty( answer, content );
        request->send( 200, "application/json", content );
        // Ende Gelände, nur einen beantworten
        return request;
      }
      else if ( p.key() == cmd_get_standby )
      {
        //
        // gib STANDBY Wert zurück
        //
        answer[ cmd_standby ] = _ledControl->isStandBy();
        serializeJsonPretty( answer, content );
        request->send( 200, "application/json", content );
        // Ende Gelände, nur einen beantworten
        return request;
      }
      else if ( p.key() == cmd_set_standby )
      {
        //
        // setzt STANDBY Wert
        //
        String stbyVal( p.value().as< String >() );
        // vergleiche
        if ( stbyVal == cmd_true )
        {
          //
          // SETZEN
          //
          _ledControl->standBy( true );
        }
        else if ( stbyVal == cmd_false )
        {
          //
          // LÖSCHEN
          //
          _ledControl->standBy( false );
        }
        else
        {
          answer[ "error" ] = "wrong parameter";
          serializeJson( answer, content );
          Serial.println( "send answer: " + content );
          request->send( 200, "application/json", content );
          return request;
        }
        //
        // Wenn was gemaccht wurde
        //
        answer[ cmd_standby ] = _ledControl->isStandBy();
        serializeJsonPretty( answer, content );
        request->send( 200, "application/json", content );
        // Ende Gelände, nur einen beantworten
        return request;
      }
    }
    /*
    char output[ 128 ];
    serializeJson( doc, output );
    serializeJson( doc, output, sizeof( output ) );
    serializeJsonPretty( doc, output );
    // Compute the length of the minified JSON document
    int len1 = measureJson( doc );
    // Compute the length of the prettified JSON document
    int len2 = measureJsonPretty( doc );
    String output = "JSON = ";
    serializeJson( doc, output );
    */
    content = "{\"ok\" : \"OK\"}";
    request->send( 200, "application/json", content );
    return request;

    /*
    JsonObject obj1 = doc.createNestedObject();
        fetch('http://ota-server.fritz.box/rest/led',
          {
            method: 'POST',
            body: JSON.stringify
            (
              {
                set_rgbw:
                  {
                    red: 80.0,
                    green: 80.0,
                    blue: 80.0,
                    white: 80.0
                  }
              }
            ),
            headers:
            {
              'Content-type': 'application/json; charset=UTF-8'
            }
          }
        )
        .then(res => res.json())
        .then(console.log)
    */
  }

  void ApiJSONServerClass::setLedValues( const JsonObject &jobj )
  {
    // Serial.println( "set led values..." );
    LedControl::LedStatusClass status;
    //
    // mit aktuellen werten vorbelegen, dann kann nicht so viel
    // passieren wenn eine Farbe nicht erkannt wird
    //
    _ledControl->getPercentStatus( status );

    if ( jobj.containsKey( cmd_color_red ) )
    {
      status.red = jobj[ cmd_color_red ].as< double >();
    }
    if ( jobj.containsKey( cmd_color_green ) )
    {
      status.green = jobj[ cmd_color_green ].as< double >();
    }
    if ( jobj.containsKey( cmd_color_blue ) )
    {
      status.blue = jobj[ cmd_color_blue ].as< double >();
    }
    if ( jobj.containsKey( cmd_color_white ) )
    {
      status.white = jobj[ cmd_color_white ].as< double >();
    }
    //
    // SETZEN
    //
    _ledControl->setPercentStatus( status );
    // Serial.println( "set led values...OK" );
  }

  void ApiJSONServerClass::getLedValues( JsonObject jobj )
  {
    LedControl::LedStatusClass status;
    _ledControl->getPercentStatus( status );
    jobj[ "red" ] = status.red;
    jobj[ "green" ] = status.green;
    jobj[ "blue" ] = status.blue;
    jobj[ "white" ] = status.white;
  }

  AsyncWebServerRequest *ApiJSONServerClass::onGetIdentity( AsyncWebServerRequest *request )
  {
    String content;
#if defined( ESP8266 )
    content = "{\"apiid\": \"" + _id + "\", \"hardware\": \"ESP8266\"}";
    request->send( 200, "application/json", content );
#elif defined( ESP32 )
    content = "{\"apiid\": \"" + _id + "\", \"hardware\": \"ESP32\"}";
    request->send( 200, "application/json", content );
#endif
    Serial.println( "ApiJSONServerClass::onIdentity " + content );
    return request;
  }

  AsyncWebServerRequest *ApiJSONServerClass::onGetValues( AsyncWebServerRequest *request )
  {
    String content;
    DynamicJsonDocument answerDoc( 1024 );
    JsonObject answer = answerDoc.to< JsonObject >();
    answer[ cmd_standby ] = _ledControl->isStandBy();
    JsonObject ledValObj = answer.createNestedObject( cmd_rgbw );
    getLedValues( ledValObj );
    answer[ cmd_pwm_resolution ] = _prefs->getPwmResolution();
    answer[ cmd_pwm_frequence ] = _prefs->getPwmFreq();
    answer[ cmd_pwm_is_inverse ] = _prefs->getIsLEDInvers();
    serializeJsonPretty( answer, content );
    request->send( 200, "application/json", content );
    return request;
  }

  void ApiJSONServerClass::setID( const char *id )
  {
    _id = id;
  }

  String ApiJSONServerClass::getID()
  {
    return _id;
  }

  String ApiJSONServerClass::_getID()
  {
    String id = "API-";
#if defined( ESP8266 )
    id += String( ESP.getChipId() );
#elif defined( ESP32 )
    id += String( ( uint32_t ) ESP.getEfuseMac(), HEX );
#endif
    id.toUpperCase();
    return id;
  }

  void ApiJSONServerClass::loop()
  {
    if ( restartRequired )
    {
      yield();
      delay( 1000 );
      yield();
#if defined( ESP8266 )
      ESP.restart();
#elif defined( ESP32 )
      // ESP32 will commit sucide
      esp_task_wdt_init( 1, true );
      esp_task_wdt_add( NULL );
      while ( true )
        ;
#endif
    }
  }

}  // namespace APISrv