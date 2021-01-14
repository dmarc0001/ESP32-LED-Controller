#include "ApiJSONServer.hpp"
#include "configPage.hpp"

namespace APISrv
{
  //
  // Konstenate Strings initialisieren
  //
  const JsonString ApiJSONServerClass::cmd_set_rgbw{ LEDSrv::CMD_SET_RGBW };
  const JsonString ApiJSONServerClass::cmd_get_rgbw{ LEDSrv::CMD_GET_RGBW };
  const JsonString ApiJSONServerClass::cmd_set_standby{ LEDSrv::CMD_SET_STANDBY };
  const JsonString ApiJSONServerClass::cmd_get_standby{ LEDSrv::CMD_GET_STANDBY };
  const JsonString ApiJSONServerClass::cmd_rgbw{ LEDSrv::CMD_RGBW };
  const JsonString ApiJSONServerClass::cmd_standby{ LEDSrv::CMD_STANDBY };
  const JsonString ApiJSONServerClass::cmd_color_red{ LEDSrv::CMD_COLOR_RED };
  const JsonString ApiJSONServerClass::cmd_color_green{ LEDSrv::CMD_COLOR_GREEN };
  const JsonString ApiJSONServerClass::cmd_color_blue{ LEDSrv::CMD_COLOR_BLUE };
  const JsonString ApiJSONServerClass::cmd_color_white{ LEDSrv::CMD_COLOR_WHITE };
  const JsonString ApiJSONServerClass::cmd_pwm_resolution{ LEDSrv::CMD_PWM_RESOLUTION };
  const JsonString ApiJSONServerClass::cmd_pwm_frequence{ LEDSrv::CMD_PWM_FREQUENCE };
  const JsonString ApiJSONServerClass::cmd_pwm_is_inverse{ LEDSrv::CMD_PWM_INVERSE };
  const JsonString ApiJSONServerClass::cmd_fw_userid{ LEDSrv::CMD_FW_USERID };
  const JsonString ApiJSONServerClass::cmd_fw_passwd{ LEDSrv::CMD_FW_PASSWD };
  const JsonString ApiJSONServerClass::cmd_api_userid{ LEDSrv::CMD_API_USERID };
  const JsonString ApiJSONServerClass::cmd_api_passwd{ LEDSrv::CMD_API_PASSWD };
  const JsonString ApiJSONServerClass::cmd_wlan_ssid{ LEDSrv::CMD_WLAN_SSID };
  const JsonString ApiJSONServerClass::cmd_wlan_passwd{ LEDSrv::CMD_WLAN_PASSWD };
  const JsonString ApiJSONServerClass::cmd_wlan_mdns{ LEDSrv::CMD_WLAN_MDNSHOST };

