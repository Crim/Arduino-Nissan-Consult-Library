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

#include "WProgram.h"
#include "Consult.h"

// -- Setup Methods --

// Default Constructor
Consult::Consult()
{
  _consultSerial = NULL;
}

void Consult::setMetric(boolean v)
{
  _isMetric = v;
}

// Set Serial Device
void Consult::setSerial(HardwareSerial *serial)
{
  // Set private datamember
  _consultSerial = serial;
  
  // Begin serial connection at 9600 baud
  _consultSerial->begin(9600);
}

// -- Core methods --

// Inits the ECU
boolean Consult::initEcu()
{
  // Byte we will read from ecu
  int ecuByte;
 
  // Verify we have set the Serial
  if (_consultSerial == NULL) {
    return false;
  }
  
  // First stop any streams
  stopEcuStream();
  
  // Make 2 attempts to init
  for (int x=0; x<2; x++) {
    // Send init sequence
    writeEcu(0xFF);
    writeEcu(0xFF);
    writeEcu(0xEF);
    
    // Read from ecu
    if (readEcuWithTimeout(&ecuByte, 2)) {
       if (ecuByte == 0x10) {
         // We initialized it!
       }
       else {
         // Already initialized
       }
       // Send stop command
       stopEcuStream();
       return true;
    }
    delay(250);
  }
  
  return false;
}

// Tell the ECU to cancel its stream (shutup)
void Consult::stopEcuStream()
{
  // Send stop stream
  writeEcu(ECU_COMMAND_STOP_STREAM);
  
  // Clear out buffer
  // Really we should watch for the stop stream command
  // Back from the ecu
  delay(100);
  _consultSerial->flush();
}

// Write byte to the ECU
void Consult::writeEcu(byte v)
{
  // Use serial write to send raw value
  // Otherwise Serial.print will just send ASCII values
  _consultSerial->write(v);
}

// Read byte from the ECU,
// Returns true if we read successfully,
// Returns false if we failed to read
// Pass in your return value by reference as param 1
boolean Consult::readEcu(int *ecuByte)
{
  // Times out after 1 sec
  return readEcuWithTimeout(ecuByte, 1);
}

// Read byte from the ECU with non-standard timeout value
// Returns true if we read successfully,
// Returns false if we failed to read
// Pass in your return value by reference as param 1
// Pass in how many secs to wait for byte
boolean Consult::readEcuWithTimeout(int *ecuByte, int timeout)
{
  unsigned long startTime = millis();
  
  // Loop until our timeout
  do {
    if (_consultSerial->available() > 0) {
      *ecuByte = _consultSerial->read();
      return true;
    }
    delay(5);
  } 
  while ((millis()-startTime) < (timeout*1000));
  return false;
}

// Retrieve a value out of the ECU's registry,
// Pass in the msbAddr of the register, if this value is read from
// a single register, pass in ECU_REGISTER_NULL for lsbAddr, otherwise
// Pass in the LSB register address.  Then pass by ref your return value for returnValue
// This method returns true if the read was successful, otherwise it returns false
boolean Consult::getRegisterValue(byte msbAddr, byte lsbAddr, int *returnValue)
{
  // Value that we read from ECU
  int lsb, msb, ecuByte, readCount = 0;
  
  // Send Register read command
  writeEcu(ECU_COMMAND_READ_REGISTER);
  
  // Send MSB of Register to read
  writeEcu(msbAddr);
  
  // If we passed in a 2nd addr to read
  if (lsbAddr != ECU_REGISTER_NULL) {
    // Send Register read command
    writeEcu(ECU_COMMAND_READ_REGISTER);
  
    // Send LSB of Register to read
    writeEcu(lsbAddr);
  }
  
  // Send command termination
  writeEcu(ECU_COMMAND_TERM);
  
  // Now read first byte, it should be our ECU_COMMAND_READ_REGISTER inverted
  readEcu(&ecuByte);
  
  // Validate that its correct
  if (!errorCheckCommandByte(ECU_COMMAND_READ_REGISTER, ecuByte)) {
    return false;
  }  
  
  // Read until we get FF over (start frame)
  // Really I should do better validation here
  // The ecu will send over The command inverted, then your msbAddr
  // If you sent over a LSB as well, it would then send the command inverted
  // again, along with your LSB
  do {
    // If we fail to read anything
    if (!readEcu(&ecuByte)) {
      // Just quit
      stopEcuStream();
      return false;
    }
    
    // Do a max of 4 reads
    readCount++;
    if (readCount > 4) {
      // Error, Failed to find start frame byte!
      stopEcuStream();
      return false;
    }
  }
  while (ecuByte != 0xFF);
  
  // Next value should be number of bytes we are going to read
  readEcu(&ecuByte);
    
  // Next byte should be the MSB
  readEcu(&ecuByte);
  *returnValue = ecuByte;
  
  if (lsbAddr != ECU_REGISTER_NULL) {
    // Shift it over 8 bytes
    *returnValue = *returnValue << 8;
    
    // Next byte should  be the LSB
    readEcu(&ecuByte);
    
    // Or to our return value
    *returnValue = *returnValue | (byte)ecuByte;
  }

  // Stop stream
  stopEcuStream();
  
  // Return success
  return true;
}

