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
 * This example shows the another way of how to use MOVI to distinguish sequences
 * of the same words. In this example, only one sentence is trained with the three 
 * different words. Then getResult() is used to extract and parse the individual 
 * words. This is provided as a simple example on how to process results from MOVI 
 * directly using getResult(). A simpler way that is easier to implement but less 
 * flexible is explained in WordSequence1.
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

MOVI recognizer(true);            // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture

String KEYWORD1="ONE";
String KEYWORD2="TWO";
String KEYWORD3="THREE";
String TRAINSENTENCE=KEYWORD1+" "+KEYWORD2+" "+KEYWORD3;

void setup()  
{

  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  recognizer.addSentence(TRAINSENTENCE); // Sentence 1 -- but doesn't matter
  recognizer.train();                    // Train (may take a couple seconds)
  //*/

  //  recognizer.setThreshold(5);	// uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
}

void loop() // run over and over
{
  signed int res=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see User Manual)
  if (res==RAW_WORDS) { // The event raw_words let's us get the raw words via getResult()
    String result=recognizer.getResult();
    int pos1=result.indexOf(KEYWORD1); // search for word 1
    int pos2=result.indexOf(KEYWORD2); // search for word 2
    int pos3=result.indexOf(KEYWORD3); // search for word 3
    
    if (pos1<0 || pos2<0 || pos3<0) { // then one word is missing
      recognizer.say("Sorry, I didn't understand you.");
    } else if ((result.length()-1)!=TRAINSENTENCE.length()) { // then at least one word too many (-1 for '\n')
      Serial.println(TRAINSENTENCE.length());
      recognizer.say("Sorry, I didn't understand you.");
    } else {                          // exactly all 3 words are there
      recognizer.say("I heard "+recognizer.getResult());    
    }
  }     
}