  /**
   * Starte den JSON Server
   */
  void ApiJSONServerClass::begin( AsyncWebServer *server, LEDSrv::LEDPrefs &prefs, LedControl::LedControlClass *ledControl )
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
    //
    // PWM Einstellungen: Auflösung, Frequenz, invertiert konfigurieren
    //
    _server->on(
        "/rest/pwm", HTTP_POST,
        [ & ]( AsyncWebServerRequest *request ) {
          if ( _apiAuthRequired )
          {
            if ( !request->authenticate( _apiUserName.c_str(), _apiPassword.c_str() ) )
            {
              return request->requestAuthentication();
            }
          }
        },
        NULL,
        [ this ]( AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total ) {
          return ( onPwmCommandPost( request, data, len, index, total ) );
        } );
    //
    // Zugriff auf Firmware Update mit user/passwort konfigurieren
    //
    _server->on(
        "/rest/fw_access", HTTP_POST,
        [ & ]( AsyncWebServerRequest *request ) {
          if ( _apiAuthRequired )
          {
            if ( !request->authenticate( _apiUserName.c_str(), _apiPassword.c_str() ) )
            {
              return request->requestAuthentication();
            }
          }
        },
        NULL,
        [ this ]( AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total ) {
          return ( onFwAccountCommandPost( request, data, len, index, total ) );
        } );
    //
    // Zugriff auf API mit user/passwort konfigurieren
    //
    _server->on(
        "/rest/api_access", HTTP_POST,
        [ & ]( AsyncWebServerRequest *request ) {
          if ( _configAuthRequired )
          {
            if ( !request->authenticate( _configUserName.c_str(), _configPassword.c_str() ) )
            {
              return request->requestAuthentication();
            }
          }
        },
        NULL,
        [ this ]( AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total ) {
          return ( onApiAccountCommandPost( request, data, len, index, total ) );
        } );
    //
    // Zugriff auf WLAN Einstellungen und mDNS konfigurieren
    //
    _server->on(
        "/rest/wlan_access", HTTP_POST,
        [ & ]( AsyncWebServerRequest *request ) {
          if ( _configAuthRequired )
          {
            if ( !request->authenticate( _configUserName.c_str(), _configPassword.c_str() ) )
            {
              return request->requestAuthentication();
            }
          }
        },
        NULL,
        [ this ]( AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total ) {
          return ( onWlanSettingCommandPost( request, data, len, index, total ) );
        } );
    //
    // LED Statusabfrage
    //
    _server->on( "/rest/led", HTTP_GET, [ & ]( AsyncWebServerRequest *request ) {
      if ( _apiAuthRequired )
      {
        if ( !request->authenticate( _apiUserName.c_str(), _apiPassword.c_str() ) )
        {
          return request->requestAuthentication();
        }
      }
      onGetValues( request );
    } );
    //
    // Handler für LED Command (JSON POST)
    //
    _server->on(
        "/rest/led", HTTP_POST,
        [ & ]( AsyncWebServerRequest *request ) {
          if ( _apiAuthRequired )
          {
            if ( !request->authenticate( _apiUserName.c_str(), _apiPassword.c_str() ) )
            {
              return request->requestAuthentication();
            }
          }
        },
        NULL,
        [ this ]( AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total ) {
          return ( onLedCommandPost( request, data, len, index, total ) );
        } );
    //
    // Server gestartet
    //
  }

  /**
   * Anforderung zum einstellen der SSID/Passwort/mDNS Hostname
   */
  AsyncWebServerRequest *ApiJSONServerClass::onWlanSettingCommandPost( AsyncWebServerRequest *request,
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
    String wlanSSID;
    String wlanPw;
    String wlanMDNS;
    if ( jobj.containsKey( cmd_wlan_ssid ) )
    {
      wlanSSID = jobj[ cmd_wlan_ssid ].as< String >();
      if ( 0 != wlanSSID.compareTo( "null" ) )
      {
        Serial.print( "cmd wlan AP <" );
        Serial.print( cmd_wlan_ssid.c_str() );
        Serial.print( "> : <" );
        Serial.print( wlanSSID );
        Serial.println( ">" );
      }
      else
      {
        wlanSSID.clear();
      }
    }
    if ( jobj.containsKey( cmd_wlan_passwd ) )
    {
      wlanPw = jobj[ cmd_wlan_passwd ].as< String >();
      if ( 0 != wlanPw.compareTo( "null" ) )
      {
        Serial.print( "cmd wlan AP <" );
        Serial.print( cmd_wlan_passwd.c_str() );
        Serial.print( "> : <" );
        Serial.print( wlanPw );
        Serial.println( ">" );
      }
      else
      {
        wlanPw.clear();
      }
    }
    if ( jobj.containsKey( cmd_wlan_mdns ) )
    {
      wlanMDNS = jobj[ cmd_wlan_mdns ].as< String >();
      if ( 0 != wlanMDNS.compareTo( "null" ) )
      {
        Serial.print( "cmd wlan AP <" );
        Serial.print( cmd_wlan_mdns.c_str() );
        Serial.print( "> : <" );
        Serial.print( wlanMDNS );
        Serial.println( ">" );
      }
      else
      {
        wlanMDNS.clear();
      }
    }
    // Plausibilität testen
    if ( wlanSSID.isEmpty() || wlanPw.isEmpty() )
    {
      answer[ cmd_wlan_ssid ] = "OK - not set";
      answer[ cmd_wlan_passwd ] = "OK - not set";
    }
    else
    {
      _prefs->setSSID( wlanSSID );
      _prefs->setPassword( wlanPw );
      answer[ cmd_wlan_ssid ] = "OK";
      answer[ cmd_wlan_passwd ] = "OK";
      have_to_boot = true;
    }
    if ( wlanMDNS.isEmpty() )
    {
      answer[ cmd_wlan_mdns ] = "OK - not set";
    }
    else
    {
      answer[ cmd_wlan_mdns ] = "OK";
      _prefs->setHostname( wlanMDNS );
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
    // wenn SSID oder MDNS geändert wurde muss der Controller neu starten
    //
    if ( have_to_boot )
    {
      yield();
      restartRequired = true;
    }
    // Ende Gelände, nur einen beantworten
    return request;
  }

  /**
   * Wenn die API mit Name/Passwort geschützt werden soll, die PArameter dafür
   */
  AsyncWebServerRequest *ApiJSONServerClass::onApiAccountCommandPost( AsyncWebServerRequest *request,
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
      return request;
    }
    String result;
    JsonObject jobj = doc.as< JsonObject >();
    String apiUserId;
    String apiUserPw;
    if ( jobj.containsKey( cmd_api_userid ) )
    {
      apiUserId = jobj[ cmd_api_userid ].as< String >();
      if ( 0 != apiUserId.compareTo( "null" ) )
      {
        Serial.print( "cmd api account <" );
        Serial.print( cmd_api_userid.c_str() );
        Serial.print( "> : <" );
        Serial.print( apiUserId );
        Serial.println( ">" );
      }
      else
      {
        apiUserId.clear();
      }
    }
    if ( jobj.containsKey( cmd_api_passwd ) )
    {
      apiUserPw = jobj[ cmd_api_passwd ].as< String >();
      if ( 0 != apiUserPw.compareTo( "null" ) )
      {
        Serial.print( "cmd api account <" );
        Serial.print( cmd_api_passwd.c_str() );
        Serial.print( "> : <" );
        Serial.print( apiUserPw );
        Serial.println( ">" );
      }
      else
      {
        apiUserId.clear();
      }
    }
    // Plausibilität prüfen
    if ( !( apiUserId.isEmpty() || apiUserPw.isEmpty() ) )
    {
      _apiAuthRequired = true;
      answer[ cmd_api_userid ] = "OK";
      answer[ cmd_api_passwd ] = "OK";
    }
    else if ( apiUserId.isEmpty() || apiUserPw.isEmpty() )
    {
      apiUserId.clear();
      apiUserPw.clear();
      answer[ cmd_api_userid ] = "OK - not set";
      answer[ cmd_api_passwd ] = "OK - not set";
      _apiAuthRequired = false;
    }
    _prefs->setApiUser( apiUserId );
    _prefs->setApiPassword( apiUserPw );
    _apiUserName = apiUserId.c_str();
    _apiPassword = apiUserPw.c_str();
    Serial.println( "new api account setting success." );
    if ( !answer.containsKey( "status" ) )
    {
      answer[ "status" ] = "OK";
    }
    // serializeJson( answer, content );
    serializeJsonPretty( answer, content );
    request->send( 200, "application/json", content );
    return request;
  }

  /**
   * Der Zugang zum UPDATE soll mit Name/Passwort geschützt werden
   */
  AsyncWebServerRequest *ApiJSONServerClass::onFwAccountCommandPost( AsyncWebServerRequest *request,
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
      return request;
    }
    String result;
    JsonObject jobj = doc.as< JsonObject >();
    String updateUserId;
    String updateUserPw;
    if ( jobj.containsKey( cmd_fw_userid ) )
    {
      updateUserId = jobj[ cmd_fw_userid ].as< String >();
      if ( 0 != updateUserId.compareTo( "null" ) )
      {
        Serial.print( "cmd fw account <" );
        Serial.print( cmd_fw_userid.c_str() );
        Serial.print( "> : <" );
        Serial.print( updateUserId );
        Serial.println( ">" );
      }
      else
      {
        updateUserId.clear();
      }
    }
    if ( jobj.containsKey( cmd_fw_passwd ) )
    {
      updateUserPw = jobj[ cmd_fw_passwd ].as< String >();
      if ( 0 != updateUserPw.compareTo( "null" ) )
      {
        Serial.print( "cmd fw account <" );
        Serial.print( cmd_fw_passwd.c_str() );
        Serial.print( "> : <" );
        Serial.print( updateUserPw );
        Serial.println( ">" );
      }
      else
      {
        updateUserPw.clear();
      }
    }
    // Teste Plausibilität
    if ( !( updateUserId.isEmpty() || updateUserPw.isEmpty() ) )
    {
      _prefs->setUpdateUser( updateUserId );
      answer[ cmd_fw_userid ] = "OK";
      _prefs->setUpdatePassword( updateUserPw );
      answer[ cmd_fw_passwd ] = "OK";
      _configAuthRequired = true;
      _configUserName = updateUserId.c_str();
      _configPassword = updateUserPw.c_str();
      Serial.println( "new firmware account setting success." );
    }
    else
    {
      answer[ cmd_fw_userid ] = "OK - not set";
      answer[ cmd_fw_passwd ] = "OK - not set";
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
    // Ende Gelände, nur einen beantworten
    return request;
  }

  /**
   * Die PWM Parameter Frequenz, Auflösung, Invertierung sollen geändert werden
   */
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
      else
      {
        answer[ cmd_pwm_resolution ] = "OK - not set";
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
        if ( pwm_freq >= LEDSrv::PWM_MIN_FREQENCE && pwm_freq <= LEDSrv::PWM_MAX_FREQENCE )
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
        else
        {
          answer[ cmd_pwm_frequence ] = "ERROR: frequency not allowed";
          answer[ "status" ] = "frequency too high!";
        }
      }
      else
      {
        answer[ cmd_pwm_frequence ] = "OK - not set";
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
        int pwm_inv = jobj[ cmd_pwm_is_inverse ].as< int >();
        //
        if ( _prefs->getIsLEDInvers() != ( pwm_inv == 0 ? false : true ) )
        {
          Serial.print( "pwm cmd: <" );
          Serial.print( cmd_pwm_is_inverse.c_str() );
          Serial.print( "> : " );
          Serial.println( pwm_inv );
          //
          // geändert, das Gerät muss neu gestartet werden
          //
          have_to_boot = true;
          _prefs->setIsLEDInvers( pwm_inv == 0 ? false : true );
        }

        answer[ cmd_pwm_is_inverse ] = "OK";
      }
      else
      {
        answer[ cmd_pwm_is_inverse ] = "OK - not set";
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

  /**
   * Setzte die LED Helligkeitswerte
   */
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
      return request;
    }
    String result;
    JsonObject jobj = doc.as< JsonObject >();
    //
    // Alle Keys erfragen (im Allgemeinen sollte es nur einer sein)
    // p ist JSonPair
    //
    for ( auto p : jobj )
    {
      // is a JsonString
      if ( p.key() == cmd_set_rgbw )
      {
        Serial.print( "api rest cmd: <" );
        Serial.print( p.key().c_str() );
        Serial.println( ">" );
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
        Serial.print( "api rest cmd: <standby> to <" );
        Serial.print( stbyVal );
        Serial.println( ">" );

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
        // Wenn was gemacht wurde
        //
        answer[ cmd_standby ] = _ledControl->isStandBy();
        serializeJsonPretty( answer, content );
        request->send( 200, "application/json", content );
        // Ende Gelände, nur einen beantworten
        return request;
      }
    }
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

  /**
   * Setzte die Helligkeitswerte der LED
   */
  void ApiJSONServerClass::setLedValues( const JsonObject &jobj )
  {
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
  }

  /**
   * gib die Helligkeitswerte der LED's
   */
  void ApiJSONServerClass::getLedValues( JsonObject jobj )
  {
    LedControl::LedStatusClass status;
    _ledControl->getPercentStatus( status );
    jobj[ "red" ] = status.red;
    jobj[ "green" ] = status.green;
    jobj[ "blue" ] = status.blue;
    jobj[ "white" ] = status.white;
  }

  /**
   * API identity zurück geben
   */
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

  /**
   * Webserver antwort auf LED Helligkeitsanfrage
   */
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

  /**
   * ID des Servers setzen
   */
  void ApiJSONServerClass::setID( const char *id )
  {
    _id = id;
  }

  /**
   * ID des Webservers erfragen
   */
  String ApiJSONServerClass::getID()
  {
    return _id;
  }

  /**
   * internes erzeugen der éindeutigen ID
   */
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

  /**
   * zyklische Frage ob ewas getan werden muss
   */
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