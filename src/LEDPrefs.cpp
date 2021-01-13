#include "LEDPrefs.hpp"

namespace LEDSrv
{
  const char *LEDPrefs::serialStr = "20210113-132141-build-0130";

  LEDPrefs::LEDPrefs() : Preferences()
  {
  }

  LEDPrefs::~LEDPrefs()
  {
  }

  bool LEDPrefs::begin( bool readOnly )
  {
    bool ret = Preferences::begin( "OTASrv", readOnly );
    //
    // ist der Speicher gültig?
    //
    if ( !getBool( wasInitKey, false ) )
    {
      makeDefaults();
    }
    else
    {
      if ( getInt( prefsVersionKey ) != prefsVersion )
      {
        makeDefaults();
      }
    }
    return ret;
  }

  String LEDPrefs::getSSID()
  {
    return ( getString( SSIDKey, "nowhere" ) );
  }

  void LEDPrefs::setSSID( String ssid )
  {
    putString( SSIDKey, ssid );
  }

  String LEDPrefs::getPassword()
  {
    return ( getString( wlanPasswordKey, "" ) );
  }

  void LEDPrefs::setPassword( String _pw )
  {
    putString( wlanPasswordKey, _pw );
  }

  String LEDPrefs::getSerialStr()
  {
    return ( String( serialStr ) );
  }

  String LEDPrefs::getHostname()
  {
    return ( getString( hostnameKey, "localhost" ) );
  }

  void LEDPrefs::setHostname( String _hostname )
  {
    putString( hostnameKey, _hostname );
  }

  String LEDPrefs::getApiUser()
  {
    return ( getString( apiUserKey ) );
  }

  void LEDPrefs::setApiUser( String _userid )
  {
    putString( apiUserKey, _userid );
  }

  String LEDPrefs::getApiPassword()
  {
    return ( getString( apiPasswordKey ) );
  }

  void LEDPrefs::setApiPassword( String _pw )
  {
    putString( apiPasswordKey, _pw );
  }

  String LEDPrefs::getUpdateUser()
  {
    return ( getString( updateUserKey ) );
  }

  void LEDPrefs::setUpdateUser( String _userid )
  {
    putString( updateUserKey, _userid );
  }

  String LEDPrefs::getUpdatePassword()
  {
    return ( getString( updatePasswordKey ) );
  }

  void LEDPrefs::setUpdatePassword( String _passwd )
  {
    putString( updatePasswordKey, _passwd );
  }

  bool LEDPrefs::getIsLEDInvers()
  {
    return ( getBool( invertedLEDKey, false ) );
  }

  void LEDPrefs::setIsLEDInvers( bool _invers )
  {
    putBool( invertedLEDKey, _invers );
  }

  uint8_t LEDPrefs::getPwmResolution()
  {
    return ( getUChar( pwmResolutionKey, 8 ) );
  }

  void LEDPrefs::setPwmResolution( uint8_t _res )
  {
    putUChar( pwmResolutionKey, _res );
  }

  double LEDPrefs::getPwmFreq()
  {
    return ( getDouble( pwmFreqKey, 5000.0 ) );
  }

  void LEDPrefs::setPwmFreq( double _newFreq )
  {
    //
    // wird erst nach einem Neustart wirksam
    //
    putDouble( pwmFreqKey, _newFreq );
  }

  void LEDPrefs::getLedStats( LedControl::LedStatusClass &status )
  {
    status.red = getDouble( savedREDValueKey, 100.0 );
    status.green = getDouble( savedGREENValueKey, 100.0 );
    status.blue = getDouble( savedBLUEValueKey, 100.0 );
    status.white = getDouble( savedWHITEValueKey, 100.0 );
  }

  void LEDPrefs::setLedStats( LedControl::LedStatusClass &status )
  {
    // default RGBW in Prozent
    putDouble( savedREDValueKey, status.red );
    putDouble( savedGREENValueKey, status.green );
    putDouble( savedBLUEValueKey, status.blue );
    putDouble( savedWHITEValueKey, status.white );
  }

  bool LEDPrefs::makeDefaults()
  {
    //
    // beginn muss gelaufen sein
    // schreibe defaults für die Config
    //
    Serial.println( "preferences initialize..." );
    // wurde der schon initialisiert?
    putBool( wasInitKey, true );
    // welche Version der Einstellungen?
    putInt( prefsVersionKey, prefsVersion );
    // an welchem WLAN anmelden?
    putString( SSIDKey, defaultSSID );
    // welches PW im WLAN
    putString( wlanPasswordKey, defaultPassword );
    // welcher Hostname für das nDNS
    putString( hostnameKey, hostname );
    // api user name is null
    putString( apiUserKey, "" );
    // api passwort is null
    putString( apiPasswordKey, "" );
    // update user
    putString( updateUserKey, updateUser );
    // update password
    putString( updatePasswordKey, updatePassword );
    // led invers oder nicht als Voreinstellung
    putBool( invertedLEDKey, rgbInverted );
    // default auflösung der PWM-Timer
    putUChar( pwmResolutionKey, PWM_RESOLUTION );
    // default pwm frewuenz
    putDouble( pwmFreqKey, PWM_LED_FREQ );
    // default RGBW in Prozent
    putDouble( savedREDValueKey, 100.0 );
    putDouble( savedGREENValueKey, 100.0 );
    putDouble( savedBLUEValueKey, 100.0 );
    putDouble( savedWHITEValueKey, 100.0 );
    //
    Serial.println( "preferences initialize...DONE" );

    return true;
  }

}  // namespace LEDSrv