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
 * This basic example shows how to control MOVI(tm)'s speech synthesizer. In this 
 * example we use no speech recognition. 
 *
 * As of MOVI firmware 1.1, two synthesizers are available: espeak and SVOX PICO.
 * Furthermore, both synthesizers can be configured via their command line 
 * parameters using MOVI's setSynthesizer() command.
 *
 * SVOX Pico is smoother than espeak but is only available as female voice and the only 
 * parameter to tune is the language spoken. See also: http://topics-of-interest.com/man1/pico2wave
 * The languages available are 'en-US', 'en-GB' (default), 'de-DE', 'es-ES', 'fr-FR', and 'it-IT'. 
 *
 * Espeak (the default synthesizer) allows for a lot more configuration. See:
 * http://espeak.sourceforge.net/commands.html 
 *
 * The code below shows several useful example. 
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

#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_PIC32)
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR and PIC32 architecture, other boards need to use Serial1 
#endif

MOVI recognizer(true);            // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture

void setup()  
{
  recognizer.init();      // Initialize MOVI (waits for it to boot)
  recognizer.say(F("Hello, this is the default voice"));
  delay(3000);
  
  recognizer.setVoiceGender(FEMALE_VOICE);
  recognizer.say(F("Hello, this is the female voice"));
  delay(3000);
  
  if (recognizer.getFirmwareVersion()>=1.1f) { // Firmware 1.1 has more features!
    
    recognizer.setSynthesizer(SYNTH_ESPEAK,"-v+whisper");
    recognizer.say(F("Do not tell anybody that MOVI can whisper now!"));
    delay(3000);
    
    recognizer.setSynthesizer(SYNTH_ESPEAK,"-v+croak");
    recognizer.say(F("MOVI can creep you out too!"));
    delay(3000);
    
    recognizer.setSynthesizer(SYNTH_ESPEAK,"-p85 -vf4");
    recognizer.say(F("Or speak like a child!"));
    delay(3000);
    
    recognizer.setSynthesizer(SYNTH_ESPEAK,"-s 300");
    recognizer.say(F("or extremely fast: 1 2 3 4 5..."));
    delay(3500);
    
    recognizer.setSynthesizer(SYNTH_ESPEAK,"-s 80");
    recognizer.say(F("or very slow: 1 2 3 4 5..."));
    delay(8000);
    
    recognizer.setSynthesizer(SYNTH_PICO);
    recognizer.say(F("As of firmware 1.1, I am the additional female voice."));
    delay(3000);
    
    recognizer.setSynthesizer(SYNTH_PICO,"-l=en-US");
    recognizer.say(F("Fancy an American accent?"));
    delay(3000);
    
    recognizer.setSynthesizer(SYNTH_PICO,"-l=es-ES");
    recognizer.say(F("Y puedo hablar castellano."));
    delay(4000);
    
    recognizer.setSynthesizer(SYNTH_ESPEAK);
  }  
  
  recognizer.setVolume(50);
  recognizer.say(F("50 percent volume."));
  delay(3000);
  recognizer.setVolume(25);
  recognizer.say(F("25 percent volume."));
  delay(3000);
  recognizer.setVolume(0);
  recognizer.say(F("0 percent volume."));
}

void loop() // run over and over
{
 // do nothing
}
