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

#include "MOVIShield.h"
#include <Stream.h>

#if defined(ARDUINO) && ARDUINO >=100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <avr/pgmspace.h>
#define F(str) (str)
#endif

#ifdef ARDUINO_ARCH_AVR
#include <SoftwareSerial.h>
#endif

#ifndef F // check to see if F() macro is missing -- should not be triggered but...
#error MOVI 1.1 requires the F() macro.
#endif

// This is a workaround to not have the MOVI API give warning messages about the use of the F() function.
// It is explained here: https://github.com/arduino/Arduino/issues/1793
#ifdef __GNUC__
#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif
#if GCC_VERSION < 40602 // Test for GCC < 4.6.2
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data"))) // Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734#c4
#ifdef PSTR
#undef PSTR
#define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];})) // Copied from pgmspace.h in avr-libc source
#endif
#endif
#endif
#endif

MOVI::MOVI()
{
    usehardwareserial=false;
    construct(ARDUINO_RX_PIN, ARDUINO_TX_PIN, false);
}

MOVI::MOVI(bool debugonoff)
{
    usehardwareserial=false;
    construct(ARDUINO_RX_PIN, ARDUINO_TX_PIN, debugonoff);
}

MOVI::MOVI(bool debugonoff, int rx, int tx)
{
    #ifndef ARDUINO_ARCH_AVR
    #warning rx and tx parameters only supported in AVR architecture. Using Serial1 hardwired. 
    #endif
    usehardwareserial=false;
    construct(rx, tx, debugonoff);
}

MOVI::MOVI(bool debugonoff, HardwareSerial *hs)
{
    usehardwareserial=true;
    mySerial = hs;
    construct(0, 0, debugonoff);
}

// Arduino's' C++ does not allow for constructor overloading!
void inline MOVI::construct(int rx, int tx, bool debugonoff)
{
    debug=debugonoff;
    shieldinit=0;
    passstring="";
    response="";
    result="";
    intraining=false;
    firstsentence=true;
    callsigntrainok=true;
    
    #ifdef ARDUINO_ARCH_AVR
        if (!usehardwareserial) {
            mySerial=new SoftwareSerial(rx, tx);
        }
    #else
        if (!usehardwareserial) {
            usehardwareserial = true;
            mySerial = &Serial1;
        }
    #endif
}

void MOVI::init()
{
    init(true);
}

void MOVI::init(bool waitformovi)
{
    if (shieldinit==0) {
        if (debug) {
            Serial.begin(ARDUINO_BAUDRATE);
            while (!Serial) {
                ; // wait for serial port to connect. Needed for Leonardo only.
           }
        }
       
#ifdef ARDUINO_ARCH_AVR
        if (!usehardwareserial) ((SoftwareSerial *)mySerial)->begin(ARDUINO_BAUDRATE);
#elif defined ARDUINO_ARCH_SAM
        ((USARTClass *)mySerial)->begin(ARDUINO_BAUDRATE);
#elif defined __ARDUINO_X86__
        ((TTYUARTClass *)mySerial)->begin(ARDUINO_BAUDRATE);
#else
   #error This version of the MOVI API only supports boards with an AVR, SAM or Intel processor.
#endif

        shieldinit=1;
        while (waitformovi && !isReady()) {
            delay(10);
        }
        String sresponse="";
        do {
            mySerial->println(F("INIT"));
            delay(10);
            sresponse=getShieldResponse();
        } while (sresponse.indexOf("@")==-1);
            
        int s=sresponse.indexOf(": ");
        String ver=sresponse.substring(s+2);
        
        // toFloat() didn't work for me. Ugly workarounds working around further bugs follow!
        #ifdef __ARDUINO_X86__ // Intel can't do .c_str() either!
        char carray[ver.length() + 1];
        ver.toCharArray(carray, sizeof(carray));
        firmwareversion = atof(carray);
        #else                 // AVR, SAM
        firmwareversion=atof(ver.c_str());
        #endif
        s=sresponse.indexOf("@");
        ver=sresponse.substring(s+1);
        #ifdef __ARDUINO_X86__   // Intel
        char c2array[ver.length() + 1];
        ver.toCharArray(c2array, sizeof(c2array));
        hardwareversion = atof(c2array);
        #else                    // AVR, Sam
        hardwareversion=atof(ver.c_str());
        #endif
    }
}

