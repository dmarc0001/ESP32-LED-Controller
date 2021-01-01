#include "LedContol.hpp"

namespace LedControl
{
  /**
   * Der Konsgtruktor
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
    _standby = standby;
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
    if ( OTASrv::rgbInverted )
    {
      nullValue = OTASrv::PWM_STEPS;
    }
    //
    // Standby ein oder aus
    //
    if ( _standby )
    {
      // Werte in den Speicher retten
      getPercentStatus( _standbyVal );
      // LED löschen
      ledcWrite( OTASrv::PWM_LED_CHANNEL_RED, nullValue );
      ledcWrite( OTASrv::PWM_LED_CHANNEL_GREEN, nullValue );
      ledcWrite( OTASrv::PWM_LED_CHANNEL_BLUE, nullValue );
      ledcWrite( OTASrv::PWM_LED_CHANNEL_WHITE, nullValue );
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
  void LedControlClass::init()
  {
    //
    // Namensraum der PREFS "ProjectDefaults.hpp"
    //
    using namespace OTASrv;
    //
    // WLAN Lampe
    //
    pinMode( LED_WLANOK, OUTPUT );
    digitalWrite( LED_WLANOK, LOW );

    // configure LED PWM functionalitites
    ledcSetup( PWM_LED_CHANNEL_RED, PWM_LED_FREQ, PWM_RESOLUTION );
    ledcSetup( PWM_LED_CHANNEL_GREEN, PWM_LED_FREQ, PWM_RESOLUTION );
    ledcSetup( PWM_LED_CHANNEL_BLUE, PWM_LED_FREQ, PWM_RESOLUTION );
    ledcSetup( PWM_LED_CHANNEL_WHITE, PWM_LED_FREQ, PWM_RESOLUTION );

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
    if ( _standby )
    {
      // aus dem Speicher holen
      status = _standbyVal;
      return;
    }
    //
    // Kein Standby
    //
    LedStatusClass _realStatus;
    //
    // lese die Werte ein
    //
    readLedValuesToStatus( _realStatus );
    status.red = round( ( static_cast< double >( _realStatus.red ) * 100.0 ) / fsteps );
    status.green = round( ( static_cast< double >( _realStatus.green ) * 100.0 ) / fsteps );
    status.blue = round( ( static_cast< double >( _realStatus.blue ) * 100.0 ) / fsteps );
    status.white = round( ( static_cast< double >( _realStatus.white ) * 100.0 ) / fsteps );
  }

  /**
   * setzt den Status in Prozent
   */
  void LedControlClass::setPercentStatus( LedStatusClass &status )
  {
    if ( _standby )
    {
      // aus em Speicher holen
      _standbyVal = status;
      return;
    }
    //
    // kein Standby
    //
    LedStatusClass _realStatus;
    //
    // übersetzte in reale Werte
    //
    _realStatus.red = round( ( fsteps * static_cast< double >( status.red ) ) / 100.0 );
    _realStatus.green = round( ( fsteps * static_cast< double >( status.green ) ) / 100.0 );
    _realStatus.blue = round( ( fsteps * static_cast< double >( status.blue ) ) / 100.0 );
    _realStatus.white = round( ( fsteps * static_cast< double >( status.white ) ) / 100.0 );
    writeLedValuesFromStatus( _realStatus );
  }

  /**
   * lese PWM-Channels in den Status
   */
  void LedControlClass::readLedValuesToStatus( LedStatusClass &status )
  {
    using namespace OTASrv;
    status.red = static_cast< double >( OTASrv::PWM_STEPS ? OTASrv::PWM_STEPS - ledcRead( PWM_LED_CHANNEL_RED )
                                                          : ledcRead( PWM_LED_CHANNEL_RED ) );
    status.green = static_cast< double >( OTASrv::PWM_STEPS ? OTASrv::PWM_STEPS - ledcRead( PWM_LED_CHANNEL_GREEN )
                                                            : ledcRead( PWM_LED_CHANNEL_GREEN ) );
    status.blue = static_cast< double >( OTASrv::PWM_STEPS ? OTASrv::PWM_STEPS - ledcRead( PWM_LED_CHANNEL_BLUE )
                                                           : ledcRead( PWM_LED_CHANNEL_BLUE ) );
    status.white = static_cast< double >( OTASrv::PWM_STEPS ? OTASrv::PWM_STEPS - ledcRead( PWM_LED_CHANNEL_WHITE )
                                                            : ledcRead( PWM_LED_CHANNEL_WHITE ) );
  }

  /**
   * Schreibe aus dem Status in die PWM Channels
   */
  void LedControlClass::writeLedValuesFromStatus( const LedStatusClass &status )
  {
    using namespace OTASrv;

    if ( !_standby )
    {
      ledcWrite( OTASrv::PWM_LED_CHANNEL_RED,
                 static_cast< uint32_t >( OTASrv::rgbInverted ? OTASrv::PWM_STEPS - status.red : status.red ) );
      ledcWrite( OTASrv::PWM_LED_CHANNEL_GREEN,
                 static_cast< uint32_t >( OTASrv::rgbInverted ? OTASrv::PWM_STEPS - status.green : status.green ) );
      ledcWrite( OTASrv::PWM_LED_CHANNEL_BLUE,
                 static_cast< uint32_t >( OTASrv::rgbInverted ? OTASrv::PWM_STEPS - status.blue : status.blue ) );
      ledcWrite( OTASrv::PWM_LED_CHANNEL_WHITE,
                 static_cast< uint32_t >( OTASrv::rgbInverted ? OTASrv::PWM_STEPS - status.white : status.white ) );
      yield();
      delay( 40 );
    }
  }

}  // namespace LedControl