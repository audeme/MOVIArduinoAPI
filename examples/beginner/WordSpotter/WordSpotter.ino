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

#ifdef ARDUINO_ARCH_AVR 
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR architecture, other boards need to use Serial1 
#endif

String KEYWORD="computer";        // The keyword to listen for
MOVI recognizer(true);            // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture


void setup()  
{

  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  // add the 'keywords' as sentences   
  recognizer.addSentence(KEYWORD); // Add keyword as sentence 1 
  
  // Add the top-50 most frequent English words as 'background model'.
  // This way, the keyword is not triggered by other random words.
  // See MOVI's user manual for a more detailed explanation on this.
  recognizer.addSentence("the be to of and a");
  recognizer.addSentence("in that have I it for");
  recognizer.addSentence("not on with he as you");
  recognizer.addSentence("do at this but his by");
  recognizer.addSentence("from they we say her she");
  recognizer.addSentence("or an will my one all would");
  recognizer.addSentence("there their what so up out");
  recognizer.addSentence("if about who get which go me");
  recognizer.train();                          // Train (may take 20seconds)
  //*/

  recognizer.welcomeMessage(false);             // silence MOVI's welcome message
  recognizer.responses(false);                  // silence MOVI's responses
  // recognizer.beeps(false);                      // silence MOVI's beeps -- we recommend to do that once you are familiar with this example.
  //  recognizer.setThreshold(5);		// uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
  recognizer.ask("Listening. I react to the word "+KEYWORD);     // The ask method speaks the passed string and then directly listens for a response (after beeps, but beeps are turned off). 
  KEYWORD.toUpperCase(); // make it uppercase (for comparison later)
  // By calling it in setup(), we make sure we don't need a call sign when entering loop()
}

void loop() // run over and over
{
  signed int res=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see User Manual)
  if (res==RAW_WORDS) { // The event raw_words let's us get the raw words via getResult() 
    if (recognizer.getResult().indexOf(KEYWORD)>=0) { // if the raw result string contains the (uppercase) keyword string: bingo!
        recognizer.ask("keyword spotted");     // say keyword has been spotted
    } else {
        recognizer.ask();     // silently listen again
    }
  }
  if (res==SILENCE) { // Silence doesn't produce raw_words, so we need to have a case distinction
        recognizer.ask();     // silently listen again
  }
}
