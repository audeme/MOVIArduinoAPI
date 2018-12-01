#
# Build MOVI Arduino Voice Dialog Shield Library for Raspberry PI
#
# Copyright (c) 2018 by Gerald Friedland. fractor@audeme.com

CXX=g++
ARDUINODIR=piduino_light
CFLAGS=-DRASPBERRYPI -Wall -pedantic -I. -Ipiduino_light
LIBFLAGS=-L. -lmovi -lpiduino
LIBS = libmovi.a libpiduino.a
OBJ = MOVIShield.o

all: libpiduino.a libmovi.a examples

libpiduino.a: 
	@$(MAKE) -C $(ARDUINODIR)
	ln -s piduino_light/libpiduino.a libpiduino.a

libmovi.a: $(OBJ) libpiduino.a
	ar rvs libmovi.a $(OBJ) libpiduino.a
	ranlib libmovi.a

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $< -L. -lpiduino

examples: beginner intermediate proficient debug sd_hacks

beginner: LightSwitch LightSwitch2 LightSwitch3 SynthesizerControl WordCount WordSequence1 WordSequence2 WordSpotter WordSpotter2
intermediate: BeepsOff ElizaKickstarter NestedDialog Password YesSir 
proficient: BattleShip Eliza HuntTheWumpus LowLevelInterface SentenceSets 
debug: SerialMonitor SimpleDebug VersionCheck 
sd_hacks: LightSwitch_MX LightSwitch_DE PlaySounds

LowLevelInterface: libpiduino.a
	$(CXX) -o examples/proficient/$@/$@ $(CFLAGS) -xc++ examples/proficient/$@/$@.ino -L. -lpiduino

LightSwitch: $(LIBS)
	$(CXX) -o examples/beginner/$@/$@ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino $(LIBFLAGS)

LightSwitch2: $(LIBS)
	$(CXX) -o examples/beginner/$@/$@ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino $(LIBFLAGS)

LightSwitch3: $(LIBS)
	$(CXX) -o examples/beginner/$@/$@ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino $(LIBFLAGS)
	
SynthesizerControl: $(LIBS)
	$(CXX) -o examples/beginner/$@/$@ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino $(LIBFLAGS)
	
WordCount: $(LIBS)
	$(CXX) -o examples/beginner/$@/$@ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino $(LIBFLAGS)

WordSequence1: $(LIBS)
	$(CXX) -o examples/beginner/$@/$@ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino $(LIBFLAGS)

WordSequence2: $(LIBS)
	$(CXX) -o examples/beginner/$@/$@ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino $(LIBFLAGS)

WordSpotter: $(LIBS)
	$(CXX) -o examples/beginner/$@/$@ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino $(LIBFLAGS)

WordSpotter2: $(LIBS)
	$(CXX) -o examples/beginner/$@/$@ $(CFLAGS) -xc++ examples/beginner/$@/$@.ino $(LIBFLAGS)

BeepsOff: $(LIBS)
	$(CXX) -o examples/intermediate/$@/$@ $(CFLAGS) -xc++ examples/intermediate/$@/$@.ino $(LIBFLAGS)

ElizaKickstarter: $(LIBS)
	$(CXX) -o examples/intermediate/$@/$@ $(CFLAGS) -xc++ examples/intermediate/$@/$@.ino $(LIBFLAGS)
	
NestedDialog: $(LIBS)
	$(CXX) -o examples/intermediate/$@/$@ $(CFLAGS) -xc++ examples/intermediate/$@/$@.ino $(LIBFLAGS)
  
Password: $(LIBS)
	$(CXX) -o examples/intermediate/$@/$@ $(CFLAGS) -xc++ examples/intermediate/$@/$@.ino $(LIBFLAGS)

YesSir: $(LIBS)
	$(CXX) -o examples/intermediate/$@/$@ $(CFLAGS) -xc++ examples/intermediate/$@/$@.ino $(LIBFLAGS)	

BattleShip: $(LIBS)
	$(CXX) -o examples/proficient/$@/$@ $(CFLAGS) -xc++ examples/proficient/$@/$@.ino $(LIBFLAGS)

Eliza: $(LIBS)
	$(CXX) -o examples/proficient/$@/$@ $(CFLAGS) -xc++ examples/proficient/$@/$@.ino $(LIBFLAGS)
	
HuntTheWumpus: $(LIBS)
	$(CXX) -o examples/proficient/$@/$@ $(CFLAGS) -xc++ examples/proficient/$@/$@.ino $(LIBFLAGS)
	
SentenceSets: $(LIBS)
	$(CXX) -o examples/proficient/$@/$@ $(CFLAGS) -xc++ examples/proficient/$@/$@.ino $(LIBFLAGS)

SerialMonitor: $(LIBS)
	$(CXX) -o examples/debug/$@/$@ $(CFLAGS) -xc++ examples/debug/$@/$@.ino $(LIBFLAGS)
	
SimpleDebug: $(LIBS)
	$(CXX) -o examples/debug/$@/$@ $(CFLAGS) -xc++ examples/debug/$@/$@.ino $(LIBFLAGS)

VersionCheck: $(LIBS)
	$(CXX) -o examples/debug/$@/$@ $(CFLAGS) -xc++ examples/debug/$@/$@.ino $(LIBFLAGS)

LightSwitch_MX: $(LIBS)
	$(CXX) -o examples/sdcard_hacks/$@/$@ $(CFLAGS) -xc++ examples/sdcard_hacks/$@/$@.ino $(LIBFLAGS)
	
LightSwitch_DE: $(LIBS)
	$(CXX) -o examples/sdcard_hacks/$@/$@ $(CFLAGS) -xc++ examples/sdcard_hacks/$@/$@.ino $(LIBFLAGS)

PlaySounds: $(LIBS)
	$(CXX) -o examples/sdcard_hacks/$@/$@ $(CFLAGS) -xc++ examples/sdcard_hacks/$@/$@.ino $(LIBFLAGS)

clean: 
	rm -f $(ARDUINODIR)/*.o *.o core 

distclean: clean
	rm -f *.a
	rm -f $(ARDUINODIR)/*.a
	rm -f `find examples/ -name "*" -not -type d -not -name "*.ino" -print`
