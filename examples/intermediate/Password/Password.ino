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
 * This example implements a simulated burglary alarm on/off switch,
 * maybe at a house entry. We don't really have an alarm but we control
 * nifty blinking lights if you connect them: A red LED on pin 13 and 
 * a green LED on pin 12. More importantly, this exapmple shows how to
 * use MOVI's password function for this. MOVI's API eases password 
 * prompts by providing the method password() and two specialized events
 * for valid and invalid password entry. Also, using the password function,
 * the password key does not travel to MOVI or accross the Arduino PINs.
 *
 * Circuitry:
 * Arduino UNO R3, MEGA2560 R3, or Arduino Leonardo R3.
 * Red LED on pin 13 and GND.
 * Green LED on pin 12 and GND. 
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

MOVI recognizer(true);            // Get a MOVI object, parameter true enables serial monitor interface, rx and tx for alternative communication pins on AVR architecture boards

const int redLED=13;              // Change if you connect LEDs to different pins
const int greenLED=12;            
const int maxAttempts=3;          // Maximum number of attempts for password tries

const String passKey="two two three"; // IMPORTANT: The password must consist only of words contained in the trained sentences 
// and must not contain digits or other non-letter characters except one space between the words.

bool alarmArmed=true;   // stores the state of the alarm
int passwordAttempts=0; // counts how many attempts the user had on the password

void setup() // setup Arduino and MOVI
{
  // Say hello with the red LED upon start of the Arduino board
  pinMode(redLED, OUTPUT);
  digitalWrite(redLED, HIGH);   
  delay(1);                   
  digitalWrite(redLED, LOW);  
  delay(1); 

  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.

  // recognizer.callsign("COMPUTER"); // uncomment if you want to make the call sign 'computer'.
  recognizer.addSentence("one two three four five six seven eight nine zero"); // Add 'sentence 1'
  recognizer.addSentence("arm alarm");  // Add sentence 2
  recognizer.addSentence("open door");  // Add sentence 3

  //recognizer.addSentence(passKey); // uncomment to train the passkey. This will lower the false negative rate at the price of lower security due to an increase of false positives.   
  recognizer.train();   // Train (first one takes about 20seconds)

  //*/

  // recognizer.setThreshold(5);           // uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.

  // Say hello with the green LED once MOVI is up and running
  pinMode(greenLED, OUTPUT);
  digitalWrite(greenLED, HIGH);   
  delay(1);                   
  digitalWrite(greenLED, LOW);  
  delay(1);

  recognizer.say("Door alarm simulator. Valid commands are: 'Arm alarm' and 'open door'.");
  
}

void loop() // run over and over
{
  if (alarmArmed) { // Green light on = Alarm off, red light on = alarm on
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);  
  } 
  else {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW); 
  }

  int result=recognizer.poll(); // See if the recognizer got something

  if (result==1 || result==4 || result==SILENCE || result==UNKNOWN_SENTENCE) {  // Give the user some guidance, 4 only if passkey trained.
    recognizer.say("Valid commands are: 'Arm alarm' and 'open door'.");
  }

  if (result==2) { // arm alarm
    recognizer.say("Alarm armed!");
    digitalWrite(redLED, LOW);   // blink red led. green will go off next loop() cycle.
    delay(250);                   
    digitalWrite(redLED, HIGH);  
    delay(250);
    digitalWrite(redLED, LOW);   
    delay(250);                   
    digitalWrite(redLED, HIGH);  
    delay(250);
    // If you have an actual alarm system add some code here that arms it.
    alarmArmed=true;  // now actually arm the alarm
  }

  if (result==3) { // open door
    if (alarmArmed) { // if alarm armed, ask for password...
      recognizer.password("Password please!", passKey); // password() gets a prompt sentence and a KEY to compare to
      passwordAttempts=0;
    } 
    else {
      // If you have an actual alarm system add some code here that opens the door.
      recognizer.say("Come in!"); // ...otherwise be friendly ;-)
    }
  }

  if (result==PASSWORD_ACCEPT) { // This is an event generated by the API and cannot be seen on the Serial Monitor
    recognizer.say("Thank you. Access granted.");
    digitalWrite(greenLED, LOW);   // Some green blinking
    delay(250);                   
    digitalWrite(greenLED, HIGH);  
    delay(250);
    digitalWrite(greenLED, LOW);   
    delay(250);                   
    digitalWrite(greenLED, HIGH);  
    delay(250);
    digitalWrite(greenLED, LOW);   
    delay(250);                   
    digitalWrite(greenLED, HIGH);
    // If you have an actual alarm system add some code here that disarms it.
    alarmArmed=0;        /// disarm alarm
  } 

  if (result==PASSWORD_REJECT) { // This is an event generated by the API and cannot be seen on the Serial Monitor
    recognizer.say("Invalid password!");
    digitalWrite(redLED, LOW);  // do some angry blinking
    delay(250);                   
    digitalWrite(redLED, HIGH);  
    delay(250);
    digitalWrite(redLED, LOW);   
    delay(250);                   
    digitalWrite(redLED, HIGH);   
    passwordAttempts++; // check for how many attempts 
    if (passwordAttempts>=maxAttempts) {
      recognizer.say("Too many tries. Go away!"); // be unfriendly
      // Here, in reality, an alarm should go off or so but since this is a demo we just stop asking...
    } 
    else {
      recognizer.password("Try Again!", passKey);
    } 
  }  

}


