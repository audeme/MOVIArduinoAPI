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
 * This example shows how to use MOVI as a keyword spotter by 'abusing' the callsign. 
 * This is easier but requires the finish() command that is available as of MOVI firmware 1.1.
 * Also, only one keyword can be defined and it can only be English. Keyword used should adhere to
 * the standards for call signs, outlined in the user manual e.g., need to be longer words.
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

String KEYWORD="computer";    // The keyword to listen for
MOVI recognizer(true);        // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture

void setup()  
{

  recognizer.init();      // Initialize MOVI (waits for it to boot)
  
  if (recognizer.getFirmwareVersion()<1.1f) { // This example needs 1.1 firmware!
      recognizer.say("This example only works with firmware 1.1 or higher.");
      while (1) {} // loop endlessly (Sorry)
  }
  
  recognizer.callSign(KEYWORD);    // Add keyword as callsign

  recognizer.welcomeMessage(false);             // silence MOVI's welcome message
  recognizer.responses(false);                  // silence MOVI's responses
  recognizer.beeps(false);                      // silence MOVI's beeps -- we recommend to comment that out for debugging.
  //  recognizer.setThreshold(5);		// uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
  recognizer.say("Listening. I react to the word "+KEYWORD+".");     // Tell the user what's going on (after beeps, but beeps are turned off). 
}

void loop() // run over and over
{
  signed int res=recognizer.poll();     // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see User Manual). Some of the events are defined as constants.
  if (res==BEGIN_LISTEN) {              // The event BEGIN_LISTEN means that MOVI has spotted the callsign and is listening. 
    recognizer.finish();                // Immediately abort the actual listening for a sentence.
    recognizer.say("Keyword spotted."); // Give feedback.    
  }
}
