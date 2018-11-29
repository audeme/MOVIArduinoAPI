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
 *
 * This example sketch implements the game BattleShip using MOVI(tm).
 * It is based on an Instructable:
 * https://www.instructables.com/id/Voice-based-Battleship-Game-for-Arduino-with-AI/
 *
 * Please check there for details. 
 *
 * Circuitry:
 * Arduino MEGA2560 R3,  Arduino Uno R3 or Arduino Leonardo R3.
 * Connect speaker to MOVI.
 * IMPORTANT: Always use an external power supply with MOVI. 
 * We recommend connecting a headset to MOVI for this example.
 *
 * We also recommend downloading and printing  
 * the following play sheet:                                                       
 * https://commons.wikimedia.org/wiki/File:Battleships_Paper_Game.svg              
 *
 * Other Arduino-compatible boards:  
 * Consult MOVI's user manual before connecting MOVI.
 *
 * Note: This example uses the F() function to place Strings into flash memory.
 * Unfortunately, some GCC versions produce warnings when using this function. Just ignore them.
 *
 ****************************** Legal Notes ****************************************
 * Copyright (c) 2017 by Gerald Friedland of Audeme, LLC.                          *
 * All rights reserved.                                                            *
 * Redistribution and use in source and binary forms, with or without              *
 * modification, are PERMITTED provided that the following conditions are met:     *
 * 1. Redistributions of source code must retain the above copyright notice, this  *
 * list of conditions and the following disclaimer.                                *
 * 2. Redistributions in binary form must reproduce the above copyright notice,    *
 * this list of conditions and the following disclaimer in the documentation       *
 * and/or other materials provided with the distribution.                          *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          *
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR *
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES  *   
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;    *
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND     *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT      *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS   *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                    *
 *                                                                                 *
 * The views and conclusions contained in the software and documentation are those *
 * of the authors and should not be interpreted as representing official policies, *
 * either expressed or implied, of the FreeBSD Project, the author's employers or  *
 * the University of California.                                                   *
 *                                                                                 *
 * Author Contact: fractor@audeme.com                                              *      
 * Limited support for this project is available at http://www.audeme.com/forum    *
 ***********************************************************************************
 *
 **************************** Battleship Rules *************************************
 * The rules of the game are the turn-by-turn rules described on Wikipedia:        *
 * http://en.wikipedia.org/wiki/Battleship_(game)                                  *
 * and summarized as follows:                                                      *
 * One player is you and the other player is Arduino.                              *
 * The game is played on four grids, two for each player. The grids are 10×10 and  * 
 * the individual squares in the grid are identified by letter and number. In this *
 * game, the NATO alphabet is used to encode the letter, e.g. "alpha 10". On one   *
 * grid the player arranges ships and records the shots by the opponent. On the    *
 * other grid the player records their own shots. Before the game begins, each     *
 * player secretly arranges their ships on their primary grid. Each ship occupies  *
 * a number of consecutive squares on the grid, arranged either horizontally or    *
 * vertically. The number of squares for each ship is determined by the type of    *
 * the ship. The ships cannot overlap (i.e., only one ship can occupy any given    *
 * square in the grid) and around each ship is a 1-square buffer. The types and    *
 * numbers of ships allowed are the same for each player. This game uses the 1990  *
 * Milton Bradley version of the rules which specify the following ships:          *
 * 1 ship of 5 squares, 1 ship of 4 squares, 1 ship of 3 squares, 2 ships of 2     *
 * squares and 2 ships of 1 square.                                                *
 * After the ships have been positioned, the firing rounds of the game start. In   *
 * each round, each player takes a turn to announce a target square in the         *
 * opponent's grid which is to be shot at. The opponent announces whether or not   *
 * the square is occupied by a ship: "Miss" is announced if no ship is hit.        *
 * "Hit" is announced if a ship is hit but there are still unhit squares on the    *
 * same ship, and "Sunk" is announced if a hit resulted in all squares of a ship   *
 * being hit.                                                                      *
 * The attacking player notes hit or miss on their own "tracking" grid in order    *
 * to build up a picture of the opponent's fleet. After a player gets a hit, the   *
 * player continues shooting until he or she gets a miss.                          *
 * When all of a player's ships have been sunk, the game is over and the opponent  *
 * wins.                                                                           *
 **********************************************************************************/

#define DEBUG               // If defined, send output to Serial Monitor about various play states.

//#define PLAYMUSIC         // If defined, play sounds. Requires MOVI library 1.11 or higher, MOVI firmware 1.10 or higher and sound files uploaded to SD card.
                            // The sound files can be otained here:
                            // http://www.noiseforfun.com/waves/musical-and-jingles/NFF-zomboid.wav
                            // http://www.noiseforfun.com/waves/musical-and-jingles/NFF-bulletin.wav
                            // and (use free download option): http://www.playonloop.com/2015-music-loops/facing-destiny/
                            // Please respect copyright licenses, which depend on how you use the files and this program. 
 
