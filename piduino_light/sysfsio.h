/*
  sysfsio.h - GPIOs for use on Raspberry PI with the MOVI(TM) Arduino 
  Speech Dialog Shield by Gerald Friedland at Audeme.com in 2018.
  
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

#ifndef sysfsio_h
#define sysfsio_h


#ifdef __cplusplus
extern "C"{
#endif

int GPIOExport(int);
int GPIOUnexport(int);
int GPIODirection(int, int);
int GPIORead(int);
int GPIOWrite(int, int);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
