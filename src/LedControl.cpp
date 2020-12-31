#include "LedContol.hpp"

namespace LedControl
{
  LedControlClass::LedControlClass() : _standby( false )
  {
  }

  void LedControlClass::standBy( bool standby )
  {
    _standby = standby;
    uint32_t nullValue = 0;
    if ( !ledInited )
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
    ledInited = true;
    standBy();
  }
}  // namespace LedControl