/**** MOVI Specifics ****/ 
#include "MOVIShield.h"     // Include MOVI library, needs to be *before* the next #include

#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_PIC32)
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR and PIC32 architecture, other boards need to use Serial1 
#endif

MOVI recognizer(true);     // Get a MOVI object, true enables Serial Monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture

// The numbers for text to speech and speech to text.
const String Numbers[] = // Length must be >=GRIDSIZE, see below
{ 
  "ONE", 
  "TWO", 
  "THREE", 
  "FOUR", 
  "FIVE",
  "SIX",
  "SEVEN",
  "EIGHT",
  "NINE",
  "TEN"
};

// The letter for text to speech and speech to text. We use the NATO alphabet.
const String Letters[] = // Length must be >=GRIDSIZE, see below 
{ 
  "ALPHA", 
  "BRAVO", 
  "CHARLIE", 
  "DELTA", 
  "ECHO",
  "FOXTROT",
  "GOLF",
  "HOTEL",
  "INDIA",
  "JULIETT"
};

#ifdef RASPBERRYPI
//forward declare some of the functions
void initsimulation();
void addlikelihoods();
void newgame();
bool findhit(byte *xpos, byte *ypos, byte *orientation, byte shipsize);
void place(byte xpos, byte ypos, byte orientation, byte shipsize);
boolean checkplacement(byte orientation, byte shipsize, byte xposition, byte yposition);

#ifdef DEBUG
void printsimfield();
void printfield();
void printlikelihoods();

#endif
#endif

/**** Board Configuration ****/ 
const byte GRIDSIZE=10; // Size of playing field, cannot be larger than the sentences trained to MOVI (see above).

// Ship configuration
const byte ShipSizes[]={1,2,3,4,5}; // must be sorted and: no holes!
const byte ShipCounts[]={2,2,1,1,1}; // As defined by Bradley, 1990. This gives a very good qualization of the initial probability.  
const byte NUMSHIPS=7; // This number _must_ be the sum of ShipCounts[i] and >=length of ShipSizes!
const byte MAXSHIPLEN=5; // Easier to just define that, rather than deduct all the time. Must be: ShipSizes[(sizeof(ShipSizes)/sizeof(Byte))1]

// Handy helpers:
// Is a ship horizontal or vertical?
const byte HORIZONTAL=0;
const byte VERTICAL=1;

// Is it a user's turn or a computer's turn?
const boolean USER=false;
const boolean COMPUTER=true;

// Values for the squares of the playing field. Don't change the order of the values.
const byte EMPTY=0;  
const byte HIT=1;
const byte MISS=2;
const byte SHIP=4;

// The ships belonging to the computer are stored as an array of ships, as this 
// saves dynamic memory compared to storing the entire playing field of the computer.

// Parameters that define a ship.
struct ship {
  byte x;
  byte y;
  byte len;
  boolean hits[MAXSHIPLEN];
  boolean orientation;
  boolean sunk;
};

// The array of ships.
ship myShips[NUMSHIPS];

/**** Play Logic ****/ 
// Stores who's turn it is.
boolean Turn;

// Stores how many ships have been sunk already.
byte ShipsDown;

 /*************************** Outline of the AI approach ***************************
 * The AI is concerned with coming up with the computer's next move. The best way  *
 * to estimate the next move would be to calculate the probabilities of all        *
 * possible placements of the opponents' ships based on what was learned from      *
 * previous moves and then select the square that has the highest probabilty.      *
 * There are a couple problems with this approach. First, it is currently unknown  *
 * how to calculate the ship placement probabilities with an explicit formula. The *
 * reason for this is that Battleship can be reduced to bin packing, which is      *
 * NP complete [1]. Second, simulating all possible placements requires iterating  *
 * through about 2 billion possibilities [2] which is prohibitive on a desktop     *
 * computer but even more so on an Arduino.                                        *
 * Therefore I have chosen to implement an AI based on stochastic simulation.      *
 * Rather than systematically going through all possible configurations of the     *
 * board which, as explained, would take way too much time and possibly more       *
 * memory than is available on an Arduino), the algorithm creates a small random   *
 * sample of all possible placements given the knowledge from previous moves.      *
 * The hope is that the field with the highest probability of containing a ship    *
 * will be found by approximating the likelihoods for each square with a small     *
 * number of placements. In some quick experimental runs I found that 1000         *
 * placements usually creates a good approximation, more iterations is always      *
 * better of course and everything below 100 iterations feels like the moves       *
 * become random. Since I want this program to be able to run on an Arduino Uno, I *
 * chose 500 iterations, which creates a wait time of about 2-3 seconds.           *
 * If you have a stronger board, like the Arduino Zero or Due, or are OK to wait a * 
 * couple more seconds, I certainly recommend increasing the constant NUMITER to   *
 * 1000. Last but not least [1] also tells us that we cannot easily decide wether  *
 * a placement with a number of ships will alo for more ships to be placed and how *
 * many possibilities there are for these ships. Therefore, I chose to decide that *
 * randomly too. The algorithm tries to place a new ship NUMPLACINGATTEMPTS times. *
 * I default that number to 100 because my trial runs showed that to be a good. A  *
 * lose excuse for that number is that each ship has an expected 50 fields to be   * 
 * placed in and for each of them it's either horizontal or vertical. But again,   *
 * it's very wiggly excuse. The number is better explained with magic (and         *
 * experimentation). In anyways, you can safe some time by setting this number     *
 * down to a lower value, even though everything below 20 seemed unreliable.       *
 *                                                                                 *
 * [1] Merlijn Sevenster: BATTLESHIPS AS DECISION PROBLEM, ICGA Journal, Sep 2004  * 
 * https://pdfs.semanticscholar.org/b623/82bcebee19a7cfeb1cfc18e6b1b05c680dfb.pdf  *
 * [2] http://forums.xkcd.com/viewtopic.php?p=3186221                              *
 **********************************************************************************/

