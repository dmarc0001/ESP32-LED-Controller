#pragma once
#include "OTAPrefs.hpp"
#include "ProjectDefaults.hpp"

//
// Klasse für PWM LED Control
//
namespace LedControl
{
  /**
   * Beschreibt den Status der LED-PWM Werte
   * Wertebereich abhängig von OTASrv::PWM_RESOLUTION
   */
  class LedStatusClass
  {
    public:
    double red{ 0 };
    double green{ 0 };
    double blue{ 0 };
    double white{ 0 };
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
    double fsteps{ static_cast< uint32_t >( OTASrv::PWM_STEPS ) };

    public:
    LedControlClass();
    void init();                                                              //! init Hardware
    void standBy( bool standby = true );                                      //! alle LED auf STANDBY
    void getPercentStatus( LedStatusClass &status );                          //! gib den Status in Prozent
    void setPercentStatus( LedStatusClass &status );                          //! setzt den Status in Prozent
    void getStatus( LedStatusClass &status );                                 //! gib den aktuellen Status
    void setStatus( LedStatusClass &status );                                 //! setzt die PWM Channel
    void setRGBW( uint8_t red, uint8_t green, uint8_t blue, uint8_t white );  //! Setzt die WM Channel
    void setRGB( uint8_t red, uint8_t green, uint8_t blue );                  //! Setz die PWM Channel
    void setRed( uint8_t red );                                               //! setze PWM Channel für ROT
    uint8_t getRed();                                                         //! hole PWM Channel für ROT
    void setGreen( uint8_t red );                                             //! setze PWM Channel für Grün
    uint8_t getGreen();                                                       //! hole PWM Channel für Grün
    void setBlue( uint8_t blue );                                             //! setze PWM Channel für Blau
    uint8_t getBlue();                                                        //! hole PWM Channel für Blau
    void setWhite( uint8_t white );                                           //! setze PWM Channel für Weiß
    uint8_t getWhite();                                                       //! hole PWM Channel für Weiß

    private:
    void readLedValuesToStatus( LedStatusClass &status );           //! lese PWM-Channels in Status
    void writeLedValuesFromStatus( const LedStatusClass &status );  //! schreibe PWM-Channels in Status
  };
}  // namespace LedControl