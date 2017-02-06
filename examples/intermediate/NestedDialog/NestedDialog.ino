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
 * This example implements the a simple nested dialog 
 * as discussed on Audeme's forum under:
 * http://www.audeme.com/forum.html#/20160403/nested-voice-commands-do-something-movi-are-y-5201362/
 * Thanks to dmworking247 for this helpful question!
 *
 * This example shows how to use a context variable to nest the dialog questions. The context variable keeps 
 * track of where the dialog currently is. The presented method 'unnests' the questions and each chunk of 
 * code only deals with one specific response. Also, when the dialog system becomes more complex, one can easily 
 * create methods that each deal with one question/answer pair. 
 *
 * Dialog:
 * Make a cup of tea
 * I-- Are you Sure ? (Yes/No)
 * I---- How many lumps of sugar? (one/two)
 * Make a sound
 * I-- How many beeps? (one/two)
 *
 * Circuitry:
 * Arduino UNO R3, MEGA2560 R3, or Arduino Leonardo R3.
 * Connect speaker to MOVI.
 * IMPORTANT: Always use external power supply with MOVI. 
 *
 * Other Arduino-compatible boards:  
 * Consult MOVI's user manual before connecting MOVI.
 */

#include "MOVIShield.h"     // Include MOVI library, needs to be *before* the next #include

#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_PIC32)
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR and PIC32 architecture, other boards need to use Serial1 
#endif

MOVI recognizer(true);      // Get a MOVI object, true enables serial monitor interface, rx and tx for alternate communication pins on AVR architecture boards



// Context states:
const int BOOT=0;
const int AREYOUSURE=1; 
const int HOWMANYBEEPS=2;
const int HOWMANYSUGAR=3;
int context=BOOT;   // stores the context

bool okresponse=false; // this one makes things easier, see below

void setup()
{
  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.

  // Arduino Uno and Leonardo have issues with a lot of long sentences, as this overflows the
  // capacity of the SRAM. Therefore, MOVI suports the F() function, for addsentence, say, ask, 
  // and password, which places the strings into Flash memory.
  // More information about the F() function can be found here: https://www.arduino.cc/en/Tutorial/Memory
  recognizer.addSentence(F("make a cup of tea")); // Add sentence 1 
  recognizer.addSentence(F("make a sound")); // Add sentence 2 
  recognizer.addSentence(F("yes")); // Add sentence 3 
  recognizer.addSentence(F("no")); // Add sentence 4 
  recognizer.addSentence(F("one")); // Add sentence 5 
  recognizer.addSentence(F("two")); // Add sentence 6   
  recognizer.train();               // Train (takes about 20seconds)
  //*/

  // recognizer.setThreshold(5); // uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
  recognizer.responses(false); // turn of automatic responses (so we can react).
  
}

void loop() 
{
    int result=recognizer.poll(); // See if the recognizer got something
    if (context==BOOT) { // MOVI just started
        if (result==1) {
            recognizer.ask(F("Are you sure?"));
            context=AREYOUSURE;
        }
        if (result==2) {
            recognizer.ask(F("How many times should I beep?"));
            context=HOWMANYBEEPS;
        }
        if (result>2) {
            recognizer.say(F("Commands are: Make a coup of tea and Make a sound."));
        }
    }
    if (context==AREYOUSURE) {
        okresponse=false;
        if (result==3) { // yes
           context=HOWMANYSUGAR;
           okresponse=true;
        }
        if (result==4) { // no
           recognizer.say(F("Maybe next time"));
           okresponse=true;
           context=BOOT;
        }
        if ((result>0) && (okresponse==false)) { // Any other spoken response. 
           recognizer.say(F("Please respond yes or no"));
           recognizer.ask(F("Are you sure?"));
        }
    }
    if (context==HOWMANYSUGAR) {
        okresponse=false;
        if (result==5) { // one
           recognizer.say(F("one lump"));
           context=BOOT;
           okresponse=true;
        }
        if (result==6) { // two
           recognizer.say(F("two lumps"));
           okresponse=true;
           context=BOOT;
        }
        if ((result>0) && (okresponse==false)) { // Any other spoken response. 
           recognizer.say(F("Please respond one or two"));
           recognizer.ask(F("How many lumps of sugar?"));
        }
    }
    if (context==HOWMANYBEEPS) {
        okresponse=false;
        if (result==5) { // one
           recognizer.say(F("beep"));
           context=BOOT;
           okresponse=true;
        }
        if (result==6) { // two
           recognizer.say(F("beep beep"));
           okresponse=true;
           context=BOOT;
        }
        if ((result>0) && (okresponse==false)) { // Any other spoken response. 
           recognizer.say(F("Please respond one or two"));
           recognizer.ask(F("How many beeps?"));
        }
    }
}