/**** AI ****/
const word NUMITER=500;            // Number of randomized simulations. Everything below 100 is not reliable. See text above.
const word NUMPLACINGATTEMPTS=100; // Number of placing attempts for each ship. Everything below 20 is not reliable. See text above.

word placingnumber; // A serial number that increases with each placing attempt. This allows to alternate through starting ship sizes.
word failedplacements;       // Number of failed placements. Serves as 'timeout'.

byte curmovex;               // The final move decision by the computer, x coordinate
byte curmovey;               // The final move decision the computer, y coordinate
 
byte shipcounts[NUMSHIPS];      // This will change during the game.

byte myField[GRIDSIZE][GRIDSIZE]; // In this field, we keep track of all the movements by the user. In the beginning, we use this field, to place our ships. 
byte simField[GRIDSIZE][GRIDSIZE];    // This field is used for the various simulation iterations.
word likelihoods[GRIDSIZE][GRIDSIZE]; // This field adds each ship placement from the simulation to calculate likelihoods of the most probably ship position

/************************** Code  ***************************************************/

#if defined(ARDUINO_ARCH_AVR) 
void arduinoreset() // Restarts program from beginning but does not reset the peripherals and registers
{
  asm volatile ("  jmp 0");  
} 
#endif


/**** Field manipulation methods ****/

// This method places all ships randomly onto the simulation field.
// If forreal is true, it will use the placement to fill the myships array with data.
boolean placeships(boolean forreal)
{
  word countnotok=0; // count placing tries (per ship)
  byte numship=0;    // helper
  placingnumber++;   // increments by one each call of this method to round-robin the ship sizes to not bias towards a certain ship size when simulating
  for (byte ships=0;ships<(sizeof(ShipSizes)/sizeof(byte));ships++) { // Iterate over the ship types
    byte i=(ships+placingnumber)%(sizeof(ShipSizes)/sizeof(byte));   // round robin
    for (byte j=0;j<shipcounts[i];j++) {                             // Iterate over the ship counts
      boolean ok=false;
      byte orientation;  
      byte xposition;    
      byte yposition;  
      while (!ok) {                                                 // Now try to place the ship 
        if (!forreal && findhit(&xposition, &yposition, &orientation,ShipSizes[i])) { // forreal assumes an empty field with no history, otherwise place ships onto hits first
          place(xposition,yposition,orientation,ShipSizes[i]);
          ok=true;
        } 
        else {                                        // if there is no more hit to cover under this ship size
          orientation=random(HORIZONTAL,VERTICAL+1);  // Get a random orientation...
          xposition=random(0,GRIDSIZE);               // ...and a random x and y position.
          yposition=random(0,GRIDSIZE); 
          if (checkplacement(orientation, ShipSizes[i], xposition, yposition)) { // check if the ship would fit on the field
            place(xposition,yposition,orientation,ShipSizes[i]);                 // if so place it
            if (forreal) {                                                       // if forreal then fill the myships array with data based on the placement
              myShips[numship].x=xposition;
              myShips[numship].y=yposition;
              myShips[numship].orientation=orientation;
              myShips[numship].len=ShipSizes[i];
              myShips[numship].sunk=false;                                      // a brand new ship is not sunk
              for (byte k=0;k<ShipSizes[i];k++) {                               // or hit in anyways
                myShips[numship].hits[k]=false;  
              }
              numship++;                                                        // keep track of how many shps are in myships already
            }
            ok=true;
          }    
        }
        if (!ok) {                                                             // if placement didn't work (many reasons)
          countnotok++;                                                        // count as failed attempt
          if (countnotok>NUMPLACINGATTEMPTS) {                                 // check if we have tried for too long
            failedplacements++;                                                // if so, bail out
            return false;
          }                                                                    // otherwise: try again!
        }
      }
    }
  }
  return true; 
}

