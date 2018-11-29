#
# Build MOVI Arduino Voice Dialog Shield Library for Raspberry PI
#
# Copyright (c) 20108 by Gerald Friedland. fractor@audeme.com

CXX=g++
CFLAGS=-DRASPBERRYPI -Wall -pedantic -I. -Ipiduino_light
DEPS = MOVIShield.h
OBJ = piduino_light/Arduino.o piduino_light/piduinowrapper.o piduino_light/sysfsio.o piduino_light/HardwareSerial.o  piduino_light/Print.o piduino_light/WMath.o piduino_light/IPAddress.o piduino_light/stdlib_noniso.o piduino_light/WString.o MOVIShield.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

all: LightSwitch LightSwitch2 LightSwitch3 SynthesizerControl WordCount WordSequence1 WordSequence2 WordSpotter WordSpotter2 BeepsOff ElizaKickstarter NestedDialog Password YesSir BattleShip Eliza HuntTheWumpus LowLevelInterface SentenceSets SerialMonitor SimpleDebug VersionCheck LightSwitch_MX LightSwitch_DE PlaySounds

LightSwitch: $(OBJ)
	$(CXX) -o examples/beginner/$@/$@ $^ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino

LightSwitch2: $(OBJ)
	$(CXX) -o examples/beginner/$@/$@ $^ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino

LightSwitch3: $(OBJ)
	$(CXX) -o examples/beginner/$@/$@ $^ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino
	
SynthesizerControl: $(OBJ)
	$(CXX) -o examples/beginner/$@/$@ $^ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino
	
WordCount: $(OBJ)
	$(CXX) -o examples/beginner/$@/$@ $^ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino

WordSequence1: $(OBJ)
	$(CXX) -o examples/beginner/$@/$@ $^ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino

WordSequence2: $(OBJ)
	$(CXX) -o examples/beginner/$@/$@ $^ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino

WordSpotter: $(OBJ)
	$(CXX) -o examples/beginner/$@/$@ $^ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino

WordSpotter2: $(OBJ)
	$(CXX) -o examples/beginner/$@/$@ $^ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino

BeepsOff: $(OBJ)
	$(CXX) -o examples/intermediate/$@/$@ $^ $(CFLAGS) -xc++ examples/intermediate/$@/$@.ino

ElizaKickstarter: $(OBJ)
	$(CXX) -o examples/intermediate/$@/$@ $^ $(CFLAGS) -xc++ examples/intermediate/$@/$@.ino
	
NestedDialog: $(OBJ)
	$(CXX) -o examples/intermediate/$@/$@ $^ $(CFLAGS) -xc++ examples/intermediate/$@/$@.ino

Password: $(OBJ)
	$(CXX) -o examples/intermediate/$@/$@ $^ $(CFLAGS) -xc++ examples/intermediate/$@/$@.ino

YesSir: $(OBJ)
	$(CXX) -o examples/intermediate/$@/$@ $^ $(CFLAGS) -xc++ examples/intermediate/$@/$@.ino	

BattleShip: $(OBJ)
	$(CXX) -o examples/proficient/$@/$@ $^ $(CFLAGS) -xc++ examples/proficient/$@/$@.ino

Eliza: $(OBJ)
	$(CXX) -o examples/proficient/$@/$@ $^ $(CFLAGS) -xc++ examples/proficient/$@/$@.ino
	
HuntTheWumpus: $(OBJ)
	$(CXX) -o examples/proficient/$@/$@ $^ $(CFLAGS) -xc++ examples/proficient/$@/$@.ino
	
LowLevelInterface: $(OBJ)
	$(CXX) -o examples/proficient/$@/$@ $^ $(CFLAGS) -xc++ examples/proficient/$@/$@.ino

SentenceSets: $(OBJ)
	$(CXX) -o examples/proficient/$@/$@ $^ $(CFLAGS) -xc++ examples/proficient/$@/$@.ino

SerialMonitor: $(OBJ)
	$(CXX) -o examples/debug/$@/$@ $^ $(CFLAGS) -xc++ examples/debug/$@/$@.ino
	
SimpleDebug: $(OBJ)
	$(CXX) -o examples/debug/$@/$@ $^ $(CFLAGS) -xc++ examples/debug/$@/$@.ino

VersionCheck: $(OBJ)
	$(CXX) -o examples/debug/$@/$@ $^ $(CFLAGS) -xc++ examples/debug/$@/$@.ino

LightSwitch_MX: $(OBJ)
	$(CXX) -o examples/sdcard_hacks/$@/$@ $^ $(CFLAGS) -xc++ examples/sdcard_hacks/$@/$@.ino
	
LightSwitch_DE: $(OBJ)
	$(CXX) -o examples/sdcard_hacks/$@/$@ $^ $(CFLAGS) -xc++ examples/sdcard_hacks/$@/$@.ino

PlaySounds: $(OBJ)
	$(CXX) -o examples/sdcard_hacks/$@/$@ $^ $(CFLAGS) -xc++ examples/sdcard_hacks/$@/$@.ino

.PHONY: clean

clean:
	rm -f piduino_light/*.o *.o core
	rm -f `find examples/ -name "*" -not -type d -not -name "*.ino" -print`
