
# Controller for LED RGBW stripes etc


## URL's
* /                   GET  -> Control GUI
* /rest/config        GET  -> REST Controller config GUI 

## REST API Url's
* /rest/identity
  - GET  -> REST Server identity
* /rest/led
  - GET  -> REST LED Status
* /rest/led
  - POST -> REST LED Command
* /rest/pwm
  - POST -> REST set PWM Settings
* /rest/fw_access
  - POST -> REST set Firmware access informations
* /rest/api_access
  - POST -> REST set api access informations
* /rest/wlan_access
  - POST -> REAT set WLAN SSID,PASSWD,mDNS Hostname

## Factory reset if somthin was wrong
Factory RESET GPIO34 (defined in ProjectDefaults.hpp "RESET_SWITCH_PIN") 
to ground while start controller.

## Initial connection
Than controller spans an WLAN as Accesspoint with addr 192.168.4.1 
and SSID "LED-AP-XXXXXX" while XXXX is the internal chip number.
The password is "password". Not supriseing...

## Configure the controller
If you connectet with the controller, you may give an URL, it's equal.
The internal DNS Micro Server resolve the Controlers IP and you will 
connected to the main Site. There cou my config some things.

Or you can put the controller in your WLAN over Url
http://192.168.4.1/ (then click in "config controller") 
  or
http://192.168.0.1/rest/config

the initial account is admin, password admin

### PWM propertys
You can config the PWM resolution (8, 10, 12 or 15 bits), the frequency 
in range 400 Hz up to 50 kHz.
You can also (for example for one anode RGB LED) invert the PWM signal.
This propertys ar availible in the config page.
You can also set the account for config, and if you whish an account 
for using the API.

All the Propertys are stored in teh NVM of the Controller.


enjoy!