// This method writes a ship onto the simulation field
// given position, orientation and size. No checks are made. 
void place(byte xpos, byte ypos, byte orientation, byte shipsize)
{
  if (orientation==HORIZONTAL) {
    for (int k=0;k<shipsize;k++) {
      simField[xpos+k][ypos]=SHIP;
    } 
  } 
  else {
    for (int k=0;k<shipsize;k++) {
      simField[xpos][ypos+k]=SHIP;
    }
  } 
}


// This method scans the simulation field for a hit and tries to place a ship with given length.
// If successful, findhit returns true and the variables pointed to are filled with
// the position and orientation of the ship.
bool findhit(byte *xpos, byte *ypos, byte *orientation, byte shipsize)
{
  byte k; // handy helper
  for (byte y=0;y<GRIDSIZE;y++) { // Search for hits...
    for (byte x=0;x<GRIDSIZE;x++) {
      if (simField[x][y]==HIT) {  // found a hit
        *xpos=x;                  // set coords to this position
        *ypos=y;
        *orientation=random(HORIZONTAL,VERTICAL+1);  // Get a random orientation... override if
        if ((x<GRIDSIZE-1) && simField[x+1][y]==HIT) *orientation=HORIZONTAL; // we know it's horizontal
        if ((x>0) && simField[x-1][y]==HIT) *orientation=HORIZONTAL; // we also know it's horizontal

        if ((y<GRIDSIZE-1) && simField[x][y+1]==HIT) *orientation=VERTICAL; // we know it's VERTICAL
        if ((y>0) && simField[x][y-1]==HIT) *orientation=VERTICAL; // we also know it's vertical

        // Now determine the minimum size of a ship, which is #hits + 1. (otherwise ship would be sunk)
        // Also determine the maximum size of a ship given various contraints, including hitting other ships, 
        // edges of the field.
        byte minsize=0;
        byte maxsize;
        byte minusroom=0;
        byte plusroom=0;
        if (*orientation==HORIZONTAL) {  // Scan for hits horizontally... 
          while (x+minsize<GRIDSIZE && simField[x+minsize][y]==HIT) minsize++; 
          maxsize=minsize;
          while (x+maxsize<GRIDSIZE && simField[x+maxsize][y]<=HIT) {   // estimate maximum size of a ship: Can't overlap a miss or another ship: forward
            maxsize++;
            plusroom++;
          }
          k=1;
          while (x-k>0 && simField[x-k][y]<=HIT) { // estimate maximum size of a ship: Can't overlap a miss or another ship: backward
            maxsize++;
            k++;
            minusroom++;
          }
          maxsize=min(maxsize,shipsize);

        } 
        else { // Scan for hits vertically...  (same thing as above)
          while (y+minsize<GRIDSIZE && simField[x][y+minsize]==HIT) minsize++;
          maxsize=minsize;
          while (y+maxsize<GRIDSIZE && simField[x][y+maxsize]<=HIT) { // estimate maximum size of a ship: Can't overlap a miss or another ship: forward
            maxsize++;
            plusroom++;
          }
          k=1;
          while (y-k>0 && simField[x][y-k]<=HIT) {  // estimate maximum size of a ship: Can't overlap a miss or another ship: backward
            maxsize++;
            k++;
            minusroom++;
          }
          maxsize=min(maxsize,shipsize);
        }
        // With all the dimension estimated, do a sanity check:
        if (shipsize<=minsize) return false; // don't place a ship smaller than fits into the hit vector +1 (it would be sunk otherwise!)
        if (shipsize>maxsize) return false; // don't place a ship larger than fits into the hit vector

        // If we have space, we need to overlap the hits with the ship in all ways possible. We will therefore randomly wiggle the ship back and force.
        if (minusroom==0) return true; // no wiggling because the ship fits perfectly. No need to wiggle forward as this is taken care by different ship sizes.	
  
        // Other then that, wiggle back maximally...
        signed char wiggleback=min(shipsize-minsize,minusroom);
        // ...and then random steps forward, maximally how many we can do.
        wiggleback-=min(random(0,wiggleback+1),plusroom);
        if (*orientation==HORIZONTAL) {
          *xpos-=wiggleback;
        } 
        else {
          *ypos-=wiggleback;
        } 
        return true; // Bingo
      }    
    }
  }
  // No hit found!
  return false;
}

// This method checks if the placement of a ship of the given size at the given coordinates with the given orientation is legal
// according to the rules.

