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
 * This example sketch shows how to count the words MOVI heard.  One can get 
 * the bare words recognized (and visible in the Serial Monitor) by catching 
 * the RAW_WORDS event and then using the method getResult(). 
 * Then, we just count the words. The event SILENCE tells us no words. 
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

#ifdef ARDUINO_ARCH_AVR 
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR architecture, other boards need to use Serial1 
#endif

MOVI recognizer(true);      // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture

void setup()  
{

  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  recognizer.callSign("Arduino");
  recognizer.addSentence("ONE TWO THREE FOUR FIVE SIX SEVEN EIGHT NINE TEN"); // Sentence 1 -- but doesn't matter, we just need some vocabulary to test this with
  recognizer.train();                   // Train (may take a couple seconds)
  //*/
  recognizer.responses(false);          // silence MOVI's responses, so we don't hear any from MOVI about sentences not being recognized...
  //  recognizer.setThreshold(5);	// uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
}

void loop() // run over and over
{
  signed int res=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, 
                                    // negative values denote events (see User Manual)
  if (res==RAW_WORDS) {             // The event raw_words let's us get the words via getResult()
    String result=recognizer.getResult();  // Get the result and save it in a string
    int numwords=1;                        // If we end up here, we have at least one word
    for (int i=0;i<result.length();i++)  { // Go through the result string...
      if (result[i]==' ') numwords++;         // ... and count spaces (= words).
    }
    recognizer.say("I heard "+String(numwords)+" words.");  // Say how many words we have.
  }  
  if (res==SILENCE) {               // The event SILENCE means there were no words
    recognizer.say("I heard 0 words.");  // Say we have 0 words.
  }  
}