// Validate our command byte error check
boolean Consult::errorCheckCommandByte(byte commandByte, byte errorCheckByte) 
{
  if (commandByte != (byte)~errorCheckByte) {
    stopEcuStream();
    return false;
  }
  return true;
}

// Retrieve the ECU part number
boolean Consult::getEcuPartNumber(char returnValue[12])
{
  int ecuByte, curPos = 6;
  
  // Default data
  returnValue[0] = '2';
  returnValue[1] = '3';
  returnValue[2] = '7';
  returnValue[3] = '1';
  returnValue[4] = '0';
  returnValue[5] = '-';
  
  // Stop any previous streams
  stopEcuStream();
  
  // Send command  
  writeEcu(ECU_COMMAND_ECU_INFO);  // 11010000 
  writeEcu(ECU_COMMAND_TERM);      // 00101111  
  
  // First byte should be 0xD0 inverted
  readEcu(&ecuByte);
  if (!errorCheckCommandByte(ECU_COMMAND_ECU_INFO, ecuByte)) {
    return false;
  }
  
  // Then we should get a start frame byte
  readEcu(&ecuByte);
  
  // Then we should get 22 bytes of data
  for (int x=1; x<=22; x++) {
    readEcu(&ecuByte);
    if (x >= 19 && x <= 22) {
      returnValue[curPos] = (char)ecuByte;
      curPos++;
    }
  }
  
  // All terminator
  returnValue[curPos] = '\0';
  
  // Stop ecu stream
  stopEcuStream(); 
  return true;
}

// Retrieve ECU Codes
boolean Consult::getNumberOfErrorCodes(int *numberOfCodes)
{
   int numberOfBytes = 0, ecuByte;
  
  // Stop any previous streams
  stopEcuStream();
  
  // Send command  
  writeEcu(ECU_COMMAND_SELF_DIAG); 
  writeEcu(ECU_COMMAND_TERM); 
  
  // First byte should be 0xD1 inverted
  readEcu(&ecuByte);
  if (!errorCheckCommandByte(ECU_COMMAND_SELF_DIAG, ecuByte)) {
    return false;
  }
  
  // Then we should get a start frame byte
  readEcu(&ecuByte);
  
  // The next byte should be how many bytes the ECU is about to send over
  readEcu(&numberOfBytes);
  
  // Number of codes = number of bytes / 2
  *numberOfCodes = numberOfBytes / 2;
  return true;
}

boolean Consult::getErrorCode(int codeNumber, ConsultErrorCode *errorCode)
{
  int numberOfBytes = 0, ecuByte, ecuCode, ecuLastSeen;
  
  // Stop any previous streams
  stopEcuStream();
  
  // Send command  
  writeEcu(ECU_COMMAND_SELF_DIAG); 
  writeEcu(ECU_COMMAND_TERM); 
  
  // First byte should be 0xD1 inverted
  readEcu(&ecuByte);
  if (!errorCheckCommandByte(ECU_COMMAND_SELF_DIAG, ecuByte)) {
    return false;
  }
  
  // Then we should get a start frame byte
  readEcu(&ecuByte);
  
  // The next byte should be how many bytes the ECU is about to send over
  readEcu(&numberOfBytes);
  
  // Makee sure that they requested an appropriate number
  if (codeNumber > (numberOfBytes/2)) {
    // Requested invalid code
    stopEcuStream();
    return false;
  }

  for (int x=0; x<(numberOfBytes/2); x++) {
    // Read byte, this should be the error code
    readEcu(&ecuCode);
    
    // Read byte, this should how many starts since last seen
    readEcu(&ecuLastSeen);
    
    // If this is the one they wanted
    if (codeNumber == (numberOfBytes/2)) {
      errorCode->setCode(ecuCode);
      errorCode->setLastSeen(ecuLastSeen);
      
      // Stop ECU Stream and return
      stopEcuStream();
      return true;
    }
  }

  // We got to the end, stop the stream
  // Did not find the code number they requested, return false
  stopEcuStream();
  return false;
}