boolean checkplacement(byte orientation, byte shipsize, byte xposition, byte yposition)
{
  if (orientation==HORIZONTAL) {
    if (xposition+shipsize>GRIDSIZE) return false; // We can't place the ship over an edge.
    for (byte k=0;k<shipsize;k++) { // Now check if the ship would overlap another thing!
      if (simField[xposition+k][yposition]!=EMPTY) return false;
    }
    if (xposition+shipsize<GRIDSIZE) {   // We are not at an edge: Let's check for another ship to the right.
      if (simField[xposition+shipsize][yposition]==SHIP || simField[xposition+shipsize][yposition]==HIT) return false;   
      if (yposition+1<GRIDSIZE) {        // Check the lower right corner for another ship if it's not the edge
        if (simField[xposition+shipsize][yposition+1]==SHIP || simField[xposition+shipsize][yposition+1]==HIT) return false; 
      }
      if (yposition>0) {         // Check the upper right corner for another ship if it's not the edge
        if (simField[xposition+shipsize][yposition-1]==SHIP || simField[xposition+shipsize][yposition-1]==HIT) return false; 
      }
    }
    if (xposition>0) {           // We are not at an edge: Let's check for another ship to the left.
      if (simField[xposition-1][yposition]==SHIP || simField[xposition-1][yposition]==HIT) return false;
      if (yposition+1<GRIDSIZE) {         // Check the lower left corner for another ship if it's not the edge
        if (simField[xposition-1][yposition+1]==SHIP || simField[xposition-1][yposition+1]==HIT) return false; 
      }
      if (yposition>0) {         // Check the upper left corner for another ship if it's not the edge
        if (simField[xposition-1][yposition-1]==SHIP || simField[xposition-1][yposition-1]==HIT) return false; 
      } 
    }
    // Check for neighboring ships...
    if (yposition>0) { // ... above this ship (if not at an edge)
      for (byte k=0;k<shipsize;k++) { 
        if (simField[xposition+k][yposition-1]==SHIP || simField[xposition+k][yposition-1]==HIT) return false;
      }  
    }
    if (yposition+1<GRIDSIZE) { // ... below this ship (if not at an edge)
      for (byte k=0;k<shipsize;k++) { 
        if (simField[xposition+k][yposition+1]==SHIP || simField[xposition+k][yposition+1]==HIT) return false;
      }  
    }
  } 
  else {
    if (yposition+shipsize>GRIDSIZE) return false; // We can't place the ship over an edge.
    for (byte k=0;k<shipsize;k++) { // Now check if the ship would overlap another thing!
      if (simField[xposition][yposition+k]!=EMPTY) return false;
    } 
    if (yposition+shipsize<GRIDSIZE) {  // We are not at an edge: Let's check for another ship below us.
      if (simField[xposition][yposition+shipsize]==SHIP || simField[xposition][yposition+shipsize]==HIT) return false; 
      if (xposition+1<GRIDSIZE) {         // Check the lower right corner for another ship if it's not the edge
        if (simField[xposition+1][yposition+shipsize]==SHIP || simField[xposition+1][yposition+shipsize]==HIT) return false; 
      }
      if (xposition>0) {         // Check the lower left corner for another ship if it's not the edge
        if (simField[xposition-1][yposition+shipsize]==SHIP || simField[xposition-1][yposition+shipsize]==HIT) return false; 
      }
    }
    if (yposition>0) {  // We are not at an edge: Let's check for another ship above us.
      if (simField[xposition][yposition-1]==SHIP || simField[xposition][yposition-1]==HIT) return false;
      if (xposition+1<GRIDSIZE) {         // Check the upper right corner for another ship if it's not the edge
        if (simField[xposition+1][yposition-1]==SHIP || simField[xposition+1][yposition-1]==HIT) return false; 
      }
      if (xposition>0) {         // Check the upper left corner for another ship if it's not the edge
        if (simField[xposition-1][yposition-1]==SHIP || simField[xposition-1][yposition-1]==HIT) return false; 
      }  
    }
    // Check for neighboring ships...
    if (xposition>0) { // ... above this ship (if not at an edge)
      for (byte k=0;k<shipsize;k++) { 
        if (simField[xposition-1][yposition+k]==SHIP || simField[xposition-1][yposition+k]==HIT) return false;
      }  
    }
    if (xposition+1<GRIDSIZE) { // ... below this ship (if not at an edge)
      for (byte k=0;k<shipsize;k++) { 
        if (simField[xposition+1][yposition+k]==SHIP || simField[xposition+1][yposition+k]==HIT) return false;
      }  
    }
  }
  // Passed all tests!
  return true;   
}


