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
 * This example shows how to use MOVI's API to turn off the beeping
 * and instead respond with "Yes, Sir" to signalize MOVI is listening.
 * This is achieved by training a soft call sign. A soft call sign is defined 
 * as a sentence (or multiple for multiple call signs) and compared against a 
 * background model (see wordspotter example). Other sentences are then handled
 * normally, if and only if, the soft call sign was detected first.
 *
 * The example uses "Jeeves" as a soft call sign and simulates the famous butler.
 * The only two real sentences to recognize are "Are you listening?" and "No beeps here.".
 *
 * One caveat:
 * Since soft call signs are recognized through the Arduino interface and not directly 
 * on the MOVI shield, expect a soft call sign to react a little more slowly (about 1 sec) 
 * than a call sign defined by the CALLSIGN command.
 * 
 *
 * Circuitry:
 * LED is pin D13 and GND
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

MOVI recognizer(true);            // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture boards
boolean listening=false;          // Since we are using our own callsign, we only want to listen if 

void setup()  
{
  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  
  // Soft call sign.
  recognizer.addSentence("Jeeves"); // Sentence 1 will serve as a call sign
  
  // Add the top-19 most frequent English words as 'background model'.
  // This way, the 'callsign' is not triggered by other random words.
  // See MOVI's user manual for a more detailed explanation on this. 
  // In experimenting with one word per sentences vs multiple words in one
  // sentence (as in the wordspotter example), it turned out, this one works
  // better for this example. 
  
  recognizer.addSentence("the"); // #2
  recognizer.addSentence("be");  // #3
  recognizer.addSentence("to");  // #4
  recognizer.addSentence("of");  // #5
  recognizer.addSentence("a");   // #6
  recognizer.addSentence("in");  // #7
  recognizer.addSentence("that"); // #8
  recognizer.addSentence("have"); // #9
  recognizer.addSentence("I");    // #10
  recognizer.addSentence("it");   // #11
  recognizer.addSentence("for");  // #12
  recognizer.addSentence("not");  // #13
  recognizer.addSentence("on");   // #14
  recognizer.addSentence("with"); // #15
  recognizer.addSentence("he");   // #16
  recognizer.addSentence("as");   // #17
  recognizer.addSentence("you");  // #18
  recognizer.addSentence("do");   // #19
  recognizer.addSentence("at");   // #20
  
  // Real sentences
  recognizer.addSentence("Are you listening"); // Add sentence 1 (as sentence 21)
  recognizer.addSentence("No beeps here");     // Add sentence 2 (as sentence 22)
  
  recognizer.train();                          // Train (may take 20seconds) 
  //*

  // Reconfigure MOVI
  recognizer.beeps(false);                      // turns the beeping off
  recognizer.welcomeMessage(false);             // turns off MOVI welcome message
  recognizer.responses(false);                  // turns of MOVI's responses
  recognizer.ask();                             // Skip waiting for MOVI's callsign
  
  // Say a welcome message
  recognizer.say("I am ready, sir. Just call me Jeeves.");
  
  //  recognizer.setThreshold(5);			// uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
}

void loop() // run over and over
{
  signed int event=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see docs)
  if (event==1) { // This is our self-programmed call sign
    if (!listening) {
       recognizer.say("Yes, Sir");  // if it's the first time, respond
       listening=true;              // Now we are listening!
    }
    recognizer.ask();  // tell MOVI to listen (since we don't want MOVI to use it's actual call sign)
  }
  if (!listening && (event>1 || event==SILENCE || event==UNKNOWN_SENTENCE)) { // If it's not the call sign and we are not listening
    recognizer.ask();          // silently ignore what was said, and tell MOVI to listen again
  }
  if (listening) {
    if (event==21) {   // Sentence 1: Are you listening?
      recognizer.ask("Indeed, Sir");
      listening=false;   // We are only going to listen for one sentence before the next call sign. (Just like MOVI's original behavior).
    } 
    if (event==22) {   // Sentence 2: No beeps here.
      recognizer.ask("Precisely, Sir");
      listening=false;   // We are only going to listen for one sentence before the next call sign. (Just like MOVI's original behavior).
    }
    if (event==SILENCE || event==UNKNOWN_SENTENCE) { // If we are listening but it's nothing we understand, be polite
      recognizer.ask("I am sorry, Sir, I could not understand you very well.");
      listening=false;   // Just like MOVI's original behavior. However, arguably, in this case we could directly listen for the next sentence by commenting this assignment out.
    }
  }
}


