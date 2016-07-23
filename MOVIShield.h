/********************************************************************
 This is a library for the Audeme MOVI Voice Control Shield
 ----> http://www.audeme.com/MOVI/
 This code is inspired and maintained by Audeme but open to change
 and organic development on GITHUB:
 ----> https://github.com/audeme/MOVIArduinoAPI
 Written by Gerald Friedland for Audeme LLC.
 Contact: fractor@audeme.com
 BSD license, all text above must be included in any redistribution.
 ********************************************************************/

#ifndef ____MOVIShield__
#define ____MOVIShield__

// Check for architecture
#ifdef ARDUINO_ARCH_AVR // Arduino Uno, Mega, Leonardo, Yun, Duemillanova and clones (Freeduino, etc)
#include <SoftwareSerial.h>
#elif defined ARDUINO_ARCH_SAM // Arduino Due
// Will use Serial1 later
#elif defined __ARDUINO_X86__  // Intel Edison, Intel Galileo
// Will user Serial1 later
#elif defined ARDUINO_ARCH_SAMD  // Arduino Zero, Zero Pro, M0 and M0 Pro
// Not supported right now
#else                          // Old versions of the Arduino IDE that doesn't have ARCH destinctions yet.
#include <SoftwareSerial.h>  
#define ARDUINO_ARCH_AVR      // Assume AVR
#endif

#include "Arduino.h"

#ifndef ARDUINO_RX_PIN
#define ARDUINO_RX_PIN 10  //  Arduino RX pin (default 10) needs to be connected to MOVI TX PIN
#endif

#ifndef ARDUINO_TX_PIN
#define ARDUINO_TX_PIN 11  //  Arduino TX pin (default 11) needs to be connected to MOVI RX PIN
#endif

#ifndef ARDUINO_BAUDRATE
#define ARDUINO_BAUDRATE 9600  // Communication rate between MOVI and Arduino. 9600bps is max for software serial ports.
#endif                         // As of firmware 1.1 changing the bitrate is possible with a config file on the SDcard but
                               // then HardwareSerial must be used and initialized before constructing MOVI.

#ifndef SYNTH_ESPEAK
#define SYNTH_ESPEAK 0  //  Constant for setSynthesizer method
#endif

#ifndef SYNTH_PICO
#define SYNTH_PICO 1  //  Constant for setSynthesizer method
#endif


// --- MOVI events ---

#ifndef SHIELD_IDLE
#define SHIELD_IDLE 0
#endif

#ifndef BEGIN_LISTEN
#define BEGIN_LISTEN -140
#endif

#ifndef END_LISTEN
#define END_LISTEN -141
#endif

#ifndef BEGIN_SAY
#define BEGIN_SAY -150
#endif

#ifndef END_SAY
#define END_SAY -151
#endif

#ifndef CALLSIGN_DETECTED
#define CALLSIGN_DETECTED -200
#endif

#ifndef RAW_WORDS
#define RAW_WORDS -201
#endif

#ifndef PASSWORD_ACCEPT
#define PASSWORD_ACCEPT -204
#endif

#ifndef PASSWORD_REJECT
#define PASSWORD_REJECT -404
#endif

#ifndef NOISE_ALARM
#define NOISE_ALARM -530
#endif

#ifndef SILENCE
#define SILENCE -501
#endif

#ifndef UNKNOWN_SENTENCE
#define UNKNOWN_SENTENCE -502
#endif

// --- MOVI useful constants ---

#ifndef MALE_VOICE
#define MALE_VOICE false
#endif

#ifndef FEMALE_VOICE
#define FEMALE_VOICE true
#endif

#ifndef API_VERSION
#define API_VERSION 1.10f
#endif

class MOVI
{
    
public:
    
    // --- Methods that must be used in setup() ----
    
    // Construct a MOVI object with default configuration.
    MOVI();
    
    // Construct a MOVI object with optional serial monitor interaction.
    MOVI(bool debugonoff);
   
    // Construct a MOVI object with different communication pins and optional serial monitor interaction. This constructor only works on AVR architecture CPU (e.g Arduino Uno, Mega, Leonardo. NOT Due, Zero, Edison)
    MOVI(bool debugonoff, int rx, int tx);
    
    // Construct a MOVI object with an existing HardwareSerial (eg. Arduino Mega). If you use this constructor, you need to
    // calls hs.begin(<bitrate>) before calling init.
    MOVI(bool debugonoff, HardwareSerial *hs);
    
    // init waits for MOVI to be booted and resets some settings. If the recognizer had been stopped with
    // stopDialog() it is restarted.
    void init();
    
    // This init method only initializes the API and doesn't wait for MOVI to be ready if the parameter is false.
    void init(bool waitformovi);
    
    // This method can be used to determine if MOVI is ready to receive commands, e.g. when MOVI has been initialized with init(false).
    bool isReady();
    
    // This method adds a sentence to the training set. Sentences must not contain any punctuation or numbers.
    // Everything must be spelled out. No special characters, umlauts or accents. Uppercase or lowercase does
    // not matter.
    bool addSentence(String sentence);
    
    // addsentence using Flash memory (e.g., addsentence(F("Light On");)
    bool addSentence(const __FlashStringHelper* sentence);
    
    // This method checks if the training set contains new sentences since the last training. If so, it trains all
    // sentences added in this MOVI instance. Once training is performed, no more sentences can be added
    // and training cannot be invoked again in the same instance.
    bool train();
    
