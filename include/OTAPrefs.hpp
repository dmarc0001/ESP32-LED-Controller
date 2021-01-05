#pragma once
#include <Preferences.h>
#include "LedContol.hpp"
#include "ProjectDefaults.hpp"

namespace LedControl
{
  // Forward deklaration für die Klasse
  class LedStatusClass;
}  // namespace LedControl

namespace OTASrv
{
  class OTAPrefs : public Preferences
  {
    private:
    public:
    OTAPrefs();
    ~OTAPrefs();

    bool begin( bool readOnly = false );
    String getSSID();
    String getPassword();
    String getSerialStr();
    String getHostname();
    String getApiUser();
    String getApiPassword();
    String getUpdateUser();
    String getUpdatePassword();
    bool getIsLEDInvers();
    uint8_t getPwmResolution();
    double getPwmFreq();
    void getLedStats( LedControl::LedStatusClass & );
    void setLedStats( LedControl::LedStatusClass & );

    private:
    bool makeDefaults();
  };
}  // namespace OTASrv