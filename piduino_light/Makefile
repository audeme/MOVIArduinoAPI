#
# Build Arduino Core for Raspberry PI
#
# Copyright (c) 2018 by Gerald Friedland. fractor@audeme.com

CXX=g++
CFLAGS=-Wall -pedantic -I.
OBJ = Arduino.o piduinowrapper.o sysfsio.o HardwareSerial.o Print.o WMath.o IPAddress.o stdlib_noniso.o WString.o

all: libpiduino.a

libpiduino.a: $(OBJ)
	ar rvs libpiduino.a $(OBJ)
	ranlib libpiduino.a

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f *.o core 

distclean: clean
	rm -f *.a
