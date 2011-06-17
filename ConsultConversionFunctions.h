/**
  Arduino Consult
  June 12th 2011 - Stephen Powis (edrsp@mindspring.com)
  
  Note:
    Much of this logic swiped from Patrick Cole's Empeg Consult
    and simply ported to run on the Arduino platform.
    (http://www.suupremez.com/empeg-consult)
  
  Licensed under the GPL V2
  Feel free to use this code for whatever you want,
  all that I ask that if you do use it, shoot me an email and
  let me know what for!  Its always cool to hear when people
  find your code useful.
**/
#ifndef ConsultConversionFunctions_h
#define ConsultConversionFunctions_h

// Include default arduino stuffs
#include "WProgram.h"

class ConsultConversionFunctions
{
  public:
    static int convertCoolantTemp(int v);
    static int convertVehicleSpeed(int v);
    static int convertBatteryVoltage(int v);
    static int convertIgnitionTiming(int v);
    static int convertTachometer(int v);
};


#endif
