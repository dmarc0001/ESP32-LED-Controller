#include "OTAPrefs.hpp"

namespace OTASrv
{
  const char *OTAPrefs::serialStr = "20210113-122643-build-0125";

  OTAPrefs::OTAPrefs() : Preferences()
  {
  }

  OTAPrefs::~OTAPrefs()
  {
  }

  bool OTAPrefs::begin( bool readOnly )
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

  String OTAPrefs::getSSID()
  {
    return ( getString( SSIDKey, "nowhere" ) );
  }

  void OTAPrefs::setSSID( String ssid )
  {
    putString( SSIDKey, ssid );
  }

  String OTAPrefs::getPassword()
  {
    return ( getString( wlanPasswordKey, "" ) );
  }

  void OTAPrefs::setPassword( String _pw )
  {
    putString( wlanPasswordKey, _pw );
  }

  String OTAPrefs::getSerialStr()
  {
    return ( String( serialStr ) );
  }

  String OTAPrefs::getHostname()
  {
    return ( getString( hostnameKey, "localhost" ) );
  }

  void OTAPrefs::setHostname( String _hostname )
  {
    putString( hostnameKey, _hostname );
  }

  String OTAPrefs::getApiUser()
  {
    return ( getString( apiUserKey ) );
  }

  void OTAPrefs::setApiUser( String _userid )
  {
    putString( apiUserKey, _userid );
  }

  String OTAPrefs::getApiPassword()
  {
    return ( getString( apiPasswordKey ) );
  }

  void OTAPrefs::setApiPassword( String _pw )
  {
    putString( apiPasswordKey, _pw );
  }

  String OTAPrefs::getUpdateUser()
  {
    return ( getString( updateUserKey ) );
  }

  void OTAPrefs::setUpdateUser( String _userid )
  {
    putString( updateUserKey, _userid );
  }

  String OTAPrefs::getUpdatePassword()
  {
    return ( getString( updatePasswordKey ) );
  }

  void OTAPrefs::setUpdatePassword( String _passwd )
  {
    putString( updatePasswordKey, _passwd );
  }

  bool OTAPrefs::getIsLEDInvers()
  {
    return ( getBool( invertedLEDKey, false ) );
  }

  void OTAPrefs::setIsLEDInvers( bool _invers )
  {
    putBool( invertedLEDKey, _invers );
  }

  uint8_t OTAPrefs::getPwmResolution()
  {
    return ( getUChar( pwmResolutionKey, 8 ) );
  }

  void OTAPrefs::setPwmResolution( uint8_t _res )
  {
    putUChar( pwmResolutionKey, _res );
  }

  double OTAPrefs::getPwmFreq()
  {
    return ( getDouble( pwmFreqKey, 5000.0 ) );
  }

  void OTAPrefs::setPwmFreq( double _newFreq )
  {
    //
    // wird erst nach einem Neustart wirksam
    //
    putDouble( pwmFreqKey, _newFreq );
  }

  void OTAPrefs::getLedStats( LedControl::LedStatusClass &status )
  {
    status.red = getDouble( savedREDValueKey, 100.0 );
    status.green = getDouble( savedGREENValueKey, 100.0 );
    status.blue = getDouble( savedBLUEValueKey, 100.0 );
    status.white = getDouble( savedWHITEValueKey, 100.0 );
  }

  void OTAPrefs::setLedStats( LedControl::LedStatusClass &status )
  {
    // default RGBW in Prozent
    putDouble( savedREDValueKey, status.red );
    putDouble( savedGREENValueKey, status.green );
    putDouble( savedBLUEValueKey, status.blue );
    putDouble( savedWHITEValueKey, status.white );
  }

  bool OTAPrefs::makeDefaults()
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

}  // namespace OTASrv