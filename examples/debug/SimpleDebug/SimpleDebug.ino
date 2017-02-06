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
 * This example shows how to use MOVI to listen in to random words without callsign.
 * Note that MOVI is designed for sentences and therefore whole sentences need to be 
 * uttered each time. 
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


void setup()  
{

  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  // add the 'keywords' as sentences   
  
  recognizer.addSentence("This is the first sentence"); // sentence one
  recognizer.addSentence("This is the second sentence"); // sentence two
  recognizer.addSentence("This is the third sentence"); // sentence three
  recognizer.train();                          // Train (may take 20seconds)
  //*/

  // Uncomment the following two commands for even more debugging! 
  // WARNING: MICDEBUG stays on until it's turned off and the recognizer is restarted or another program is loaded and the MOVI board is restarted.
  //recognizer.sendCommand("MICDEBUG","ON"); // To undo: uncomment both lines again and change this one to recognizer.sendCommand("MICDEBUG","OFF"); (or load another sketch and reset MOVI)
  //recognizer.sendCommand("RESTART","");
  
  // recognizer.setThreshold(5);           // uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
}

void loop() // run over and over
{
  signed int res=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see User Manual)
  if (res==RAW_WORDS) { // The event raw_words let's us get the raw words via getResult() 
    recognizer.say("I heard "+recognizer.getResult());
  }
  if (res==1) { 
    recognizer.say("It matches sentence 1.");
  }
  if (res==2) { 
    recognizer.say("It matches sentence 2.");
  }
  if (res==3) { 
    recognizer.say("It matches sentence 3.");
  }
}
