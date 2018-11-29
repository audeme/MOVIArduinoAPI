/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified for RaspberryPi by Hristo Gochkov 2015

  Further modified for use on Raspberry PI with the MOVI(TM) Arduino 
  Speech Dialog Shield by Gerald Friedland at Audeme.com in 2018.
*/

#ifndef Arduino_h
#define Arduino_h

#include <math.h>
#include "bcm2835_registers.h"
#include "pgmspace.h"



#ifdef __cplusplus
extern "C"{
#endif

// Arduino Digital PIN to Raspberry PI GPIO mappping. Change for convenience.
// Arduino Pins:                      D0  D1  D2 D3 D4  D5  D6  D7  D8  D9  D10 D11 D12 D13 
const int ArduinoDPINtoPIGPIO[14] = { 15, 14, 4, 17, 18, 27, 22, 23, 24, 25, 5, 6, 12, 13};
// Pi Pins:			      10  8   7  11  12  13  15  16  18  22  29 31 32  33 
// Caution: Pins D1 and D2 are RX/TX and are used for communication with MOVI. Don't use.

#define PWM_RANGE     0xFF

void yield(void);

#define HIGH 0x1
#define LOW  0x0

#define INPUT   0x0
#define OUTPUT  0x1
#define ALT0    0x04
#define ALT1    0x05
#define ALT2    0x06
#define ALT3    0x07
#define ALT4    0x03
#define ALT5    0x02
#define INPUT_PULLUP    0x10
#define INPUT_PULLDOWN  0x20

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define CHANGE 3
#define FALLING 1
#define RISING 2

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define interrupts()
#define noInterrupts()
#define cli()
#define sei()

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

// avr-libc defines _NOP() since 1.6.2
#ifndef _NOP
#define _NOP() do {} while (0)
#endif

#define bit(b) (1UL << (b))

#define micros() (unsigned long)(STCV)
#define millis() (unsigned long)(STCV / 1000)

typedef unsigned int word;
typedef uint8_t boolean;
typedef uint8_t byte;

//under millisecond delayMicroseconds halts the CPU
void delayMicroseconds(uint32_t m);
void delay(uint32_t m);
void sleepMicroseconds(uint32_t m);

void pinMode(uint8_t, uint8_t);
void digitalWrite(uint8_t, uint8_t);
int digitalRead(uint8_t);

//those return nothing and are here for compatibility
int analogRead(uint8_t pin);
void analogReference(uint8_t mode);

uint32_t analogWriteSetup(uint32_t freq, uint32_t range);
void analogWrite(uint8_t, uint16_t);

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

typedef void (*voidFuncPtr)(void);
void attachInterrupt(uint8_t, voidFuncPtr, int mode);
void detachInterrupt(uint8_t);
// end unimplemented

void setup(void);
void loop(void);


#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#include "WCharacter.h"
#include "WString.h"
#include "HardwareSerial.h"

uint16_t makeWord(uint16_t w);
uint16_t makeWord(byte h, byte l);

// WMath prototypes
long random(long);
long random(long, long);
void randomSeed(unsigned long);
long map(long, long, long, long, long);

#define word(...) makeWord(__VA_ARGS__)

#endif

#include "binary.h"

#endif
