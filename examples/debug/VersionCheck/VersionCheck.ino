/****************************************************************************
 * This is an example for the use of Audeme's MOVI(tm) Voice Control Shield *
 * ----> http://www.audeme.com/MOVI/                                        *
 * This code is inspired and maintained by Audeme but open to change        *
 * and organic development on GITHUB:                                       *
 * ----> https://github.com/audeme/MOVIArduinoAPI                           *
 * Written by Gerald Friedland for Audeme LLC.                              *
 * Contact: fractor@audeme.com                                              *
 * BSD license, all text above must be included in any redistribution.      *
 ****************************************************************************
 *
 * This simple example let's MOVI say the current firmware and library version. 
 *
 * Circuitry:
 * Arduino UNO R3, MEGA2560 R3, or Arduino Leonardo R3.
 * Connect speaker to MOVI.
 * IMPORTANT: Always use external power supply with MOVI. 
 * 
 * Other Arduino-compatible boards:  
 * Consult MOVI's User Manual before connecting MOVI.
 */

#include "MOVIShield.h"     // Include MOVI library, needs to be *before* the next #include

#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_PIC32)
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR and PIC32 architecture, other boards need to use Serial1 
#endif

MOVI recognizer(true);            // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture

// Helper function that works across Arduino platforms that converts float to String
String float2string(float f, int places)
{
  int beforepoint=(int) f;
  int afterpoint=(f-beforepoint)*pow(10,places);
  return String(beforepoint)+"."+String(afterpoint);
}

void setup()  
{
  recognizer.init();      // Initialize MOVI (waits for it to boot)

  recognizer.say("MOHVEE's firmware is version "+float2string(recognizer.getFirmwareVersion(),1));
  recognizer.say("MOHVEE's Arduino library is version "+float2string(recognizer.getAPIVersion(),2));
  recognizer.say("MOHVEE's hardware is version "+float2string(recognizer.getHardwareVersion(),1)); 
}

void loop() // run over and over
{
    recognizer.poll(); // poll to enable debug console
}
