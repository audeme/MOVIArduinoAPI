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
 * Firmware 1.1 of MOVI allows playing back uncompressed sound files stored on 
 * the update partition of the SDcard. This is in response to diverse forum 
 * request by our users (thank you!). The sound files should be stored as .wav 
 * or .au files (Linux).
 *
 * This example uses three wav files, that can be downloaded from: http://soundbible.com/tags-bird.html
 * rooster.wav
 * falcon.wav
 * owl.wav
 *
 * Filenames are not case sensitive.
 *
 * NOTE: The only way to abort a playing sound file is to issue an "ABORT" command on the Serial Monitor.
 * Programmatically, the API offers recognizer.abort() (e.g. after a timeout or a buttonpress). 
 *
 * Circuitry:
 * Arduino UNO R3, MEGA2560 R3, or Arduino Leonardo R3.
 * Connect speaker to MOVI.
 * IMPORTANT: Always use external power supply with MOVI. 
 * 
 * Other Arduino-compatible boards:  
 * Consult MOVI's User Manual before connecting MOVI.
 *
 */

#include "MOVIShield.h"     // Include MOVI library, needs to be *before* the next #include

#ifdef ARDUINO_ARCH_AVR 
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR architecture, other boards need to use Serial1 
#endif

MOVI recognizer(true);            // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture

void setup()  
{
  recognizer.init();      // Initialize MOVI (waits for it to boot)

  if (recognizer.getFirmwareVersion()<1.1f) { // This example needs 1.1 firmware!
      recognizer.say("This example only works with firmware 1.1 or higher and needs extra files on the SDCard.");
      while (1) {} // loop endlessly (Sorry)
  }

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  recognizer.callSign("birdsounds"); // Train callsign Arduino (may take 20 seconds)
  recognizer.addSentence("rooster"); // Add sentence 1
  recognizer.addSentence("falcon");  // Add sentence 2
  recognizer.addSentence("owl");  // Add sentence 3
  recognizer.train();                // Train (may take 20seconds) 
  //*/

  //  recognizer.setThreshold(5);			// uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
}

void loop() // run over and over
{
  signed int res=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see docs)
  if (res==1) {                     // Sentence 1.
    recognizer.play("rooster.wav"); // Play rooster.wav
  } 
  if (res==2) {                    // Sentence 2.
    recognizer.play("falcon.wav"); // Play falcon.wav
  } 
  if (res==3) {                    // Sentence 3.
    recognizer.play("owl.wav");    // Play owl.wav
  } 
  // Do more ...
}
