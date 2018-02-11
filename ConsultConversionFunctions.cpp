/**
  Arduino Consult
  June 12th 2011 -

  Licensed under the GPL V2
  Feel free to use this code for whatever you want,
  all that I ask that if you do use it, shoot me an email and
  let me know what for!  Its always cool to hear when people
  find your code useful.
**/

#include "WProgram.h"
#include "ConsultConversionFunctions.h"

// Converts coolant temp value from ECU into human readable form
int ConsultConversionFunctions::convertCoolantTemp(int v)
{
  // Convert to celcius
  v = v - 50;

//  if (!_isMetric) {
    // Convert to farenh
    return (v * 1.8) + 32;
//  }
  return v;
}

// Convert vehicle speed value from ECU into human readable form
int ConsultConversionFunctions::convertVehicleSpeed(int v)
{
  v = v * 2;
  v = v * 0.621;
/*  if (!_isMetric) {
    // Convert to mph
    return v * 0.621;
  }
*/
  return v;
}

// Convert Battery Voltage from ECU into human readable form
int ConsultConversionFunctions::convertBatteryVoltage(int v)
{
  // Convert to mV
  return v * 80;
}

// Convert Ignition Timing value from ECU to human readable form
int ConsultConversionFunctions::convertIgnitionTiming(int v)
{
  return 110 - v;
}

// Convert tach from ECU to human readable form
int ConsultConversionFunctions::convertTachometer(int v)
{
  return v * 12.5;
}

// Convert Maf voltage from ECU to human readable form
int ConsultConversionFunctions::convertMafVoltage(int v)
{
	// return value is in mV
	return v * 5;
}

// Convert O2 voltage from ECU to human readable form
int ConsultConversionFunctions::convertO2Voltage(int v)
{
	// Return value is in mV
	return v * 10;
}