// Mark a user's ship at coordinate x,y as sunk (needs to be marked as HIT).
// What we could do but don't do is mark the surroundings of a ship as MISS.
// The reason for not doing this is that it barely matters but it allows for some 
// wiggle room in some cases of some speech recognition misunderstandings.
void marksunk(byte x, byte y)
{
  byte shipsize=1;      // minimum shipszize is 1
  byte orientation=0;   //Let's figure out the orientation of that ship (similar to findhit)
  if ((x<GRIDSIZE-1) && myField[x+1][y]==HIT) {
    orientation=HORIZONTAL; // we know it's horizontal
    shipsize++;
  }
  if ((x>0) && myField[x-1][y]==HIT) {
    orientation=HORIZONTAL; // we know it's horizontal
    shipsize++;
  }
  if ((y<GRIDSIZE-1) && myField[x][y+1]==HIT) {
    orientation=VERTICAL; // we know it's vertical
    shipsize++;
  }
  if ((y>0) && myField[x][y-1]==HIT) {
    orientation=VERTICAL; // we know it's vertical
    shipsize++;
  }
  if (shipsize>1) { // we have an orientation, now scan left and right/up and down of the ship and mark all hits as SHIP.
    shipsize=0;
    if (orientation==HORIZONTAL) { // go right
      while (x+shipsize<GRIDSIZE && myField[x+shipsize][y]==HIT) {
        myField[x+shipsize][y]=SHIP;
        shipsize++;
      }
      byte k=1; 
      while (x>0 && myField[x-k][y]==HIT) {
        shipsize++;
        myField[x-k][y]=SHIP;
        k++;
      } 
    } 
    else {
      while (y+shipsize<GRIDSIZE && myField[x][y+shipsize]==HIT) {
        myField[x][y+shipsize]=SHIP;
        shipsize++;
      }
      byte k=1; 
      while (y>0 && myField[x][y-k]==HIT) {
        shipsize++;
        myField[x][y-k]=SHIP;
        k++;
      }
    }  
  } 
  else { // if it's only a size 1, mark the HIT as SHIP. Done.
    myField[x][y]=SHIP;
  } 
  if (shipcounts[shipsize-1]>0) {  // Subtract the ship from shipcounts so the simulation doesn't try to place it (we already know where it is).
    shipcounts[shipsize-1]--;
  } 
  else {                          // If we find out the user reported sunken ships wrongly, everything goes out of the window. So let's tell the user and abort. 
    recognizer.say(F("You placed your ships against the rules. Resetting."));
    newgame();
  }  
}

/**** Main AI methods ****/


// This methods determines the next move by implementing the AI as described above.
// A side effect of this method is changing the variables: curmovex and curmovey, denmoting the square chosen to attack.
void nextmove()
{
  word count=0;          // reset all counts
  placingnumber=0;
  failedplacements=0;

  recognizer.say(F("My move is:")); // Tell the user, that the computer is moving...

#ifdef DEBUG
  initsimulation();
  Serial.println(F("I know about you:"));
  printsimfield();
#endif
    
  for (int i=0;i<GRIDSIZE;i++) {    // set likelihood matrix to 0
    for (int j=0;j<GRIDSIZE;j++) {
      likelihoods[i][j]=0; 
    }
  }
  while (count<NUMITER) {         // Place ships into simulation field NUMITER times
    initsimulation();             // start with current field configuration (myFIeld)
    if (placeships(false)) {      // place all ships remaining
      addlikelihoods();           // if successfull, take the result and add it into the likelihood matrix
    }
    count++;
  }

  byte maxx=0;                   // Now scan the likelihood matrix for the highest number that is not occupied by a MISS or SHIP. 
  byte maxy=0;                   // Note that in the worst case the likelihood matrix contains all 0s. Then the move becomes random but stays valid.
  word maxlikelihood=0;
  for (int y=0;y<GRIDSIZE;y++) { 
    for (int x=0;x<GRIDSIZE;x++) {
      if ((myField[x][y]==EMPTY) && (likelihoods[x][y]>maxlikelihood)) {
        maxlikelihood=likelihoods[x][y];
        maxx=x;
        maxy=y;
      }
    }
  }
  curmovex=maxx;               // return the coordinates of that number
  curmovey=maxy;
  recognizer.ask(Letters[maxy]+" "+Numbers[maxx]); // and announce them to the user in the form <letter><number>.
}


/**** Determine Game States ****/


// Game over theater
void gameover()
{
  #ifdef PLAYMUSIC
  recognizer.play(F("NFF-Zomboid.wav"));
  #endif
  recognizer.say(F("Game Over. You lost."));  
  delay(5000);
}

// Game win theater
void gamewin()
{
  #ifdef PLAYMUSIC
  recognizer.play(F("NFF-bulletin.wav"));
  #endif
  recognizer.say(F("Congratulations. You won!"));  
  delay(5000);
}

// Find out if a given attack by a user on xpos and ypos resulted in a hit.
boolean isHit(byte xpos, byte ypos)
{
  for (byte i=0;i<NUMSHIPS;i++) {             // Iterate through myShips
    for (byte k=0;k<myShips[i].len;k++) {
      if (myShips[i].orientation==HORIZONTAL) {
        if (myShips[i].y==ypos && myShips[i].x+k==xpos) { // coordinate match!
          if (myShips[i].sunk) return false; // ship already on ground
          if (myShips[i].hits[k]==true) return false; // ship already hit at that spot
          myShips[i].hits[k]=true; // mark hit
          return true;
        }
      } 
      else { // VERTICAL
        if (myShips[i].y+k==ypos && myShips[i].x==xpos) { // coordinate match!
          if (myShips[i].sunk) return false; // ship already on ground
          if (myShips[i].hits[k]==true) return false; // ship already hit at that spot
          myShips[i].hits[k]=true; // mark hit
          return true;
        }
      }  
    }
  }
  return false; // no ship matched coordinates
}

