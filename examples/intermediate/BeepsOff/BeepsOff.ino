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
 * and instead use an LED on pin 13 to signalize MOVI is listening.
 * Many boards have a hardwired LED on board already connected to D13.
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

#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_PIC32)
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR and PIC32 architecture, other boards need to use Serial1 
#endif

const int led = 13;
MOVI recognizer(true);            // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture boards

void setup()  
{
  pinMode(led, OUTPUT);    // Make LED port writeable
  digitalWrite(led, HIGH); // Blink LED.   
  delay(1);                   
  digitalWrite(led, LOW);  
  delay(1); 

  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  recognizer.callSign("Arduino"); // Train callsign Arduino (may take 20 seconds)
  recognizer.addSentence("Are you listening"); // Add sentence 1
  recognizer.train();                          // Train (may take 20seconds) 
  //*

  recognizer.beeps(false);                      // turns the beeping off
  //  recognizer.setThreshold(5);			// uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
}

void loop() // run over and over
{
  signed int event=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see docs)
  if (event==BEGIN_LISTEN) { // This is returned at the start of listening
    digitalWrite(led, HIGH); // LED on.   
  }
  if (event==END_LISTEN) { // This is returned at the end of listening
    digitalWrite(led, LOW); // LED off 
  }
  if (event==1) {   // Sentence 1: Are you listening?
    recognizer.say("Yes, I am listening."); // Speak a sentence in response
  } 
}


