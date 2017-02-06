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
 * This example shows how to use MOVI for push-to-talk application. In other words,
 * MOVI will not listen to a call sign but listen and  recognize only while a button 
 * is pressed. This is is achieved by connecting a button (for the push) to the 
 * Arduino and using MOVI's new control commands pause() and finish() in connection 
 * with the button press.
 *
 * This example requires MOVI firmware 1.1!
 *
 * Circuitry:
 * Button between pin D2 and GND
 * Arduino UNO R3, MEGA2560 R3, or Arduino Leonardo R3.
 * Connect speaker to MOVI.
 * IMPORTANT: Always use external power supply with MOVI. 
 * 
 * Other Arduino-compatible boards:  
 * Consult MOVI's User Manual before connecting MOVI.
 *
 * For instructions on how to connect a button to Arduino, read this: http://www.instructables.com/id/Arduino-Button-Tutorial/
 *
 * How to use this example:
 * Connect the button, compile and upload this program, wait for sentences to be trained.
 * Once MOVI responds with the "Push to talk example" sentence, MOVI is ready.
 * Push the button and say "alpha here copy". Release the button.
 * MOVI responds with: "Copy. Listening to you, alpha."
 * Push the button and say "Alpha out". Release the button
 * MOVI responds with: "Roger. Alpha out."
 *
 * Optionally: uncomment the no beeps command to quiet MOVI's beeps. 
 *
 */

#include "MOVIShield.h"     // Include MOVI library, needs to be *before* the next #include

#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_PIC32)
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR and PIC32 architecture, other boards need to use Serial1 
#endif

#define BUTTON_PIN        2 // Button pin on D2
boolean button_was_pressed; // previous state

MOVI recognizer(true);      // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture
bool listening=false;

void setup()  
{
  recognizer.init();        // Initialize MOVI (waits for it to boot)

  if (recognizer.getFirmwareVersion()<1.1f) { // This example needs 1.1 firmware!
      recognizer.say("This example only works with firmware 1.1 or higher.");
      while (1) {} // loop endlessly (Sorry)
  }
 
  pinMode(BUTTON_PIN, INPUT);     // Make button port readable
  digitalWrite(BUTTON_PIN, HIGH); // pull-up
  button_was_pressed = false;
 
  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  recognizer.addSentence("alpha here copy");      // Add sentence 1
  recognizer.addSentence("alpha out");            // Add sentence 2
  recognizer.train();                             // Train (may take 20seconds) 
  //*/
  // recognizer.setThreshold(5);		  // uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
 
  recognizer.welcomeMessage(false); // turn off welcome message -- no callsign needed. 
  recognizer.say("Push To Talk Example. Push button and speak.");
  delay(1000);                                    // Finish speaking.
  listening=true;                                 // This is what MOVI does by default...
  // recognizer.beeps(false);                     // OPTIONAL: turns the beeping off
}

boolean handle_button()
{
  int button_pressed = !digitalRead(BUTTON_PIN); // pin low -> pressed
  return button_pressed;
}

void loop() // run over and over
{
  // check if the button is pressed. 
  boolean button_pressed = handle_button();
  
  if (button_pressed) {
    if (!listening) {   // start recognize if not already doing so! 
      // recognize!    
      recognizer.ask(); // Recognize! (ask without parameter, just puts MOVI into listening mode)
      listening=true;   // We are listening...
      delay(500);       // Adding delay to make the system more resistant to oscillating buttons 
    } 
  }
  
  if (!button_pressed) {
    // finish recognizing, if started:
    if (listening) {
      recognizer.finish(); // Force-finish the sentence (If something is recognized, we will get a result.)
      recognizer.pause();  // Pause recognizing again 
      listening=false;     // No more recognizing
      delay(500);          // Adding delay to make the system more resistant to oscillating buttons
    }
  }
  
  // Now check if anything came in as recognized...
  signed int res=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see docs)
  
  if (res==1) {                     // Sentence 1
    recognizer.say("Copy. Listening to you, alpha.");
  } 
  if (res==2) {                    // Sentence 2 
    recognizer.say("Roger. Alpha out."); 
  }
  
  // Do more ...
}
