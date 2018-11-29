/*
  Arduino.cpp for use on Raspberry PI with the MOVI(TM) Arduino 
  Speech Dialog Shield by Gerald Friedland at Audeme.com in 2018.

  Feel free to add your own functionality here. Uses GPIO through SYSFS.
  Alternative, more powerful implementations of GPIO are discussed at 
  https://elinux.org/RPi_GPIO_Code_Samples

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
*/

#define ARDUINO_MAIN
#include "Arduino.h"
#include "sysfsio.h"
#include <errno.h>

void sleepMicroseconds(uint32_t m) {
  usleep(m);
}

void delay(uint32_t m){
  while(m--) usleep(1000);
}

void delayMicroseconds(uint32_t m){
  usleep(m);
}

void yield() 
{
 usleep(1); // No multithreading so not sure what to do here.
}

void pinMode(uint8_t pin, uint8_t mode)
{
	if (pin>13) {
		fprintf(stderr,"Only Arduino pins D0-D13 supported. Ignoring.\n");
		return;
	}
//	printf("pin= %d, mode=%d, RPIpin=%d\n",pin,mode,ArduinoDPINtoPIGPIO[pin]);
	if (-1 == GPIOExport(ArduinoDPINtoPIGPIO[pin])) return;
	if (-1 == GPIODirection(ArduinoDPINtoPIGPIO[pin], mode)) return;	
}

void digitalWrite(uint8_t pin, uint8_t value)
{
	if (pin>13) {
		fprintf(stderr,"Only Arduino pins D0-D13 supported. Ignoring.\n");
		return;
	}	
	GPIOWrite(ArduinoDPINtoPIGPIO[pin], value);
}
 
int digitalRead(uint8_t pin)
{
	if (pin>13) {
		fprintf(stderr,"Only Arduino pins D0-D13 supported. Returning 0.\n");
		return 0;
	}
	return GPIORead(ArduinoDPINtoPIGPIO[pin]);
}

void analogReference(uint8_t mode)
{
	fprintf(stderr,"analogReference() not implemented. Ignoring.\n");
}

int analogRead(uint8_t pin)
{ 
	fprintf(stderr,"analogRead() not implemented. Returning 0.\n");
	return 0; 
}

uint32_t analogWriteSetup(uint32_t freq, uint32_t range)
{
	fprintf(stderr,"analogWriteSetup() not implemented. Returning 0.\n");
	return 0; 
}


void analogWrite(uint8_t pin, uint16_t value)
{
	fprintf(stderr,"analogWrite() not implemented. Ignoring.\n");
}

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout)
{
	fprintf(stderr,"pulseIn() not implemented. Returning 0.\n");
	return 0; 
}


void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
	fprintf(stderr,"shiftOut() not implemented. Ignoring.\n"); 
}

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder)
{
	fprintf(stderr,"shiftIn() not implemented. Returning 0.\n"); 
	return 0;
}

void attachInterrupt(uint8_t, voidFuncPtr, int mode)
{
	fprintf(stderr,"attachInterrupt() not implemented. Ignoring.\n"); 
}

void detachInterrupt(uint8_t)
{
	fprintf(stderr,"dettachInterrupt() not implemented. Ignoring.\n"); 
}

