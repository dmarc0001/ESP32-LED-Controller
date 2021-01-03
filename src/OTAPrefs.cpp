#include "OTAPrefs.hpp"

namespace OTASrv
{
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
    if ( getString( serialKey, "0000" ) != String( serialStr ) )
    {
      //
      // aktualisieren! Neue Firmware
      //
      putString( serialKey, serialStr );
      // TODO: alle Strings prüfen, könnten nun auch geändert sein
    }

    return ret;
  }

  String OTAPrefs::getSSID()
  {
    return ( getString( SSIDKey, "nowhere" ) );
  }

  String OTAPrefs::getPassword()
  {
    return ( getString( passwordKey, "" ) );
  }

  String OTAPrefs::getSerialStr()
  {
    return ( getString( serialKey, "0000" ) );
  }

  String OTAPrefs::getHostname()
  {
    return ( getString( hostnameKey, "localhost" ) );
  }

  String OTAPrefs::getApiUser()
  {
    return ( getString( apiUserKey ) );
  }

  String OTAPrefs::getApiPassword()
  {
    return ( getString( passwordKey ) );
  }

  String OTAPrefs::getUpdateUser()
  {
    return ( getString( updateUserKey ) );
  }

  String OTAPrefs::getUpdatePassword()
  {
    return ( getString( updatePasswordKey ) );
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
    putString( passwordKey, defaultPassword );
    // welcher Hostname für das nDNS
    putString( hostnameKey, hostname );
    // Seriennummer / version
    putString( serialKey, serialStr );
    // api user name
    putString( apiUserKey, apiUser );
    // api passwort
    putString( apiPasswordKey, apiPassword );
    // update user
    putString( updateUserKey, updateUser );
    // update password
    putString( updatePasswordKey, updatePassword );
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