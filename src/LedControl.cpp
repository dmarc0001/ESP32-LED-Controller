#include "LedContol.hpp"

namespace LedControl
{
  /**
   * Der Konstruktor
   */
  LedControlClass::LedControlClass() : _standby( false )
  {
  }

  /**
   * Setzte LED in standby (off) ohne das im Speicher zu vermerken
   */
  void LedControlClass::standBy( bool standby )
  {
    uint32_t nullValue = 0;
    //
    if ( _standby == standby )
    {
      // nix zu tun...
      return;
    }
    //
    // war schon was initialisiert?
    //
    if ( !_ledInited )
    {
      return;
    }
    //
    // Invertierte Anschlüsse?
    //
    if ( LEDSrv::rgbInverted )
    {
      nullValue = _steps;
    }
    _standby = standby;
    //
    // Standby ein oder aus
    //
    if ( _standby )
    {
      // LED löschen
      ledcWrite( LEDSrv::PWM_LED_CHANNEL_RED, nullValue );
      ledcWrite( LEDSrv::PWM_LED_CHANNEL_GREEN, nullValue );
      ledcWrite( LEDSrv::PWM_LED_CHANNEL_BLUE, nullValue );
      ledcWrite( LEDSrv::PWM_LED_CHANNEL_WHITE, nullValue );
    }
    else
    {
      // werte aus dem Speicher lesen
      setPercentStatus( _standbyVal );
    }
  }

  /**
   * Den Standby status erfragen
   */
  bool LedControlClass::isStandBy()
  {
    return _standby;
  }

  /**
   * Initialisiere die LED Hardware
   */
  void LedControlClass::init( LEDSrv::LEDPrefs &prefs )
  {
    //
    // Namensraum der PREFS "ProjectDefaults.hpp"
    //
    using namespace LEDSrv;
    // Einstelungen lesen
    _inverted = prefs.getIsLEDInvers();
    uint8_t res = prefs.getPwmResolution();
    _fsteps = pow( 2.0, static_cast< double >( res ) );
    _steps = static_cast< uint32_t >( _fsteps );
    double pwmfreq = prefs.getPwmFreq();
    //
    // WLAN Lampe
    //
    pinMode( LED_WLANOK, OUTPUT );
    digitalWrite( LED_WLANOK, LOW );

    // configure LED PWM functionalitites
    ledcSetup( PWM_LED_CHANNEL_RED, pwmfreq, res );
    ledcSetup( PWM_LED_CHANNEL_GREEN, pwmfreq, res );
    ledcSetup( PWM_LED_CHANNEL_BLUE, pwmfreq, res );
    ledcSetup( PWM_LED_CHANNEL_WHITE, pwmfreq, res );

    // attach the channel to the GPIO2 to be controlled
    ledcAttachPin( PWM_LED_RED, PWM_LED_CHANNEL_RED );
    ledcAttachPin( PWM_LED_GREEN, PWM_LED_CHANNEL_GREEN );
    ledcAttachPin( PWM_LED_BLUE, PWM_LED_CHANNEL_BLUE );
    ledcAttachPin( PWM_LED_WHITE, PWM_LED_CHANNEL_WHITE );
    _ledInited = true;
    standBy( true );
  }

  /**
   * gib den Status in Prozent
   */
  void LedControlClass::getPercentStatus( LedStatusClass &status )
  {
    status = _standbyVal;
  }

  /**
   * setzt den Status in Prozent
   */
  void LedControlClass::setPercentStatus( LedStatusClass &status )
  {
    _standbyVal = status;
    if ( _standby )
    {
      return;
    }
    LedStatusClass _realStatus;
    //
    // übersetzte in reale Werte
    //
    _realStatus.red = round( ( _fsteps * static_cast< double >( status.red ) ) / 100.0 );
    _realStatus.green = round( ( _fsteps * static_cast< double >( status.green ) ) / 100.0 );
    _realStatus.blue = round( ( _fsteps * static_cast< double >( status.blue ) ) / 100.0 );
    _realStatus.white = round( ( _fsteps * static_cast< double >( status.white ) ) / 100.0 );
    //
    // kein Standby
    //
    writeLedValuesFromStatus( _realStatus );
  }

  /**
   * lese PWM-Channels in den Status
   */
  void LedControlClass::readLedValuesToStatus( LedStatusClass &status )
  {
    using namespace LEDSrv;
    status.red = static_cast< double >( _inverted ? _steps - ledcRead( PWM_LED_CHANNEL_RED ) : ledcRead( PWM_LED_CHANNEL_RED ) );
    status.green = static_cast< double >( _inverted ? _steps - ledcRead( PWM_LED_CHANNEL_GREEN ) : ledcRead( PWM_LED_CHANNEL_GREEN ) );
    status.blue = static_cast< double >( _inverted ? _steps - ledcRead( PWM_LED_CHANNEL_BLUE ) : ledcRead( PWM_LED_CHANNEL_BLUE ) );
    status.white = static_cast< double >( _inverted ? _steps - ledcRead( PWM_LED_CHANNEL_WHITE ) : ledcRead( PWM_LED_CHANNEL_WHITE ) );
  }

  /**
   * Schreibe aus dem Status in die PWM Channels
   */
  void LedControlClass::writeLedValuesFromStatus( const LedStatusClass &status )
  {
    using namespace LEDSrv;

    if ( !_standby )
    {
      ledcWrite( LEDSrv::PWM_LED_CHANNEL_RED, static_cast< uint32_t >( _inverted ? _steps - status.red : status.red ) );
      ledcWrite( LEDSrv::PWM_LED_CHANNEL_GREEN, static_cast< uint32_t >( _inverted ? _steps - status.green : status.green ) );
      ledcWrite( LEDSrv::PWM_LED_CHANNEL_BLUE, static_cast< uint32_t >( _inverted ? _steps - status.blue : status.blue ) );
      ledcWrite( LEDSrv::PWM_LED_CHANNEL_WHITE, static_cast< uint32_t >( _inverted ? _steps - status.white : status.white ) );
      yield();
      delay( 10 );
    }
  }

}  // namespace LedControl