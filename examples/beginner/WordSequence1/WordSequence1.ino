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
 * This example shows the simple way of how to use MOVI to distinguish sequences
 * of the same words. An alternative way is explained in WordSequence2.
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
  
  recognizer.addSentence("one two three"); // Sentence 1
  recognizer.addSentence("one three two"); // Sentence 2
  recognizer.addSentence("two one three"); // Sentence 3
  recognizer.addSentence("two three one"); // Sentence 4
  recognizer.addSentence("three one two"); // Sentence 5
  recognizer.addSentence("three two one"); // Sentence 6
  recognizer.train();                          // Train (may take 20seconds)
  //*/

  //  recognizer.setThreshold(5);		// uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
}

void loop() // run over and over
{
  signed int res=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see User Manual)
  if (res==1) { // Sentence 1
    recognizer.say("I heard one two three."); 
  }
  if (res==2) { // Sentence 2
    recognizer.say("I heard one three two.");
  }
  if (res==3) { // Sentence 3
    recognizer.say("I heard two one three.");
  }
  if (res==4) { // Sentence 4
    recognizer.say("I heard two three one.");
  }
  if (res==5) { // Sentence 5
    recognizer.say("I heard three one two.");
  }
  if (res==6) { // Sentence 6
    recognizer.say("I heard three two one.");
  }
  // If sentence is not recognized, MOVI will say: "Sorry, I didn't understand that".
}
