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

#ifndef ConsultErrorCode_h
#define ConsultErrorCode_h

// Include default arduino stuffs
#include "WProgram.h"

class ConsultErrorCode
{
  private:
    int _code;
    int _lastSeen;
    
  public:
    // Constructors
    ConsultErrorCode();
    ConsultErrorCode(int code, int lastSeen);
    
    // Setters
    void setCode(int code);
    void setLastSeen(int lastSeen);
    
    // Getters
    int getCode();
    int getLastSeen();
};

#endif
