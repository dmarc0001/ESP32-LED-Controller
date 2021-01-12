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
  // TODO: set String values begrenzen
  class OTAPrefs : public Preferences
  {
    private:
    static const char *serialStr;

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
    void setUpdateUser( String );
    String getUpdatePassword();
    void setUpdatePassword( String );
    bool getIsLEDInvers();
    void setIsLEDInvers( bool is_invers );
    uint8_t getPwmResolution();
    void setPwmResolution( uint8_t res );
    double getPwmFreq();
    void setPwmFreq( double newFreq );
    void getLedStats( LedControl::LedStatusClass & );
    void setLedStats( LedControl::LedStatusClass & );

    private:
    bool makeDefaults();
  };
}  // namespace OTASrv