    // This method sets the callsign to the parameter given. If the callsign has previously been set to the
    // same value, nothing happens. Only one callsign can be trained per MOVI instance. The callsign can be
    // the empty string. The MOVI will react to any noise above the threshold.
    void callSign(String callsign);
    
    
    // --- Methods that are typically used in setup() but can also be used in loop() ---
    
    // Sets the output volume of the speaker.
    void setVolume(int volume);
    
    // Sets the gender of the speech synthesizer.
    void setVoiceGender(bool female);
    
    // Sets the noise threshold of the recognizer. Values vary between 2 and 95. Factory default is 5.
    void setThreshold(int threshold);
    
    // Turns the spoken responses as a result of recognition events (e.g. silence or noise) on or off.
    void responses(bool on);
    
    // Turns off the spoken welcome message indicating the call sign.
    void welcomeMessage(bool on);
    
    // Turns the recognition beeps on or off.
    void beeps(bool on);
    
    // --- Methods that are typically used in loop() ---
    
    // This method is called in loop() to get an event from the recognizer. 0 stand for no event. A postive number
    // denotes a sentence number. A negative value defines an event number. Event numbers are the negatives of
    // the numbers displayed on the serial monitor. For example: MOVIEvent[200] would return -200.
    signed int poll();
    
    // Gets the result string of an event. For example: MOVIEvent[201]: LET THERE LIGHT results in "LET THERE BE
    // LIGHT\n". The resulting string might need trimming for comparison to other strings. The resulting string
    // is uppercase and does not contain any numbers, punctuation or special characters.
    String getResult();
    
    // Makes MOVI speak the sentence given as parameter using the speech synthesizer.
    void say(String sentence);
    
    // Directly listen without requiring a callsign.
    void ask();

    // Makes MOVI speak the sentence given as first parameter and then directly listen without requiring a
    // callsign.
    void ask(String question);
    
    // say using Flash memory (e.g., say(F("Hello World");)
    void say(const __FlashStringHelper* sentence);

    // ask, Flash memory version (e.g., ask(F("How are you?");)
    void ask(const __FlashStringHelper* question);
 
    
    // Makes MOVI speak the sentence given as first parameter. Then MOVI's password function is used to query for
    // a password. The API comapres the passkey with the password and return either PASSWORD_REJECT or
    // PASSWORD_ACCEPT as an event. The passkey is not transferred to or saved on the MOVI board.
    // IMPORTANT: The passkey must consist only of words contained in the trained sentences
    // and must not contain digits or other non-letter characters except one space between the words.
    void password(String question, String passkey);
    
    // password, Flash memory version (e.g., ask(F("Password please",password);)
    void password(const __FlashStringHelper* question, String passkey);
    
    // --- Infrequently used advanced commands ---
 
    // Pauses the recognizer until the an upause(), ask(), say() or password() command.
    void pause();
    
    // Silently interrupts a pause. See pause()
    void unpause();
    
    // Finishes up the currently executing sentence recognition.
    void finish();
    
    // Play an audio file (wave format) located on the update partition of the SDcard.
    void play(String filename);
    
    // Aborts a play or say command immediately
    void abort();
    
    // Sets MOVI's synthesizer to one of SYNTH_ESPEAK or SYNTH_PICO.
    void setSynthesizer(int synth);
    
    // Sets MOVI's synthesizer to one of SYNTH_ESPEAK or SYNTH_PICO with given command line parameters.
    void setSynthesizer(int synth, String commandline);
    
    // Sends a command manually to MOVI, without argument.
    void sendCommand(String command);
    
    // Sends a command manually to MOVI, with arguments.
    void sendCommand(String command, String parameter);
    
    // Sends a command manually to MOVI. Flash memory version for AVR
    void sendCommand(const __FlashStringHelper* command, const __FlashStringHelper* parameter);
    
    // Returns MOVI's firmware version.
    float getFirmwareVersion();
    
    // Returns MOVI's board revision.
    float getHardwareVersion();
    
    // Returns the version of this API.
    float getAPIVersion();
    
    // Stops the recognizer and synthesizer.
    void stopDialog();
    
    // Restarts the recognizer and synthesizer manually.
    void restartDialog();
    
    // Resets MOVI to factory default. This method should only be used in setup() and only if needed. All trained
    // sentences and callsigns are untrained. The preferrable method for a factory reset is to use the serial
    // monitor.
    void factoryDefault();
    
    // Destructs the MOVI object
    ~MOVI();
    
    // --- private methods and variables ---
private:
    float hardwareversion; // stores hardware version
    float firmwareversion; // stores firmware version
    int shieldinit;        // stores the init state of the MOVI object
    bool callsigntrainok;  // makes sure callsign is only called once
    bool debug;            // debug allows serial monitor interfacing
    bool intraining;       // determines if training is ok
    bool firstsentence;    // determines if addSentence() has been called
    void construct(int rx, int tx, bool debugonoff); // workaround for non-functioning constructor overloading
    String passstring;      // stores the passkey for a password() request
    bool usehardwareserial; // flag to store if we are using AVR SoftwareSerial or HardwareSerial

    Stream *mySerial;
    
    String response; // stores the stream of serial communication characters
    String result;   // stores the last result for getResult()
    String getShieldResponse(); // used to communicate with the shield before poll()
    
    bool sendCommand(String command, String parameter, String okresponse); // sends a command and listens to responses. Only works before poll().

    // Sends a command manually to MOVI. Flash memory version for AVR
    void sendCommand(const __FlashStringHelper* command);
    bool sendCommand(const __FlashStringHelper* command, const __FlashStringHelper* parameter, String okresponse);
};


#endif /* defined(____MOVIShield__) */
