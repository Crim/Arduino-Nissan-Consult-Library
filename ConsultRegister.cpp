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
#include "ConsultRegister.h"

// Default Constructor
ConsultRegister::ConsultRegister() 
{
}

// Constructor
ConsultRegister::ConsultRegister(const char* label, byte msbAddr, byte lsbAddr, int (*functionPointer)(int))
{
  // Set
  setLabel(label);
  setMsbAddr(msbAddr);
  setLsbAddr(lsbAddr);
  setValueConvertFunction(functionPointer);
  _value = 0;
}

// Setter Methods
void ConsultRegister::setLabel(const char* v)
{
  _label = v;
}

void ConsultRegister::setMsbAddr(byte addr) 
{
  _msbAddr = addr;
}

void ConsultRegister::setLsbAddr(byte addr)
{
  _lsbAddr = addr;
}

void ConsultRegister::setValue(int v)
{
  if (_valueConvertFunc != NULL) {
    v = _valueConvertFunc(v);
  }
  _value = v;
}

void ConsultRegister::setValueConvertFunction(int (*functionPointer)(int))
{
  _valueConvertFunc = functionPointer;
}

// Getter Methods
const char *ConsultRegister::getLabel()
{
  return _label;
}

byte ConsultRegister::getMsbAddr()
{
  return _msbAddr;
}

byte ConsultRegister::getLsbAddr()
{
  return _lsbAddr;
}

int ConsultRegister::getValue()
{
  return _value;
}
