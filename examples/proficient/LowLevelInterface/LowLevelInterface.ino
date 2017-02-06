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
 * MOVI low level interface using Serial Monitor, without API.
 *
 * Circuitry:
 * Arduino UNO R3, MEGA2560 R3, or Arduino Leonardo R3.
 * Connect speaker to MOVI.
 * IMPORTANT: Always use external power supply with MOVI. 
 * 
 * Other Arduino-compatible boards:  
 * Consult MOVI's user manual before connecting MOVI.
 *
 * WARNING:
 * This is an advanced example for people who have experience with MOVI 
 * and want to save memory or perform manual debugging.
 *
 * Compile and upload this program and open the Serial Console.
 * The serial console should be at 9600 baud, autoscroll and "Newline"
 * Consult the user manual for further information. 
 * 
 * Type 'HELP' for further information.
 * 
 */

String response="";
int curchar;
int baudrate=9600; // Good for most. Old Arduino UNO's might work more stable with 4800.

#ifdef ARDUINO_ARCH_AVR
SoftwareSerial mySerial(10,11); // Use default configuration for AVR (optional: you can rewire and change pins here)
#else
#define mySerial Serial1       // Use Serial1 for non-AVR boards.
#endif

void setup()  // setup
{
  // Open serial communications and wait for port to open:
  Serial.begin(baudrate); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Arduino Ready.");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(baudrate);
  mySerial.println("ABOUT");
  Serial.println("Type 'HELP' for a list of commands.");
  response="";
}


void loop() // run over and over
{
 if (mySerial.available()) { 
    curchar=mySerial.read();
    if (curchar=='\n') {
      Serial.println(response);
      if (response.indexOf("#1")>=0) {
        mySerial.println("SAY Sentence 1 recognized.");
        // Do something...
      }
      response="";
    } else {
      response+=(char) curchar;
    }
 }   
      
 if (Serial.available())
    mySerial.write(Serial.read());
    
}