signed int MOVI::poll()
{
    firstsentence=false; // We assume loop() and we can't train in loop.
    intraining=false;
  
    int curchar;
    int eventno;
    while (mySerial->available()) {
        curchar=mySerial->read();
        if (curchar=='\n') {
            if (debug) {
                Serial.println(response);
            }
            if (response.lastIndexOf("MOVIEvent[")>=0) { // Dylan-suggested fix that makes sure buffer junk is not interpreted
                eventno=response.substring(response.indexOf("[")+1,response.indexOf("]:")).toInt();
                result=response.substring(response.indexOf(" ")+1);
                if (eventno<100) { // then it's a user-read-only event
                    response="";
                    return SHIELD_IDLE;
                }
                if (eventno==202) {
                    result=response.substring(response.indexOf("#")+1);
                    response="";
                    return result.toInt()+1; // Sentences returned start at 0,
                                             // we make it easier for non-programmers and start at 1.
                }
                if (eventno==203) { // this is a password event
                    response="";
                    result.trim();
                    if (passstring.equals(result)) {
                        return PASSWORD_ACCEPT;
                    } else {
                        return PASSWORD_REJECT;
                    }
                }
                response="";
                return -eventno;
            } else {
                // other jibberish not belonging to MOVI
            }
        } else {
            response+=(char) curchar;
        }
    }

    if (debug) {
        while (Serial.available()) {
             mySerial->write(Serial.read());
        }
    }
    
    return SHIELD_IDLE;
}

String MOVI::getResult()
{
    return result;
}

String MOVI::getShieldResponse()
{
    String resp="";
    int curchar;
    if (shieldinit==0) {
        init();
        return "";
    }
    while (shieldinit>0) {
        while (mySerial->available()) {
            curchar=mySerial->read();
            if (curchar=='\n') {
                if (resp=="") continue;
                return resp;
            } else {
                resp+=(char) curchar;
            }
        }
        delay(10);
    }
    return "";
}

bool MOVI::sendCommand(String command, String parameter, String okresponse)
{
    if (isReady()) {
        mySerial->println(command+" "+parameter+"\n");
        if (okresponse=="") return true;
        if (getShieldResponse().indexOf(okresponse)>=0) {
            return true;
        } else return false;
    } else return false;
}

bool MOVI::sendCommand(const __FlashStringHelper* command, const __FlashStringHelper* parameter, String okresponse)
{
    if (isReady()) {
        mySerial->print(command);
        mySerial->print(" ");
        mySerial->print(parameter);
        mySerial->println("\n");
        if (okresponse=="") return true;
        if (getShieldResponse().indexOf(okresponse)>=0) {
            return true;
        } else return false;
    } else return false;
}

void MOVI::sendCommand(String command, String parameter)
{
    if (firstsentence || intraining) sendCommand(command,parameter,"]"); // Use controlled sendcommand when used during initialization
    else mySerial->println(command+" "+parameter+"\n");
}

void MOVI::sendCommand(String command)
{
    if (firstsentence || intraining) sendCommand(command,"","]"); // Use controlled sendcommand when used during initialization
    else mySerial->println(command+"\n");
}

void MOVI::sendCommand(const __FlashStringHelper* command, const __FlashStringHelper* parameter)
{
    if (firstsentence || intraining) {  // Use controlled sendcommand when used during initialization
        sendCommand(command,parameter,"]");
    } else {
        mySerial->print(command);
        mySerial->print(" ");
        mySerial->print(parameter);
        mySerial->println("\n");
    }
}

void MOVI::sendCommand(const __FlashStringHelper* command)
{
    if (firstsentence || intraining) { // Use controlled sendcommand when used during initialization
        sendCommand(command,F(""),"]");
    } else {
        mySerial->print(command);
        mySerial->println("\n");
    }
}

bool MOVI::isReady()
{
    if (shieldinit==100) {
        return true;
    }
    if (shieldinit==0) {
        init();
    }
    mySerial->println(F("PING\n"));
    if (getShieldResponse().indexOf("PONG")) {
        shieldinit=100;
        return true;
    }
    shieldinit=1;
    return false;
}

void MOVI::factoryDefault()
{
    sendCommand(F("FACTORY"));
}

void MOVI::stopDialog()
{
    sendCommand(F("STOP"));
}

void MOVI::restartDialog()
{
    sendCommand(F("RESTART"));
}

