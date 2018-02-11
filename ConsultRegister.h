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

#ifndef ConsultRegister_h
#define ConsultRegister_h

// Include default arduino stuffs
#include "WProgram.h"
#include "ConsultConversionFunctions.h"

class ConsultRegister {
  private:
    const char *_label;                     // Description of field
    byte _msbAddr;                          // Register MSB address
    byte _lsbAddr;                          // Register LSB address
    int _value;                             // Value we pulled from the ECU
    int (*_valueConvertFunc)(int);          // Function pointer to the conversion function
    
  public:
    // Constructors
    ConsultRegister();
    ConsultRegister(const char* label, byte msbAddr, byte lsbAddr, int (*valueConvertFunct)(int));
    
    // Getters
    const char *getLabel();
    byte getMsbAddr();
    byte getLsbAddr();
    int getValue();
    
    // Setters
    void setLabel(const char* v);
    void setMsbAddr(byte addr);
    void setLsbAddr(byte addr);
    void setValueConvertFunction(int (*valueConvertFunct)(int));
    void setValue(int v);
};

#endif