// Find out if a given attack by a user on xpos and ypos resulted in a sunken ship.
// If so mark the ship as sunk. 
boolean isSunk(byte xpos, byte ypos)
{
  for (byte i=0;i<NUMSHIPS;i++) {
    if (myShips[i].sunk==false) { // if ship had been marked as sunk before, ignore
      boolean sunk=true;
      for (byte k=0;k<myShips[i].len;k++) { // otherwise go through the hits
        sunk=sunk & myShips[i].hits[k];       
      }
      if (sunk) { // if all hits positive, it's a newly sunk ship
        myShips[i].sunk=true;  // mark as sunk
        return true;          // report true
      }
    }
  }
  return false; // puh...
}

// Find out if the computer lost the game (all ships are sunk)
boolean isLost()
{
  boolean lost=true;
  for (byte i=0;i<NUMSHIPS;i++) {
    lost=lost & myShips[i].sunk;
  }
  return lost;
}

/**** Helper Methods ****/

// Clear the fields.
void clearfields()
{
  for (int i=0;i<GRIDSIZE;i++) { // Clear both fields
    for (int j=0;j<GRIDSIZE;j++) {
      myField[i][j]=EMPTY;
      simField[i][j]=EMPTY;
    }
  }
}

// Calculate the likelihoods
void addlikelihoods()
{
  for (byte i=0;i<GRIDSIZE;i++) { // Sum up the possible positions from simField
    for (byte j=0;j<GRIDSIZE;j++) {
      if (simField[i][j]==SHIP) likelihoods[i][j]+=simField[i][j]/SHIP;
    }
  }
}

// Initialize the simulation field by compying the permanent field into it 
void initsimulation()
{
  for (int i=0;i<GRIDSIZE;i++) { // Copy myField to simField
    for (int j=0;j<GRIDSIZE;j++) {
      simField[i][j]=myField[i][j];   
    }
  }
}

/**** Debug methods ****/

#ifdef DEBUG

// Print the simulation field
void printsimfield()
{
  for (int i=0;i<GRIDSIZE;i++) { 
    for (int j=0;j<GRIDSIZE;j++) {
      Serial.print(simField[j][i]);
      Serial.print(F(" "));
    }
    Serial.println();
  }
  Serial.println(F("---"));
}

// Print the actual field
void printfield()
{
  for (int i=0;i<GRIDSIZE;i++) { // print field
    for (int j=0;j<GRIDSIZE;j++) {
      Serial.print(myField[j][i]);
      Serial.print(F(" "));
    }
    Serial.println();
  }
  Serial.println(F("---"));
}

// Print the likelihood matrix
void printlikelihoods()
{
  for (int i=0;i<GRIDSIZE;i++) { // print field
    for (int j=0;j<GRIDSIZE;j++) {
      Serial.print(likelihoods[j][i]);
      Serial.print(F(" "));
    }
    Serial.println();
  }
  Serial.println(F("---"));
}

#endif


/*** Game Play ****/

void newgame()
{ 
  #ifdef PLAYMUSIC // If enabled and available
  recognizer.play(F("POL-facing-destiny-short.wav")); // play some intro music.
  delay(1000);
  #endif
  
  recognizer.say(F("Place your ships!"));  // Instruct user 
          
  for (byte k=0;k<NUMSHIPS;k++) shipcounts[k]=ShipCounts[k]; // reset ship counts and playing fields.
  ShipsDown=0;
  clearfields();

  randomSeed(analogRead(0)); // Initialize random generator with a read on an unconnected pin. NOTE: The pin must be unconnected!
  placeships(true); // Place the ships onto simulation field. True means place ships for real, i.e. fill the myships array representing the computer's ships. 

  #ifdef DEBUG  // If enabled: show the result of placing
  Serial.println(F("My field (don't look):"));
  printsimfield();
  #endif 
  
  Turn=USER; // It's the user's turn. Let's go. The user can say "pass" to the let computer start. 
  recognizer.ask(F("You start. Say a coordinate. For example 'alpha one' or 'charlie three'. You can also say 'pass' or 'surrender'."));  
}


/**** Arduino setup ****/

void setup()  
{
  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  // recognizer.callSign("ARDUINO"); // Commented out because a call sign is not important for this game. 
  
  for (int i=0;i<GRIDSIZE;i++) {  // Now make the grid coordinates sentences in a way that result returns the grid position (see loop()). 
    for (int j=0;j<GRIDSIZE;j++) {
     recognizer.addSentence(Letters[i]+" "+Numbers[j]);
    }
  }

  // Add Additional control sentences.  
  recognizer.addSentence(F("Miss")); // Sentence 101 
  recognizer.addSentence(F("Hit"));  // Sentence 102  
  recognizer.addSentence(F("Sunk")); // Sentence 103
  recognizer.addSentence(F("Pass")); // Sentence 104
  recognizer.addSentence(F("Repeat")); // Sentence 105
  recognizer.addSentence(F("Surrender")); // Sentence 106
  recognizer.train();  // Train (may take 20seconds) 
  
  //*/

  //  recognizer.setThreshold(5);			// For this game we recommend a headset microphone! Uncomment and set to a higher value 
                                      // (valid range 2-95) if you have a problems with the recognition. 
  
  // Done with MOVI stuff. Start a new game!    
  newgame();
}

