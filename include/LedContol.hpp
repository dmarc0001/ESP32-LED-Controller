#pragma once
#include "OTAPrefs.hpp"
#include "ProjectDefaults.hpp"

//
// Klasse für PWM LED Control
//
namespace LedControl
{
  class LedControlClass
  {
    private:
    bool _standby;

    public:
    LedControlClass();
    void standBy( bool standby = true );
    void init( OTASrv::OTAPrefs &prefs );
  };
}  // namespace LedControl