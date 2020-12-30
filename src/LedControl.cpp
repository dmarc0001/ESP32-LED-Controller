#include "LedContol.hpp"

namespace LedControl
{
  LedControlClass::LedControlClass() : _standby( false )
  {
  }

  void LedControlClass::standBy( bool standby )
  {
    _standby = standby;
  }

  void LedControlClass::init( OTASrv::OTAPrefs &prefs )
  {
  }
}  // namespace LedControl