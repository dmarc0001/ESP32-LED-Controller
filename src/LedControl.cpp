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
    _standby = standby;
    uint32_t nullValue = 0;
    if ( !_ledInited )
    {
      return;
    }
    // alles aus
    if ( OTASrv::rgbInverted )
    {
      nullValue = OTASrv::PWM_STEPS;
    }
    ledcWrite( OTASrv::PWM_LED_CHANNEL_RED, nullValue );
    ledcWrite( OTASrv::PWM_LED_CHANNEL_GREEN, nullValue );
    ledcWrite( OTASrv::PWM_LED_CHANNEL_BLUE, nullValue );
    ledcWrite( OTASrv::PWM_LED_CHANNEL_WHITE, nullValue );
    digitalWrite( OTASrv::LED_WLANOK, LOW );
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
    standBy();
  }

  /**
   * gib den Status in Prozent
   */
  void LedControlClass::getPercentStatus( LedStatusClass &status )
  {
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
   * Schreibt den aktuellen Status in ein Statusobjekt
   */
  void LedControlClass::getStatus( LedStatusClass &status )
  {
    readLedValuesToStatus( status );
  }

  /**
   * Setzt die PWM Channels auf Werte aus dem Statusobjekt
   */
  void LedControlClass::setStatus( LedStatusClass &status )
  {
    writeLedValuesFromStatus( status );
  }

  /**
   * Setzte direkt die RGBW Werte
   */
  void LedControlClass::setRGBW( uint8_t red, uint8_t green, uint8_t blue, uint8_t white )
  {
    LedStatusClass status;
    status.red = static_cast< double >( red );
    status.green = static_cast< double >( green );
    status.blue = static_cast< double >( blue );
    status.white = static_cast< double >( white );
    writeLedValuesFromStatus( status );
  }

  /**
   * Setzte RGB Werte und rechne in RGBW um
   */
  void LedControlClass::setRGB( uint8_t red, uint8_t green, uint8_t blue )
  {
    LedStatusClass status;
    //
    // TODO:Je nach OTASrv::RGB_MODE umrechnen oder nicht
    //
    if ( OTASrv::RGB_MODE )
    {
      // TODO: vorher  nach RGB umrechnen
      status.red = static_cast< double >( red );
      status.green = static_cast< double >( green );
      status.blue = static_cast< double >( blue );
      status.white = 0.0;
    }
    else
    {
      // nicht umrechnen, einfach übernehmen
      status.red = static_cast< double >( red );
      status.green = static_cast< double >( green );
      status.blue = static_cast< double >( blue );
      status.white = 0.0;
    }
    writeLedValuesFromStatus( status );
  }

  /**
   *  setze PWM Channel für Grün
   */
  void LedControlClass::setRed( uint8_t red )
  {
    ledcWrite( OTASrv::PWM_LED_CHANNEL_RED, OTASrv::rgbInverted ? fsteps - red : red );
  }

  /**
   *  hole PWM Channel für ROT
   */
  uint8_t LedControlClass::getRed()
  {
    return ( OTASrv::PWM_STEPS ? OTASrv::PWM_STEPS - ledcRead( OTASrv::PWM_LED_CHANNEL_RED )
                               : ledcRead( OTASrv::PWM_LED_CHANNEL_RED ) );
  }

  /**
   *  setze PWM Channel für Grün
   */
  void LedControlClass::setGreen( uint8_t green )
  {
    ledcWrite( OTASrv::PWM_LED_CHANNEL_GREEN, OTASrv::rgbInverted ? OTASrv::PWM_STEPS - green : green );
  }

  /**
   *  hole PWM Channel für Grün
   */
  uint8_t LedControlClass::getGreen()
  {
    return ( OTASrv::PWM_STEPS ? OTASrv::PWM_STEPS - ledcRead( OTASrv::PWM_LED_CHANNEL_GREEN )
                               : ledcRead( OTASrv::PWM_LED_CHANNEL_GREEN ) );
  }

  /**
   *  setze PWM Channel für Blau
   */
  void LedControlClass::setBlue( uint8_t blue )
  {
    ledcWrite( OTASrv::PWM_LED_CHANNEL_GREEN, OTASrv::rgbInverted ? OTASrv::PWM_STEPS - blue : blue );
  }

  /**
   *  hole PWM Channel für Blau
   */
  uint8_t LedControlClass::getBlue()
  {
    return ( OTASrv::PWM_STEPS ? OTASrv::PWM_STEPS - ledcRead( OTASrv::PWM_LED_CHANNEL_BLUE )
                               : ledcRead( OTASrv::PWM_LED_CHANNEL_BLUE ) );
  }

  /**
   *  setze PWM Channel für Weiß
   */
  void LedControlClass::setWhite( uint8_t white )
  {
    ledcWrite( OTASrv::PWM_LED_CHANNEL_GREEN, OTASrv::rgbInverted ? OTASrv::PWM_STEPS - white : white );
  }

  /**
   *  hole PWM Channel für Weiß
   */
  uint8_t LedControlClass::getWhite()
  {
    return ( OTASrv::PWM_STEPS ? OTASrv::PWM_STEPS - ledcRead( OTASrv::PWM_LED_CHANNEL_WHITE )
                               : ledcRead( OTASrv::PWM_LED_CHANNEL_WHITE ) );
  }

  /**
   * lese PWM-Channels in Status
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
   * Schreibe PWM Channels in den Status
   */
  void LedControlClass::writeLedValuesFromStatus( const LedStatusClass &status )
  {
    using namespace OTASrv;

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

}  // namespace LedControl