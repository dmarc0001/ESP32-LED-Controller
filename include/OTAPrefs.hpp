#pragma once
#include <Preferences.h>
#include "ProjectDefaults.hpp"

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

    private:
    bool makeDefaults();
  };
}  // namespace OTASrv