/**** Arduino loop ****/
void loop() // run over and over
{
  signed int res=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see docs)
  if (res>0 && res<=100) {          // MOVI reports a coordinate was said  
    if (Turn==COMPUTER) {           // If it's the computers turn, then we ignore it...
      recognizer.ask(F("Please say: 'hit', 'miss' or 'sunk' or 'repeat'"));  // and complain.
    } 
    else {                          // Otherwise:
      byte xpos=(res-1)%GRIDSIZE;  // Extract x and
      byte ypos=(int) (res-1)/GRIDSIZE; // y coordinate
      recognizer.say(Letters[ypos]+" "+Numbers[xpos]); // and reaffirm to the user.
      if (isHit(xpos,ypos)) {           // Find if it is a hit. 
        recognizer.say(F("Hit"));       // if yes: announce and
        if (isSunk(xpos,ypos)) {        // check if it's also a sink.
          recognizer.say(F("and sunk!")); // if yes: announce and
          if (isLost()) {               // check if we already lost the game!
            gamewin();                  // if yes: announce the oppnent's win
            newgame();                  // and start a new game. 
          }    
        }
      } 
      else {                        // otherwise:
        recognizer.say(F("Miss"));  // announce it's a miss 
        Turn=COMPUTER;              // and pass the turn to the computer
      }
      if (Turn==COMPUTER) {
        nextmove();                 // to make a move.
      } 
      else { 
        recognizer.ask(F("Your move?")); // If it's a hit or sunk: ask for the next move  
      } 
    }
  }  
  // MOVI reports...
  if (res==101) { // ..."MISS" was said.
    if (Turn==USER) { // If it's a users turn, then we ignore it and
      recognizer.ask(F("Please say a coordinate.")); // ask for a coordinate
    } 
    recognizer.say(F("Miss")); // reaffirm to the user.
    myField[curmovex][curmovey]=MISS; // Otherwise: record the miss
    Turn=USER;                        // Give the user the turn
    recognizer.ask(F("Your move?"));  // ask for coordinates.
  }
  if (res==102) { // ... "HIT" was said. 
    if (Turn==USER) { // If it's a users turn, then we ignore it and
      recognizer.ask(F("Please say a coordinate.")); // ask for a coordinate
    } 
    recognizer.say(F("Hit")); // reaffirm to the user.
    myField[curmovex][curmovey]=HIT; // Otherwise: record the hit
    nextmove();                      // Make the next move!
  }
  if (res==103) { // ..."SUNK" was said.
    if (Turn==USER) { // If it's a users turn, then we ignore it and
      recognizer.ask(F("Please say a coordinate.")); // ask for a coordinate
    } 
    else {
      recognizer.say(F("Sunk")); // reaffirm to the user.
      myField[curmovex][curmovey]=HIT; // Otherwise: record the hit
      marksunk(curmovex,curmovey);     // Mark the ship as sunk
      ShipsDown++;                     // Count the ships downed...
      if (ShipsDown>=NUMSHIPS) {       // ...and check if it's gameover
        gameover();                    // if yes: gameover()!
        newgame();                     // and new game!
      } 
      else {
        nextmove();                      // if not: Make the next move!
      }
    }
  }
  if (res==104) { // ..."PASS" was said.
    if (Turn==COMPUTER) { // If it's a computer's turn, then we ignore it and
      recognizer.ask(F("Please say: 'hit', 'miss' or 'sunk' or 'repeat'."));  // ask for a result
    } 
    else {            // Otherwise:
      recognizer.say(F("Pass")); // reaffirm to the user.
      Turn=COMPUTER;  // obey. Turn to computer
      nextmove();     // and make next move.
    } 
  }
  if (res==106) {     // ..."SURRENDER" was said.
    recognizer.say(F("Surrender")); // reaffirm to the user.
    gameover();       // OK. gameover()
    newgame();        // Start a new game.
  } 
  if (res==UNKNOWN_SENTENCE || res==SILENCE || res==105) { // If we hit an unknown sentence or "REPEAT" (always possible)
    if (Turn==COMPUTER) { // Check who's turn it is.
      recognizer.say(Letters[curmovey]+" "+Numbers[curmovex]); // If computer, repeat the last move
      recognizer.ask(F("Please say: 'hit', 'miss', 'sunk' or 'repeat'"));  // and ask for a result
    } 
    else {                                                    // if user
      recognizer.ask(F("Please say a coordinate or 'pass' or 'surrender'.")); // ask for a coordinate
    } 
  }
}
