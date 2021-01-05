#pragma once
#include "OTAPrefs.hpp"
#include "ProjectDefaults.hpp"

namespace OTASrv
{
  // Vorwärtsdeklaration
  class OTAPrefs;
}  // namespace OTASrv

//
// Klasse für PWM LED Control
//
namespace LedControl
{
  constexpr double EPSILON = 0.05;
  /**
   * Beschreibt den Status der LED-PWM Werte
   * Wertebereich abhängig von OTASrv::PWM_RESOLUTION
   */
  class LedStatusClass
  {
    public:
    double red{ 0 };                          //! Speicher für ROT
    double green{ 0 };                        //! Speicher für GRÜM
    double blue{ 0 };                         //! Speicher für BLAU
    double white{ 0 };                        //! Speicher für WEISS
    bool operator==( LedStatusClass &other )  //! Überlade Vergleichsoperator (inline)
    {
      return ( ( abs( red - other.red ) < EPSILON ) && ( abs( green - other.green ) < EPSILON ) &&
               ( abs( blue - other.blue ) < EPSILON ) && ( abs( white - other.white ) < EPSILON ) );
    }
    bool operator!=( LedStatusClass &other )  //! Überlade Vergleichsoperator (inline)
    {
      return ( !( ( abs( red - other.red ) < EPSILON ) && ( abs( green - other.green ) < EPSILON ) &&
                  ( abs( blue - other.blue ) < EPSILON ) && ( abs( white - other.white ) < EPSILON ) ) );
    }
  };

  /**
   * Kontrolliert die LED
   */
  class LedControlClass
  {
    private:
    bool _ledInited{ false };           //! initiert?
    bool _standby{ true };              //! standby (LED off aber keine Werte im Speicher berändert)
    bool _rgbmode{ OTASrv::RGB_MODE };  //! NUR RGB LED nutzen? Voreinstellung aus ProjectDefaults
    bool _inverted{ false };            //! LED'S invertiert?
    double _fsteps{ 255.0 };            //! Helligkeitsstufen
    uint32_t _steps{ 255 };             //! Helligkeitsstufen in 32 uint

    LedStatusClass _standbyVal;  //! Wert bei Standby gespeichert
    // double fsteps{ static_cast< uint32_t >( OTASrv::PWM_STEPS ) };  //! Helligkeitsstufen

    public:
    LedControlClass();
    void init( OTASrv::OTAPrefs &prefs );             //! init Hardware
    void standBy( bool standby = true );              //! alle LED auf STANDBY
    bool isStandBy();                                 //! ist alles auf  STANDBY
    void getPercentStatus( LedStatusClass &status );  //! gib den Status in Prozent
    void setPercentStatus( LedStatusClass &status );  //! setzt den Status in Prozent

    private:
    void readLedValuesToStatus( LedStatusClass &status );           //! lese PWM-Channels in Status
    void writeLedValuesFromStatus( const LedStatusClass &status );  //! schreibe PWM-Channels in Status
  };
}  // namespace LedControl