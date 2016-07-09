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
 * Firmware 1.1 of MOVI allows using language models from VoxForge.org to switch
 * the recognized langauge from English to another language. So far, only German and 
 * Mexican Spanish seem to work.  
 * 
 * If you are interested in using MOVI for German or Mexican Spanish, please check
 * MOVI's documentation on how to install an alternative language pack.
 * 
 * Below example code is based on the basic lightswitch example. Please note: MOVI
 * does not support umlauts or accents (tildes) or any other special symbols.
 * You need to find the closest match with plain 7-bit ASCII symbols.  
 *
 * Circuitry:
 * LED is pin D13 and GND
 * Arduino UNO R3, MEGA2560 R3, or Arduino Leonardo R3.
 * Connect speaker to MOVI.
 * IMPORTANT: Always use external power supply with MOVI. 
 * 
 * Other Arduino-compatible boards:  
 * Consult MOVI's User Manual before connecting MOVI.
 *
 * If you long-press the button on the MOVI (for a couple seconds), 
 * MOVI will revert back to regular English operation.
 */

#include "MOVIShield.h"     // Include MOVI library, needs to be *before* the next #include

#ifdef ARDUINO_ARCH_AVR 
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR architecture, other boards need to use Serial1 
#endif

const int led = 13;
MOVI recognizer(true);      // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture

void setup()  
{
  pinMode(led, OUTPUT);    // Make LED port writeable
  digitalWrite(led, HIGH); // Blink LED.   
  delay(1);                   
  digitalWrite(led, LOW);  
  delay(1); 

  recognizer.init();      // Initialize MOVI (waits for it to boot)

  if (recognizer.getFirmwareVersion()<1.1f) { // This example needs 1.1 firmware!
      recognizer.say("This example only works with firmware 1.1 or higher and needs extra files on the SDCard.");
      while (1) {} // loop endlessly (Sorry)
  }

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  
  recognizer.callSign("ARDUINO"); // Train call sign ARDUINO (may take 20 seconds). Note: Call signs can only be in English!
  recognizer.addSentence("Haga luz");      // Add Spanish sentence 1
  recognizer.addSentence("Buneas Noches"); // Add Spanish sentence 2  
  recognizer.train();                      // Train (may take 20seconds) 
  //*/

  recognizer.setSynthesizer(SYNTH_PICO,"-l=es-ES"); // Switch the synthesizer to the new one and to German. 
  //  recognizer.setThreshold(5);		    // uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
}

void loop() // run over and over
{
  signed int res=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see docs)
  if (res==1) {                     // Sentence 1.
    digitalWrite(led, HIGH);        // Turn on LED
    recognizer.say("y hay luz"); // Respond a sentence in Spanish
  } 
  if (res==2) {                    // Sentence 2 
    digitalWrite(led, LOW);        // Turn off LED
    recognizer.say("Buenas Noches!"); // Respond a sentence in Spanish
  }
  // Do more ...
}
