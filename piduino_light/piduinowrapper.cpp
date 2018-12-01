/*
  piduinowrapper.cpp Arduino Console for Raspberry PI.
  Developed for use with the MOVI(TM) Arduino(R) Voice Dialog 
  Shield by Gerald Friedland at Audeme.com in 2018.

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

#include "Arduino.h"
#include "HardwareSerial.h"
#include <stdio.h>

HardwareSerial Serial;
HardwareSerial Serial1;

int main(int argv, char **args)
{
	String device="/dev/serial0";
	if (argv>2) {
		fprintf(stderr,"Usage: %s [<device>]\n",args[0]);
		fprintf(stderr,"where [<device>] is the uart device connected to MOVI.\n");
		fprintf(stderr,"Default for <device> is %s.\n",device.c_str());
		return 1;
	}
	if (argv==2) {
		device=String(args[1]);
	}
	if (!Serial.begin(115200)) {
		return -1;
	}
	Serial.println("Raspberry PI Arduino Wrapper Console for MOVI Copyright (C) 2018 by Audeme, LLC.");
	Serial1.setDevice(device.c_str());
	Serial.print("Communication on ");
	Serial.println(Serial1.getDevice());
	//probe device set.
	if (!Serial1.begin(9600)) {
		return -1;
	}
	Serial1.end(); // close it again so MOVI API can open it.
	setup();
	while (1) { 
		loop();
	}
	return 0;
} 
