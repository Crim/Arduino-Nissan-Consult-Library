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

#ifndef Consult_h
#define Consult_h

// Include default arduino stuffs
#include "WProgram.h"
#include "ConsultRegister.h"
#include "ConsultErrorCode.h"

// Define ECU commands
#define ECU_COMMAND_READ_REGISTER 0x5A
#define ECU_COMMAND_SELF_DIAG 0xD1
#define ECU_COMMAND_CLEAR_CODES 0xC1
#define ECU_COMMAND_ECU_INFO 0xD0
#define ECU_COMMAND_TERM 0xF0
#define ECU_COMMAND_STOP_STREAM 0x3

// Define single register locations
#define ECU_REGISTER_COOLANT_TEMP 0x08      
#define ECU_REGISTER_VEHICLE_SPEED 0x0B  
#define ECU_REGISTER_BATTERY_VOLTAGE 0x0C
#define ECU_REGISTER_IGNITION_TIMING 0x16

// Define two register locations
#define ECU_REGISTER_TACH_MSB 0x00
#define ECU_REGISTER_TACH_LSB 0x01

// Define our null register location
#define ECU_REGISTER_NULL 0xFF

class Consult {  
  /*
  // Define our Register Struct
  struct ecuRegister {
    char *label;    // Label for debugging purposes
    byte addrMsb;   // This sensors MSB register 
    byte addrLsb;   // This sensors LSB register if it uses two registers, otherwise put in ECU_REGISTER_NULL,
    int value;      // The value of the register gets stored here
    int (* valueConvertFunc)(int value);  // Function pointer to the conversion function
  };
  */
       
  public:
    // Default Constructor
    Consult();
    
    // Set which Serial device we are going to use
    void setSerial(HardwareSerial *serial);
  
    // Determine what type of units you want
    void setMetric(boolean v);
  
    // Inits the ECU
    boolean initEcu();
  
    // Retrieve a value out of the ECU's registry,
    // Pass in the msbAddr of the register, if this value is read from
    // a single register, pass in ECU_REGISTER_NULL for lsbAddr, otherwise
    // Pass in the LSB register address.  Then pass by ref your return value for returnValue
    // This method returns true if the read was successful, otherwise it returns false
    boolean getRegisterValue(byte msbAddr, byte lsbAddr, int *returnValue);
    
    // Validate our command byte error check
    boolean errorCheckCommandByte(byte commandByte, byte errorCheckByte);
    
    // Retrieve the ECU part number
    boolean getEcuPartNumber(char returnValue[12]);

    // Retrieve ECU error codes
    boolean getNumberOfErrorCodes(int *numberOfCodes);    
    boolean getErrorCode(int number, ConsultErrorCode *errorCode);
    
    // Clear out ECU error codes
    boolean clearErrorCodes();
        
    // Write byte to the ECU
    void writeEcu(byte v);
    
    // Read byte from the ECU,
    // Returns true if we read successfully,
    // Returns false if we failed to read
    // Pass in your return value by reference as param 1
    boolean readEcu(int *ecuByte);
    
    // Read byte from the ECU with non-standard timeout value
    // Returns true if we read successfully,
    // Returns false if we failed to read
    // Pass in your return value by reference as param 1
    // Pass in how many secs to wait for byte
    boolean readEcuWithTimeout(int *ecuByte, int timeout);
    
    // Tell the ECU to cancel its stream (shutup)
    void stopEcuStream();
    
    // Tell the ECU to start streaming register values
    boolean startEcuStream(ConsultRegister myRegisters[], int numRegisters);
    
    // Pull latest values out of ecu registers
    boolean readEcuStream(ConsultRegister myRegisters[], int numRegisters);
    
  private:
    boolean _isMetric;
    HardwareSerial *_consultSerial;
};
#endif

