/**
  Arduino Consult
  June 12th 2011 -
  
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

#include "WProgram.h"
#include "ConsultErrorCode.h"

// Default Constructor
ConsultErrorCode::ConsultErrorCode() 
{
}

ConsultErrorCode::ConsultErrorCode(int code, int lastSeen)
{
  setCode(code);
  setLastSeen(lastSeen);
}

// Setters
void ConsultErrorCode::setCode(int code)
{
  _code = code;
}

void ConsultErrorCode::setLastSeen(int lastSeen)
{
  _lastSeen = lastSeen;
}

// Getters
int ConsultErrorCode::getCode()
{
  return _code;
}

int ConsultErrorCode::getLastSeen()
{
  return _lastSeen;
}


