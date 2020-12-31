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
    bool ledInited{ false };
    bool _standby;

    public:
    LedControlClass();
    void init();                          //! init Hardware
    void standBy( bool standby = true );  //! alle LED auf STANDBY
  };
}  // namespace LedControl