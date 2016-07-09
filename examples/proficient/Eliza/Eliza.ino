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
 * This example implements a simple version of Eliza 
 * (see: https://en.wikipedia.org/wiki/ELIZA )
 * It shows how to use MOVI in dialog mode and switch voices.
 * It also shows MOVI's capability of recognizing long sentences as well as
 * how to put a large amount of sentences into flash memory so that Arduino UNO's 
 * memory does not overflow.
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

#ifdef ARDUINO_ARCH_AVR 
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR architecture, other boards need to use Serial1 
#include <avr/pgmspace.h>
#endif

MOVI recognizer(true);      // Get a MOVI object, true enables serial monitor interface, rx and tx for alternate communication pins on AVR architecture boards

// Eliza general responses
const char generalResponses_0[] PROGMEM = "What else can you tell me?";
const char generalResponses_1[] PROGMEM = "Can you tell me more?";
const char generalResponses_2[] PROGMEM = "Now what?";
const char generalResponses_3[] PROGMEM = "Is there more I can hear about?";
const char generalResponses_4[] PROGMEM = "How does that make you feel?";
const char generalResponses_5[] PROGMEM = "And this is making you think...?";
const char generalResponses_6[] PROGMEM = "Now what are you planning to do?";
const char generalResponses_7[] PROGMEM = "Can you tell me your next step?";
const char generalResponses_8[] PROGMEM = "I like listening to you.";

const char* const generalResponses[] PROGMEM = {
  generalResponses_0, generalResponses_1, generalResponses_2, generalResponses_3, generalResponses_4, 
  generalResponses_5, generalResponses_6,generalResponses_7, generalResponses_8};

// Eliza direct responses 
const char directResponses_0[] PROGMEM = "Hi. Nice to see you!";
const char directResponses_1[] PROGMEM = "I'm really new. I was just launched as a product.";
const char directResponses_2[] PROGMEM = "My name is Eliza.";
const char directResponses_3[] PROGMEM = "You are welcome.";
const char directResponses_4[] PROGMEM = "I live on top of an Arduino.";
const char directResponses_5[] PROGMEM = "I am Eliza. I am a chatbot implemented in mohvee.";
const char directResponses_6[] PROGMEM = "I'm doing pretty good. How are you?";
const char directResponses_7[] PROGMEM = "Sorry, I am bad with names and faces.";
const char directResponses_8[] PROGMEM = "I whish I could see.";
const char directResponses_9[] PROGMEM = "Fourtytwo, I heard.";
const char directResponses_10[] PROGMEM = "Electrons.";
const char directResponses_11[] PROGMEM = "That depends on my synthesizer settings.";
const char directResponses_12[] PROGMEM = "I need to ask Joe about it.";
const char directResponses_13[] PROGMEM = "Time for me to get updated with a realtime clock.";
const char directResponses_14[] PROGMEM = "I am not good with math.";
const char directResponses_15[] PROGMEM = "Bertrand and Gerald";
const char directResponses_16[] PROGMEM = "Bye bye. See you later.";

const char* const directResponses[] PROGMEM = {
  directResponses_0, directResponses_0, directResponses_0, directResponses_0, directResponses_1, directResponses_1, directResponses_2, 
  directResponses_3, directResponses_4, directResponses_5, directResponses_6, directResponses_7, directResponses_8,
  directResponses_9, directResponses_10, directResponses_11, directResponses_12, directResponses_13, directResponses_14,
  directResponses_15, directResponses_16};

// Eliza conversation starters
const char conversationStarters_0[] PROGMEM = "If you wrote a book, what would you name the main character and where would he go?";
const char conversationStarters_1[] PROGMEM = "What is your favorite thing to do?"; 
const char conversationStarters_2[] PROGMEM = "If you could design a t-shirt, what would you draw or write on it?";
const char conversationStarters_3[] PROGMEM = "What was your favorite part of your day?";
const char conversationStarters_4[] PROGMEM = "What’s the funniest thing you saw this week?";
const char conversationStarters_5[] PROGMEM = "How would you describe a perfect day for you?";
const char conversationStarters_6[] PROGMEM = "If we had an airplane to take us on vacation right now, where would you want to go?";
const char conversationStarters_7[] PROGMEM = "If we could go to your favorite place but couldn’t use a car to get there, how do you think we could get there?";
const char conversationStarters_8[] PROGMEM = "If you could have any animal in the world as a pet, where would you go to get it and what would it be?";
const char conversationStarters_9[] PROGMEM = "If you could do something just like your friend what would you do?";
const char conversationStarters_10[] PROGMEM = "If you could do something like your Dad or Mom do, what would you want to be able to do?";
const char conversationStarters_11[] PROGMEM = "If you could change anything about school what would it be?";
const char conversationStarters_12[] PROGMEM = "If you could change anything about our family what would it be?";
const char conversationStarters_13[] PROGMEM = "If you could change anything in the world, what would you change and how?";
const char conversationStarters_14[] PROGMEM = "Who is your favorite story character and what do you like about him or her?";
const char conversationStarters_15[] PROGMEM = "If you could eat lunch with your favorite actor where would you go and what would you eat together?";
const char conversationStarters_16[] PROGMEM = "If you could wake up tomorrow with a superpower, what superpower would you want to have?";
const char conversationStarters_17[] PROGMEM = "If you could have any 3 wishes granted, what would they be?";
const char conversationStarters_18[] PROGMEM = "If all your clothes could only be one color, what color would you choose?";
const char conversationStarters_19[] PROGMEM = "If you could change the lunch menu at home what would you change?";
const char conversationStarters_20[] PROGMEM = "What is your biggest dream?";
const char conversationStarters_21[] PROGMEM = "What is your biggest worry?";
const char conversationStarters_22[] PROGMEM = "If you could change your name, would you want to and what name would you choose instead?";
const char conversationStarters_23[] PROGMEM = "What are you proudest of in your life?";
const char conversationStarters_24[] PROGMEM = "Where is your favorite place to be?";
const char conversationStarters_25[] PROGMEM = "What is something you never thought you could tell me but maybe want to tell me now?";

const char* const conversationStarters[] PROGMEM = {
conversationStarters_0, conversationStarters_1, conversationStarters_2, conversationStarters_3, 
conversationStarters_4, conversationStarters_5, conversationStarters_6, conversationStarters_7,
conversationStarters_8, conversationStarters_9, conversationStarters_10, conversationStarters_11, 
conversationStarters_12, conversationStarters_13, conversationStarters_14, conversationStarters_15,
conversationStarters_16, conversationStarters_17, conversationStarters_18, conversationStarters_19, 
conversationStarters_20, conversationStarters_21, conversationStarters_22, conversationStarters_23,
conversationStarters_24, conversationStarters_25};

char buffer[255];

void setup() // setup Arduino and MOVI
{
  recognizer.init();      // Initialize MOVI (waits for it to boot)
  
  //*
  recognizer.callSign("ELIZA"); // Train callsign Arduino (may take 20 seconds)

  // More information about the F() function can be found here: https://www.arduino.cc/en/Tutorial/Memory
  recognizer.addSentence(F("Hi"));                   // Add sentence 1
  recognizer.addSentence(F("Hello"));                // Add sentence 2
  recognizer.addSentence(F("Hi Eliza"));             // Add sentence 3
  recognizer.addSentence(F("Hello Eliza"));          // Add sentence 4
  recognizer.addSentence(F("How old are you"));      // Add sentence 5
  recognizer.addSentence(F("Whats your age"));       // Add sentence 6
  recognizer.addSentence(F("Whats your name"));      // Add sentence 7
  recognizer.addSentence(F("Thank you"));            // Add sentence 8
  recognizer.addSentence(F("Where do you live"));    // Add sentence 9
  recognizer.addSentence(F("What are you"));         // Add sentence 10
  recognizer.addSentence(F("How are you"));          // Add sentence 11
  recognizer.addSentence(F("Whats my name"));        // Add sentence 12
  recognizer.addSentence(F("Whats your favorite color"));  // Add sentence 13
  recognizer.addSentence(F("Whats the meaning of life"));  // Add sentence 14
  recognizer.addSentence(F("Whats your favorite food"));   // Add sentence 15
  recognizer.addSentence(F("Are you a girl or a boy"));    // Add sentence 16
  recognizer.addSentence(F("Whats your last name"));       // Add sentence 17
  recognizer.addSentence(F("What time is it"));            // Add sentence 18
  recognizer.addSentence(F("Whats two plus two"));         // Add sentence 19
  recognizer.addSentence(F("Who made you"));               // Add sentence 20
  recognizer.addSentence(F("bye bye"));                    // Add sentence 21
 
  for (int i=0;i<26;i++) { // Add the conversation starters as sentences, to increase relevant vocabulary
      strcpy_P(buffer, (char*) pgm_read_word(&(conversationStarters[i])));
      recognizer.addSentence(buffer);
  }
  
  recognizer.train(); // Train (takes about 20seconds)
  
  //*/
  recognizer.welcomeMessage(false); // turn off welcome message -- no callsign needed. 
  // recognizer.setThreshold(5);  // uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
  recognizer.responses(false); // turn of automatic responses (so we can react).
  recognizer.setVoiceGender(FEMALE_VOICE); // Eliza is female
  if (recognizer.getFirmwareVersion()>=1.1f) { // With 1.1 firmware, we can use the better synthesizer
      recognizer.setSynthesizer(SYNTH_PICO);
  }
  recognizer.ask(F("Hello, my name is Eliza. Talk to me!"));
}

// Eliza main logic. This is the simplest thing you can do... there's lots to do better!
void loop() // run over and over
{
  int res=recognizer.poll();
  if (res>0 && res<21) { // If it's a sentence and not an event, we respond and listen again.
      strcpy_P(buffer, (char*) pgm_read_word(&(directResponses[res-1])));
      recognizer.ask(buffer);
  }
  if (res==21) {
      strcpy_P(buffer, (char*) pgm_read_word(&(directResponses[res-1])));  
      recognizer.say(buffer);
  }
  if ((res>21) || res==UNKNOWN_SENTENCE) { // If we hit an unknown sentence or one that contains a lot of other words, use a general response. 
      strcpy_P(buffer, (char*) pgm_read_word(&(generalResponses[random(0,8)])));
      recognizer.ask(buffer);
  }
  if (res==SILENCE) { // If nothing is said (e.g. noise only) then we use a conversation starter. 
      strcpy_P(buffer, (char*) pgm_read_word(&(conversationStarters[random(0,25)])));
      recognizer.ask(buffer);
  }
}
