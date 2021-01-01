#include "ApiJSONServer.hpp"

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
  // const char *ApiJSONServerClass::cmd_true{ OTASrv::CMD_TRUE };
  // const char *ApiJSONServerClass::cmd_false{ OTASrv::CMD_FALSE };
  //
  //
  //
  void ApiJSONServerClass::begin( AsyncWebServer *server,
                                  const char *username,
                                  const char *password,
                                  LedControl::LedControlClass *ledControl )
  {
    _server = server;
    _ledControl = ledControl;

    if ( strlen( username ) > 0 )
    {
      _authRequired = true;
      _username = username;
      _password = password;
    }
    else
    {
      _authRequired = false;
      _username = "";
      _password = "";
    }
    _server->on( "/test", HTTP_GET, []( AsyncWebServerRequest *request ) {
      AsyncWebServerResponse *response = request->beginResponse( 200, "text/html", DebugData::webSiteText );
      response->addHeader( "Access-Control-Allow-Origin", "*" );
      request->send( response );
    } );
    //
    // Wer bist Du?
    //
    _server->on( "/rest/identity", HTTP_GET, [ & ]( AsyncWebServerRequest *request ) { onGetIdentity( request ); } );
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
          /*
          AsyncWebServerResponse *response = request->beginResponse( 200, "text/plain", "Ok" );
          response->addHeader( "Access-Control-Allow-Origin", "*" );
          response->addHeader( "Vary", "Origin" );
          request->send( response );
          */
        },
        NULL,
        [ this ]( AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total ) {
          return ( onLedCommandPost( request, data, len, index, total ) );
        } );
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
    String content;
    if ( err )
    {
      // das ging schief
      answer[ "error" ] = err.c_str();
      serializeJson( doc, content );
      Serial.println( content );
      request->send( 404, "application/json", content );
    }
    else
    {
      // response = request->beginResponse( 200, "application/json", "Ok" );
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

}  // namespace APISrv