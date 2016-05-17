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
 * This basic example shows how to control MOVI(tm)'s synthesizer. In this 
 * example we use no speech recognition.
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
 *
 * If you long-press the button on the MOVI (for a couple seconds), 
 * MOVI will revert back to the call sign and sentences trained here.
 */

#include "MOVIShield.h"     // Include MOVI library, needs to be *before* the next #include

#ifdef ARDUINO_ARCH_AVR 
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR architecture, other boards need to use Serial1 
#endif

MOVI recognizer(true);            // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture

void setup()  
{
  recognizer.init();      // Initialize MOVI (waits for it to boot)
  recognizer.say("Hello, this is the default voice");
  delay(3000);
  recognizer.setVoiceGender(FEMALE_VOICE);
  recognizer.say("Hello, this is the female voice");
  delay(3000);
  recognizer.setVolume(50);
  recognizer.say("50 percent volume.");
  delay(3000);
  recognizer.setVolume(25);
  recognizer.say("25 percent volume.");
  delay(3000);
  recognizer.setVolume(0);
  recognizer.say("0 percent volume.");
}

void loop() // run over and over
{
  // do nothing.
}
