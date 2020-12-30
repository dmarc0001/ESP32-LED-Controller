#include "LedContol.hpp"

namespace LedControl
{
  LedControlClass::LedControlClass() : _standby( false )
  {
  }

  void LedControlClass::standBy( bool standby )
  {
    _standby = standby;
    if ( !ledInited )
    {
      return;
    }
    // alles aus
    ledcWrite( OTASrv::PWM_LED_CHANNEL_RED, 0 );
    ledcWrite( OTASrv::PWM_LED_CHANNEL_GREEN, 0 );
    ledcWrite( OTASrv::PWM_LED_CHANNEL_BLUE, 0 );
    ledcWrite( OTASrv::PWM_LED_CHANNEL_WHITE, 0 );
    digitalWrite( OTASrv::LED_WLANOK, LOW );
  }

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
    // PWM contoll init
    ledcWrite( PWM_LED_CHANNEL_RED, 0 );
    ledcWrite( PWM_LED_CHANNEL_GREEN, 0 );
    ledcWrite( PWM_LED_CHANNEL_BLUE, 0 );
    ledcWrite( PWM_LED_CHANNEL_WHITE, 0 );
  }
}  // namespace LedControl