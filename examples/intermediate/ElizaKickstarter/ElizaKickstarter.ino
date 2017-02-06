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
 * This example implements the Eliza demo as it was featured in MOVI's 
 * Kickstarter campaign: https://www.kickstarter.com/projects/310865303/movi-a-standalone-speech-recognizer-shield-for-ard
 *
 * It's not actually Eliza (see: https://en.wikipedia.org/wiki/ELIZA)
 * but it shows how to use MOVI in dialog mode and switch voices.
 * It also shows MOVI's capability of recognizing long sentences as well as
 * how to put these into flash memory so that Arduino UNO's memory does not
 * overflow.
 *
 * Original Kickstarter dialog:
 * Person: Computer -- I need a therapist.
 * MOVI: OK. I will find you one.
 * <wait>
 * MOVI: Hello, my name is Eliza. How may I help you?
 * Person: I am worried about the relationship with my girlfriend.
 * MOVI: What do you want to talk about?
 * Person: I really want to trust her but it seems I can't.
 * MOVI: Anything specific?
 * Person: Everytime I talk to her, it feels like a private conversation but then it isn't.
 * MOVI: Does that concern you? 
 * Person: Also, whenever I say something she doesn't even remember the sentence before that.
 * MOVI: I am interested. Tell me more.
 * Person: In fact, I really can't get her to learn anything!
 * MOVI: How does that make you feel?
 * Person: I mean I really like her otherwise. Her name is Siri, btw.
 * MOVI: Our time is up, we really need to stop...
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
bool elizaIsIn;

const String  elizaResponses[]={ // Eliza's responses
  "Talk to me.", 
  "What do you want to talk about?", 
  "Anything specific?", 
  "Does that concern you?", 
  "I am interested. Tell me more.", 
  "How does that make you feel?", 
  "Our time is up. We really need to stop!"
};

void setup() // setup Arduino and MOVI
{
  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  recognizer.callSign("COMPUTER"); // Train callsign Arduino (may take 20 seconds)

  // Arduino Uno and Leonardo have issues with a lot of long sentences, as this overflows the
  // capacity of the SRAM. Therefore, MOVI suports the F() function, for addsentence, say, ask, 
  // and password, which places the strings into Flash memory.
  // More information about the F() function can be found here: https://www.arduino.cc/en/Tutorial/Memory
  recognizer.addSentence(F("I need a therapist")); // Add sentence 1
  recognizer.addSentence(F("I am worried about the relationship with my girlfriend"));  // Add sentence 2
  recognizer.addSentence(F("I really want to trust her but it seems I cant"));  // Add sentence 3
  recognizer.addSentence(F("Everytime I talk to her it feels like a private conversation but then it isnt"));  // Add sentence 4
  recognizer.addSentence(F("Also whenever I say something she doesnt even remember the sentence before that"));  // Add sentence 5
  recognizer.addSentence(F("In fact I really cant get her to learn anything"));  // Add sentence 6
  recognizer.addSentence(F("I mean I really like her otherwise Her name is Siri by the way"));  // Add sentence 7
  recognizer.train();                           // Train (takes about 20seconds)
  //*/

  // recognizer.setThreshold(5);                 // uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
  recognizer.responses(false); // turn of automatic responses (so we can react).
  elizaIsIn=false; // Eliza is out
}

void loop() // run over and over
{
  int res=recognizer.poll();
  if (res==1 && elizaIsIn==false) {          // The user needs to ask for the therapist (see script and first sentence).
    recognizer.setVoiceGender(MALE_VOICE);   // Computer is male voice in this example.      
    recognizer.say(F("OK. I will find you one...")); // So MOVI will get Eliza.
    delay(5000);                             // Of course, we need some time to fetch Eliza...
    recognizer.setVoiceGender(FEMALE_VOICE); // Eliza is female voice        
    recognizer.ask(F("Hello, my name is Eliza. How may I help you?")); // The ask method says the string and then directly listens for a response (after beeps).
    res=0; // We finished processing this.
    elizaIsIn=true;          // Eliza is in.
  } 
  if (elizaIsIn) {
    if (res>0 && res<7) { // If it's a sentence and not an event, we respond and listen again.
      recognizer.ask(elizaResponses[res-1]); // Arrays start at 0, we start sentence counting at 1 (my CEO made me) -- so res-1.
    }
    if (res==UNKNOWN_SENTENCE) { // If we hit an unknown sentence, use a random response. So Eliza always responds.
      recognizer.ask(elizaResponses[random(0,6)]);
    }
    if (res==SILENCE) { // If nothing is said (e.g. noise only) then we ask again.
      recognizer.ask(F("Take your time."));
    }
    if (res==7) { // Sentence 7 means our 'time is up'. Eliza leaves.
      recognizer.say(elizaResponses[res-1]);
      elizaIsIn=false;
    }
  }
}


