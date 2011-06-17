/**
  Arduino Consult Example
  June 12th 2011 - Stephen Powis (edrsp@mindspring.com)
  
  Note:
    This example makes the assumption you have an LCD panel hooked up
    so we can see whats going on.  Another option is to setup a SoftSerial
    (or real serial if you have an Arduino with multiple UARTs) and log msgs
    back to your computer.
    
  Licensed under the GPL V2
  Feel free to use this code for whatever you want,
  all that I ask that if you do use it, shoot me an email and
  let me know what for!  Its always cool to hear when people
  find your code useful.
**/

#include "Consult.h"
#include "ConsultConversionFunctions.h"
#include "ConsultRegister.h"
#include "ConsultErrorCode.h"

// Define global myConsult object
Consult myConsult = Consult();

// This example uses an LCD to display Information
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // Tell Consult which Serial object to use to talk to the ECU
  myConsult.setSerial(&Serial);
  
  // We want MPH and Farenheit, so pass in false
  // If you want KPH or Celcius, pass in true
  myConsult.setMetric(false);
  
  // Some output on the LCD
  lcd.begin(16, 3);
  lcd.home();
  lcd.print("Arduino Consult");
  lcd.setCursor(0,1);
  lcd.print("Starting...");
  lcd.setCursor(0,2);
}