void MOVI::say(const __FlashStringHelper* sentence)
{
    sendCommand(F("SAY"),sentence);
}

void MOVI::say(String sentence)
{
    sendCommand("SAY",sentence);
}

void MOVI::pause()
{
    sendCommand(F("PAUSE"));
}

void MOVI::unpause()
{
    sendCommand(F("UNPAUSE"));
}

void MOVI::finish()
{
    sendCommand(F("FINISH"));
}

void MOVI::play(String filename)
{
    sendCommand("PLAY",filename);
}

void MOVI::abort()
{
    sendCommand(F("ABORT"),F(""));
}

void MOVI::setSynthesizer(int synth)
{
    if (synth==SYNTH_PICO) {
        sendCommand(F("SETSYNTH"),F("PICO"));
    } else {
        sendCommand(F("SETSYNTH"),F("ESPEAK"));
    }
}

void MOVI::setSynthesizer(int synth, String commandline)
{
    if (synth==SYNTH_PICO) {
            sendCommand("SETSYNTH","PICO "+commandline);
    } else {
            sendCommand("SETSYNTH","ESPEAK "+commandline);
    }
}

void MOVI::password(const __FlashStringHelper* question, String passkey)
{
    passstring=String(passkey);
    passstring.toUpperCase();
    passstring.trim();
    say(question);
    sendCommand(F("PASSWORD"),F(""));
}

void MOVI::password(String question, String passkey)
{
    passstring=String(passkey);
    passstring.toUpperCase();
    passstring.trim();
    say(question);
    sendCommand(F("PASSWORD"));
}


void MOVI::ask(String question)
{
    // checking for empty string makes ask faster when there is no question, it's better to use ask() though
    if (question.length() > 0) say(question);
    sendCommand(F("ASK"));
}

void MOVI::ask(const __FlashStringHelper* question)
{
    // To check for empty string here, we need to copy the string into string memory. Bad idea.
    say(question);
    sendCommand(F("ASK"));
}

// this is a new ask method without passing a string.
void MOVI::ask()
{
    sendCommand(F("ASK"));
}

void MOVI::callSign(String callsign)
{
    if (callsigntrainok) sendCommand("CALLSIGN",callsign,"callsign");
    callsigntrainok=false;
}

void MOVI::responses(bool on)
{
    String parameter="ON";
    if (!on) parameter="OFF";
    sendCommand("RESPONSES",parameter);
}

void MOVI::welcomeMessage(bool on)
{
    String parameter="ON";
    if (!on) parameter="OFF";
    sendCommand("WELCOMEMESSAGE",parameter);
}

void MOVI::beeps(bool on)
{
    String parameter="ON";
    if (!on) parameter="OFF";
    sendCommand("BEEPS",parameter);
}

void MOVI::setVoiceGender(bool female)
{
    if (female) sendCommand(F("FEMALE"));
    else sendCommand(F("MALE"));
}

void MOVI::setVolume(int volume)
{
    sendCommand("VOLUME",String(volume));
}

void MOVI::setThreshold(int threshold)
{
    sendCommand("THRESHOLD",String(threshold));
}

float MOVI::getFirmwareVersion()
{
    return firmwareversion;
}

float MOVI::getAPIVersion()
{
    return API_VERSION;
}

float MOVI::getHardwareVersion()
{
    return hardwareversion;
}

bool MOVI::addSentence(const __FlashStringHelper* sentence)
{
    if (firstsentence) {
        intraining=sendCommand(F("NEWSENTENCES"),F(""),"210");
        firstsentence=false;
    }
    if (!intraining) return false;
    intraining=sendCommand(F("ADDSENTENCE"),sentence,"211");
    return intraining;
}

bool MOVI::addSentence(String sentence)
{
    // Needs a new MOVI instance (typically restart Arduino). This avoids training only part of the sentence set.
    if (firstsentence) {
        intraining=sendCommand("NEWSENTENCES","","210");
        firstsentence=false;
    }
    if (!intraining) return false;
    intraining=sendCommand("ADDSENTENCE",sentence,"211");
    return intraining;
}

bool MOVI::train()
{
    if (!intraining) return false;
    sendCommand("TRAINSENTENCES","","trained");
    intraining=false;
    return true;
}

MOVI::~MOVI()
{
    if (NULL != mySerial && (!usehardwareserial))
    {
        delete mySerial;
    }
}