boolean Consult::clearErrorCodes()
{
  int ecuByte; 
  
  // Stop any previous streams
  stopEcuStream();
  
  // Send command  
  writeEcu(ECU_COMMAND_CLEAR_CODES); 
  writeEcu(ECU_COMMAND_TERM);
  
  // First byte should be 0xC1 inverted
  readEcu(&ecuByte);
  if (!errorCheckCommandByte(ECU_COMMAND_CLEAR_CODES, ecuByte)) {
    return false;
  }
  
  // Then stop the stream
  stopEcuStream();

  return true;  
}

// ECU Stream methods
// Instruct the ECU to start streaming our values
boolean Consult::startEcuStream(ConsultRegister myRegisters[], int numRegisters)
{
  // Byte read from ecu
  int ecuByte;
  
  // Loop thru each register and tell ECU we want it to stream it to us
  for (int x=0; x<numRegisters; x++) {
    // Send Register read command
    writeEcu(ECU_COMMAND_READ_REGISTER);
  
    // Send MSB of Register to read
    writeEcu(myRegisters[x].getMsbAddr());
  
    // If we have a 2nd address, request it
    if (myRegisters[x].getLsbAddr() != ECU_REGISTER_NULL) {
      // Send Register read command
      writeEcu(ECU_COMMAND_READ_REGISTER);
  
      // Send LSB of Register to read
      writeEcu(myRegisters[x].getLsbAddr());
    }
  }  
  // Send command termination
  writeEcu(ECU_COMMAND_TERM);
    
  // Loop thru each register and verify ecu got what we wanted
  for (int x=0; x<numRegisters; x++) {
    // Read firt byte, it should be our command inverted
    readEcu(&ecuByte);
    if (!errorCheckCommandByte(ECU_COMMAND_READ_REGISTER, ecuByte)) {
      // Didnt have proper check command
      return false;
    }
    
    // Next byte should be the address we wanted
    readEcu(&ecuByte);
    if (ecuByte != myRegisters[x].getMsbAddr()) {
      // Didnt have proper MSB addr
      return false;
    }
    
    // If we have a lower byte, check it
    if (myRegisters[x].getLsbAddr() != ECU_REGISTER_NULL) {
      // Read firt byte, it should be our command inverted
      readEcu(&ecuByte);
      if (!errorCheckCommandByte(ECU_COMMAND_READ_REGISTER, ecuByte)) {
        // Didnt have proper Check command
        return false;
      }
      
      // Next byte should be our LSB addr
      readEcu(&ecuByte);
      if (ecuByte != myRegisters[x].getLsbAddr()) {
        // Didnt have proper LSB
        return false;
      }
    }
  }

  // Good to go!
  return true;
}

boolean Consult::readEcuStream(ConsultRegister myRegisters[], int numRegisters)
{
  // Value read from ECU
  int ecuByte, readCount = 0, value;
  
  // Flush stream, 
  // Probably not the best way to do this, but I was worried about
  // The arduino not being able to keep up with the data feed from the ECU
  // and be reading stale data out of the UART buffer over time
  _consultSerial->flush();
  
  // Read until we hit start frame
  do {
    readEcu(&ecuByte);
    // Dont get stuck here forever, 
    // This should be a a safe max number of reads
    if (readCount > (numRegisters * 2) + 10) {
      // Failed to find start frame!
      return false;
    }
  }
  while (ecuByte != 0xFF);
  
  // Next value should be number of bytes the ecu is going to send
  // Probably should validate this some..
  readEcu(&ecuByte);
  
  // Ok we found our start frame, loop thru our registers and get the values
  for (int x=0; x<numRegisters; x++) {
    // Next byte should be the MSB
    readEcu(&ecuByte);
    value = ecuByte;
    
    // If we have a LSB defined
    if (myRegisters[x].getLsbAddr() != ECU_REGISTER_NULL) {
      // Shift it over 8 bytes
      value = value << 8;
      
      // Next byte should be the LSB
      readEcu(&ecuByte);
      
      // Or to our return value
      value = value | ecuByte;
    }
  
    // Convert value and save into our struct
    myRegisters[x].setValue(value);
  }
  
  return true;
}