void loop() {
  // Attempt to initialize consult <---> ecu conversation
  // Keep re-trying until it initializes
  while (myConsult.initEcu() == false) {
    // Failed to connect
    delay(1000);
    
    // Output to LCD
    lcd.clear();
    lcd.home();
    lcd.print("Failed to connect!");
  }
  
  // Output to LCD
  lcd.clear();
  lcd.home();
  lcd.print("Connected to ECU!");
  lcd.setCursor(0,1);
  
  // Try to pull ECU Part Number, 11 chars + 1 null terminator
  char partNumber[12];
  if (myConsult.getEcuPartNumber(partNumber)) {
    // Pulled ECU PartNumber, it is now stored 
    // in partNumber variable!
    // Output to LCD
    lcd.print(partNumber);
  }
  else {
    // Failed to retrieve ECU Part Number
    // Output to LCD
    lcd.print("Failed to get partnum");
  }
  
  // Small Pause
  delay(2000);
  
  // Clear our LCD
  lcd.clear();
  lcd.home();
    
  // Attempt to read Coolant Temp,
  // Coolant temp only spans a single register, pass in ECU_REGISTER_NULL for the LSB register address
  int coolantTemp;
  if (myConsult.getRegisterValue(ECU_REGISTER_COOLANT_TEMP, ECU_REGISTER_NULL, &coolantTemp)) {
    // coolantTemp now contains the temp, but we need to convert it to something human readable
    coolantTemp = ConsultConversionFunctions::convertCoolantTemp(coolantTemp);
    
    // Output to lcd
    lcd.print("Coolant:");
    lcd.print(coolantTemp);
  }
  else {
    // Failed to read coolant temp
    // output to LCD
    lcd.print("Failed read coolant");
  }
    
  // Attempt to read Tachometer,
  // This register spans two registers, first pass in MSB, then LSB
  int tach;
  if (myConsult.getRegisterValue(ECU_REGISTER_TACH_MSB, ECU_REGISTER_TACH_LSB, &tach)) {
    // tach now contains the tach value, but we need to convert it to something human readable
    tach = ConsultConversionFunctions::convertTachometer(tach);
    
    // Output to LCD
    lcd.setCursor(0,1);
    lcd.print("Tach:");
    lcd.print(tach);
  }
  else {
    // Failed to read tach
    // Output to LCD
    lcd.setCursor(0,1);
    lcd.print("Failed read tach");
  }
  
  // Small pause
  delay(2000);
  lcd.clear();
    
  // Pull error codes from ECU, first see how many codes the ecu has listed
  // Ideally we'd just make a call .getErrorCodes() that would return an array
  // of all the error codes it found..but we do it as follows instead to avoid 
  // allocating dynamic memory on the arduino
  int numberOfErrorCodes = 0;
  ConsultErrorCode errorCode = ConsultErrorCode();
  if (myConsult.getNumberOfErrorCodes(&numberOfErrorCodes)) {
    // Output how many error codes our ECU has stored
    lcd.home();
    lcd.print("Num Codes:"); 
    lcd.print(numberOfErrorCodes);
    
    // Then retrieve each code and display
    for (int x=1; x<=numberOfErrorCodes; x++) {
      // Display which code number
      lcd.setCursor(0,1);
      lcd.print("Code #"); lcd.print(x);
      
      // Attempt to pull code
      if (myConsult.getErrorCode(x, &errorCode)) {
        // Got our error code
        errorCode.getCode();
        errorCode.getLastSeen();
        
        // Display to LCD
        lcd.setCursor(0,2);
        lcd.print(errorCode.getCode(), HEX); lcd.print(" "); lcd.print(errorCode.getLastSeen(), HEX);
        lcd.print("       ");
        delay(2000);
      }
      else {
        // Failed to find error code?
        lcd.setCursor(0,2);
        lcd.print("Failed to get error code?");
      }
    }
  }
  else {
    // Failed to make call?
    lcd.setCursor(0,1);
    lcd.print("Failed to retrieve number of codes");
  }
  
  // Clear LCD
  lcd.clear();
  lcd.home();
  
  // Now attempt to clear codes
  if (myConsult.clearErrorCodes()) {
    // Cleared codes
    lcd.print("Cleared Codes!");
  }
  else {
    // Failed to clear codes
    lcd.print("Failed to clear codes!");
  }
  
  // Small Pause
  delay(2000);
  lcd.clear();
  lcd.home();
  
  // Create an Array of ConsultRegisters we want to read
  int numRegisters = 5;
  ConsultRegister myRegisters[numRegisters];
  
  // Create a register classes for each register type
  // Pass in a label to describe it, the register Addresses, and a function pointer to the function that
  // will convert the value from the ECU value to something human readable
  myRegisters[0] = ConsultRegister("Coolant", ECU_REGISTER_COOLANT_TEMP, ECU_REGISTER_NULL, &ConsultConversionFunctions::convertCoolantTemp);
  myRegisters[1] = ConsultRegister("Timing", ECU_REGISTER_IGNITION_TIMING, ECU_REGISTER_NULL, &ConsultConversionFunctions::convertIgnitionTiming);
  myRegisters[2] = ConsultRegister("Battery", ECU_REGISTER_BATTERY_VOLTAGE, ECU_REGISTER_NULL, &ConsultConversionFunctions::convertBatteryVoltage);
  myRegisters[3] = ConsultRegister("Speed", ECU_REGISTER_VEHICLE_SPEED, ECU_REGISTER_NULL, &ConsultConversionFunctions::convertVehicleSpeed);
  myRegisters[4] = ConsultRegister("Tach", ECU_REGISTER_TACH_MSB, ECU_REGISTER_TACH_LSB, &ConsultConversionFunctions::convertTachometer);

  // Now that we've defined these, we want to continuely poll the ecu for these values
  if (myConsult.startEcuStream(myRegisters, numRegisters)) {
    // Read registers 3 times from the ecu
    for (int x=0; x<3; x++) {
      // Read ecu stream
      if (myConsult.readEcuStream(myRegisters, numRegisters)) {
        // Loop thru values
        for (int y=0; y<numRegisters; y++) {
           // Has each registers updated value
          myRegisters[y].getValue();
          
          // Display updated value to LCD
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(myRegisters[y].getLabel());
          lcd.setCursor(0,1);
          lcd.print(myRegisters[y].getValue());
          
          // Small pause          
          delay(500);
        }
      }
      else {
        // Error, failed to read stream
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Failed to read stream!");
      }
    }
    lcd.setCursor(0,2);
    lcd.print("Stopping stream");
    // Stop ecu stream
    myConsult.stopEcuStream();
  }
  else {
    // Failed to register streaming
    lcd.print("Failed to register stream!");
  }
  
  // Small pause
  delay(500);
  lcd.clear();
  lcd.home();
}
