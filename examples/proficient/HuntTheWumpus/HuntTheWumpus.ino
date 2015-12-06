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
 * This example sketch implements the game Hunt the Wumpus using MOVI(tm).
 * Is is based on an Adafruit project that uses an LCD display and buttons:
 * https://learn.adafruit.com/arduino-hunt-the-wumpus/
 *
 * We have kept some of the original functions and comments even though they
 * are not needed or don't apply anymore to allow for comparison between this
 * and the original LCD-based version.
 * 
 * Circuitry:
 * Arduino MEGA2560 R3,  Arduino Uno R3 or Arduino Leonardo R3.
 * Connect speaker to MOVI.
 * IMPORTANT: Always use an external power supply with MOVI. 
 * We recommend connecting a headset to MOVI for this example.
 *
 * Other Arduino-compatible boards:  
 * Consult MOVI's user manual before connecting MOVI.
 *
 * Note: This example uses the F() function to place Strings into flash memory.
 * Unfortunately, some GCC versions produce warnings when using this function. Just ignore them.
 *
 * Original copyright notice:
 * // -------------------------------------------------------------------------------
 * // An Arduino sketch that implements the classic game Hunt the Wumpus
 * //
 * // http://en.wikipedia.org/wiki/Wumpus
 * //
 * // In this variant, there are twenty rooms layed out like a d20; therefore, each room is connected
 * // to three other rooms.  Two rooms are bottomless pits; if you fall in to a bottomless pit you lose.
 * // Two rooms contain giant bats; if you enter a room with a giant bat it will pick you up and carry
 * // you to a random room.  One room contains a wumpus; if you bump into the Wumpus, it will eat you and
 * // you lose.  You have 2 arrows; if you shoot the Wumpus, you win.  If you run out of arrows, you lose.
 * //
 * // MIT license.
 * //
 * // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * // IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * // FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * // AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * // LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * // OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * // SOFTWARE.
 * // -------------------------------------------------------------------------------- 
 *
 *
 */

#include "MOVIShield.h"     // Include MOVI library, needs to be *before* the next #include

#ifdef ARDUINO_ARCH_AVR 
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR architecture, other boards need to use Serial1 
#endif

// #define CHEATING // Define cheating if you want well-defined locations for player, wumpus, pits and bats that are printed on the serial monitor. Useful for debugging.

MOVI recognizer(true);            // Get a MOVI object, parameter true enables serial monitor interface, tx and rx define alternate communication pins on AVR architecture boards

// Map of the cavern layout.
/*
   The first dimension represents the cave number and the second dimension represents
 the connected caves.  NOTE - C indexing starts at 0, but cave number display starts
 at 1 - so the first line below means cave 1 is connected to caves 2, 5, and 8.
 */

const uint8_t cave[20][3] = { 
  { 2,  8, 14},    //  0
  { 7, 13, 19},    //  1
  {12, 18,  0},    //  2
  {16, 17, 19},    //  3
  {11, 14, 18},    //  4
  {13, 15, 18},    //  5
  { 9, 14, 16},    //  6
  { 1, 15, 17},    //  7
  { 0, 10, 16},    //  8
  { 6, 11, 19},    //  9
  { 8, 12, 17},    // 10
  { 4,  9, 13},    // 11
  { 2, 10, 15},    // 12
  { 1,  5, 11},    // 13
  { 0,  4,  6},    // 14
  { 5,  7, 12},    // 15
  { 3,  6,  8},    // 16
  { 3,  7, 10},    // 17
  { 2,  4,  5},    // 18
  { 1,  3,  9}};   // 19

enum HazardType { 
  EMPTY=0, BAT=1, PIT=2, WUMPUS=4 };

HazardType check_for_hazards(uint8_t room_idx);

// Index in the map of the room the player is in.
uint8_t player_room;

// Index in the map of the room the wumpus is in.
uint8_t wumpus_room;

// Index in the map of the room the first pit is in.
uint8_t pit1_room;

// Index in the map of the room the second pit is in.
uint8_t pit2_room;

// Index in the map of the room the first bat is in.
uint8_t bat1_room;

// Index in the map of the room the second bat is in.
uint8_t bat2_room;

// Count of how many arrows the player has left.
uint8_t arrow_count;

// Index in the map of the room the arrow is shot into.
/*
  This index is only valid/current during the state changes
 associated with firing an arrow into a room.
 */
uint8_t arrow_room;

// The current state.
void (*state)() = NULL;

// The state that the game was in prior to the current state.
void (*last_state)() = NULL;


// Perform one time setup for the game and put the game in the splash screen state.
void setup() 
{
  // MOVI setup ---------
  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  // The following training functions are "lazy" and only do 
  // something if there are changes. Note: training can only be performed in setup().
  // They can be commented out to save memeory and startup time if no training is needed.
  // For the smaller Arduino's we use the F() fucntion based addSentence, ask, and say methods
  // in this example. For an explanation, see: https://www.arduino.cc/en/Tutorial/Memory 
  recognizer.callSign("HUNTER"); // Train callsign HUNTER (may take 20 seconds)

  // In theory it is ok to train every possible input sentence in this game (20 caves x 2 actions + status + arrows = 42 sentences). However, it is more efficient
  // to program training the numbers as individual words (just as one sentence) and then training the actions as sentence stubs. Training sentence stubs instead of
  // just putting the action words with the numbers allows MOVI to learn the sequence of the action words which increases accuracy. Later, we will need to 
  // concatenate the sentence stubs with the numbers ourselves by using getResult() and doing string processing (See loop()). This way, we can also be more 
  // flexible with actions and introduce alternative wordings (such as 'move' and 'go'). Having said that, if accuracy is an issue due to noise or other 
  // environmental factors, training each individual sentence gives better results. 
  recognizer.addSentence(F("one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen sixteen seventeen eighteen nineteen twenty")); // Add all numbers we need as a sentence
  recognizer.addSentence(F("shoot at cave"));  // Add sentence stub 1 as a sentence
  recognizer.addSentence(F("go to cave"));  // Add sentence stub 2 as a sentence 
  recognizer.addSentence(F("move to cave"));  // Add another sentence stub as alternative to sentence stub 2
  recognizer.addSentence(F("where am i"));  // Add sentence 5
  recognizer.addSentence(F("arrows"));  // Add sentence 6
  recognizer.train();                           // Train (takes about 20seconds)
  //*/

  recognizer.responses(false);                  // Since we bypass MOVI's sentence estimation, we don't want to expose the user to unexplainable messages
  // recognizer.setThreshold(5);                 // uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
  /// --------- MOVI setup end

  // Use Pin 0 to seed the random number generator
  randomSeed(analogRead(0));

  // Initial game state
  state = begin_splash_screen;
}


// Main loop of execution.
void loop() 
{
  /* This loop looks slightly different because we need to go
   for the RAW WORDS as we didn't train full sentences. The 
   pittfall is: You can either go for the raw words or you can
   use MOVI's interpretation. The reason for this is that both 
   raw words and MOVI's guess are sent as separate events and so
   results would be interpreted twice if we worked with both models
   simultaneaously */
  String result;
  int event=recognizer.poll(); // Get event from MOVI
  if (event==RAW_WORDS) { // We've got some result we can work with.
    result=recognizer.getResult(); // get the raw words. They will all be UPPERCASE
    if (result.indexOf("WHERE")>-1) { // if the result contains the word WHERE, assume it's about that
      gamestatus();  
    } 
    else if (result.indexOf("ARROWS")>-1) { // if the result contains the word ARROWS, assume it's about that
      recognizer.say("You have "+String(arrow_count)+ " arrows.");  
    } 
    else { // It must contain a number somehow...
      int room=moviResult2Number(result); //... so let's get that!
      bool caveConnected=false; // this stores if a user given command is valid. For now, let's assume it's not.
      room--; // adjust for array index (starting at zero but cave numbers start at one)
      for (int i=0;i<3;i++) { // Now let's check if that cave number is connected to the room we are currenlty in 
        if (cave[player_room][i]==room) { // If so, set ok=true.
          caveConnected=true;
          break;  
        }
      }   
      if (!caveConnected) { // if cave is not connected, give error and game status.
        if (room==player_room) recognizer.say(F("You cannot go to or shoot at the cave you are in."));
        else if (room==-2) recognizer.say(F("I didn't understand the cave number you said.")); // It's -1 from str2num and -1 from room--, so -2
        else recognizer.say("Cave "+String(room+1)+" not connected."); // Again cave numbering is one off
        gamestatus();
      } 
      else {
        if (result.indexOf("GO TO")>-1 || result.indexOf("MOVE")>-1) { // All good
          player_room = room;       // execute
          state = begin_move_room;    
        } 
        else if (result.indexOf("SHOOT AT")>-1) { // Also all good
          arrow_room = room;
          state = begin_shooting_arrow;    
        } 
        else { // Otherwise, the user needs help
          recognizer.say(F("Invalid command. Valid commands are: where am i, go to cave cavenumber, move to cave cavenumber, shoot at cave cavenumber, and arrows."));
        }
      }
    }    
  }
  if (event==SILENCE) { // then we got silence 
    recognizer.say(F("You wait and time passes by.")); 
  } 

  // Record time of state change so animations
  // know when to stop
  if (last_state != state) {
    last_state = state;
    state();
  }
}

// MOVI helper function to convert number words as returned by MOVI to actual numbers.
int moviResult2Number(String string)
{
  if (string.indexOf("ZERO")>-1) {
    return 0;
  }
  if (string.indexOf("TEN")>-1) {
    return 10;
  }
  if (string.indexOf("ELEVEN")>-1) {
    return 11;
  }
  if (string.indexOf("TWELVE")>-1) {
    return 12;
  }
  if (string.indexOf("THIRTEEN")>-1) {
    return 13;
  }
  if (string.indexOf("FOURTEEN")>-1) {
    return 14;
  }
  if (string.indexOf("FIFTEEN")>-1) {
    return 15;
  }
  if (string.indexOf("SIXTEEN")>-1) {
    return 16;
  }
  if (string.indexOf("SEVENTEEN")>-1) {
    return 17;
  }
  if (string.indexOf("EIGHTEEN")>-1) {
    return 18;
  }
  if (string.indexOf("NINETEEN")>-1) {
    return 19;
  }
  if (string.indexOf("TWENTY")>-1) {
    return 20;
  }
  if (string.indexOf("ONE")>-1) {
    return 1;
  }
  if (string.indexOf("TWO")>-1) {
    return 2;
  }
  if (string.indexOf("THREE")>-1) {
    return 3;
  }
  if (string.indexOf("FOUR")>-1) {
    return 4;
  }
  if (string.indexOf("FIVE")>-1) {
    return 5;
  }
  if (string.indexOf("SIX")>-1) {
    return 6;
  }
  if (string.indexOf("SEVEN")>-1) {
    return 7;
  }
  if (string.indexOf("EIGHT")>-1) {
    return 8;
  }
  if (string.indexOf("NINE")>-1) {
    return 9;
  }
  return -1; 
}

// -------------------------------------------------------------------------------
// Game logic and Helpers
// -------------------------------------------------------------------------------

// Print a cave number to the lcd.
/*
  Print a cave number to the lcd by adding one to the index number and left
 padding with a single space if needed to make the cave number take up two
 places.
 
 \param cave_idx the index of the cave in the array
 */
void print_cave_number(uint8_t cave_idx)
{
  recognizer.say(String(cave_idx+1));
  // lcd.print(cave_idx + 1);
}

// Check a cave index for hazards.
HazardType check_for_hazards(uint8_t room_idx)
{
  if (room_idx == bat1_room || room_idx == bat2_room) {
    return BAT;
  } 
  else if (room_idx == pit1_room || room_idx == pit2_room) {
    return PIT;
  } 
  else if (room_idx == wumpus_room) {
    return WUMPUS;
  } 
  else {
    return EMPTY;
  }
}

// Initial game state, draw the splash screen.
void begin_splash_screen() 
{
  // lcd.print(F("HUNT THE WUMPUS"));
  recognizer.say(F("Hunt the Wumpus!"));
  state = start_game;
}


// Initialize a new game.
/*
  Randomize locations and reset variables.
 */
void start_game() 
{
  wumpus_room = random(20);
  pit1_room = random(20);
  pit2_room = random(20);
  bat1_room = random(20);
  bat2_room = random(20);

  do {
    player_room = random(20);
  } 
  while(player_room==wumpus_room ||  player_room==pit1_room || player_room==pit2_room ||  player_room==bat1_room || player_room==bat2_room);

  arrow_count = 2;

#ifdef CHEATING  /***** Cheating starts ***/
  player_room=0;
  bat1_room=12;
  bat2_room=18;
  wumpus_room=7;
  pit1_room=13;
  pit2_room=19;
  arrow_count=255;

  Serial.println(F("SPOILER ALERT:"));
  Serial.println("WUMPUS:"+String(wumpus_room+1));
  Serial.println("Bats1:"+String(bat1_room+1));
  Serial.println("Bats2:"+String(bat2_room+1));
  Serial.println("Pit1:"+String(pit1_room+1));
  Serial.println("Pit2:"+String(pit2_room+1));
  Serial.println(F("Winning path: go to cave 3, go to cave 13, shoot at cave 8."));
  delay(100);
#endif  /******* Cheating ends *****/

  state = begin_move_room;
}

// Delay to show a status before continuing with the room move.
void status_delay() 
{
  state = begin_move_room;
}

// Check for hazards when a player changes rooms
void begin_move_room() 
{
  switch (check_for_hazards(player_room)) {    
  case BAT:
    state = begin_bat_move;
    break;

  case PIT:
    state = game_over_pit;
    break;

  case WUMPUS:
    state = game_over_wumpus;
    break;

  default:
    state = enter_new_room;
    break;
  }
}


// -------------------------------------------------------------------------------
// Bat states
// -------------------------------------------------------------------------------

void begin_bat_move() 
{
  recognizer.say(F("Bats! Lifting off into the air...")); 
  state = animate_bat_move;
}

void animate_bat_move() 
{
  recognizer.say(F("flap flap flap."));
  state = end_bat_move;

#ifdef CHEATING  /***** Cheating starts ***/
  state = end_bat_move_cheat;
#endif  /***** Cheating ends ***/
}

void end_bat_move() 
{

  if (player_room == bat1_room) {
    bat1_room = random(20);
  } 
  else  {
    bat2_room = random(20);
  }
  player_room = random(20);

  state = begin_move_room;
}

/***** Cheating starts ***/
void end_bat_move_cheat() 
{
  player_room = 1;

  state = begin_move_room;
}
/***** Cheating ends ***/

// -------------------------------------------------------------------------------
// Moving states / functions
// -------------------------------------------------------------------------------

void enter_new_room() 
{
  int adjacent_hazards = EMPTY;

  recognizer.say("Cave "+String(player_room+1));
  //  print_cave_number(player_room);

  for (int i=0; i<3; i++) {
    adjacent_hazards |= check_for_hazards(cave[player_room][i]);
  }

  if (adjacent_hazards & WUMPUS) {
    recognizer.say(F("It smells like rotten meat --."));
  } 

  if (adjacent_hazards & BAT) {
    recognizer.say(F("I hear bats nearby. -- "));
  } 

  if (adjacent_hazards & PIT) {
    recognizer.say(F("I feel a draft. -- "));
  } 


  String connectedstring="";
  for (int i=0; i<3; i++) {
    connectedstring=connectedstring+" -- "+String(cave[player_room][i]+1);
    //print_cave_number(cave[player_room][i]);
  }

  recognizer.say("Connected caves are "+connectedstring);

  state = begin_input_move;
}

void gamestatus() 
{
  int adjacent_hazards = EMPTY;

  recognizer.say("Cave");
  print_cave_number(player_room);

  for (int i=0; i<3; i++) {
    adjacent_hazards |= check_for_hazards(cave[player_room][i]);
  }

  if (adjacent_hazards & WUMPUS) {
    recognizer.say(F("It smells like rotten meat --."));
  } 
  if (adjacent_hazards & BAT) {
    recognizer.say(F("I hear bats nearby."));
  } 
  if (adjacent_hazards & PIT) {
    recognizer.say(F("I feel a draft."));
  } 

  recognizer.say(F("Connected caves are"));

  for (int i=0; i<3; i++) {
    print_cave_number(cave[player_room][i]);
  }

  state = begin_input_move;
}


// -------------------------------------------------------------------------------
// Arrow shooting states / functions
// -------------------------------------------------------------------------------

void begin_input_move() 
{
}

void begin_shooting_arrow() 
{
  recognizer.say(F("Shooting."));
  arrow_count--;
  state = animate_shooting_arrow;
}

void animate_shooting_arrow() 
{
  recognizer.say(F("schschschschsch"));
  if (arrow_room == wumpus_room) {
    state = game_over_win;
  } 
  else {
    state = arrow_missed;
  }
}

void arrow_missed() {
  recognizer.say(F("Missed..."));  
  if (arrow_count <= 0) {
    state = game_over_arrow;
  } 
  else {
    state = status_delay;
  }
}


// -------------------------------------------------------------------------------
// Game over states / functions
// -------------------------------------------------------------------------------

void draw_game_over_screen(uint8_t backlight, String message) 
{
  recognizer.say(message);
  if (backlight!=1) recognizer.say(F("Game Over!"));
}

void game_over_arrow() 
{
  draw_game_over_screen(0, "Out of arrows.");
  state = game_over_delay;
}

void game_over_pit() 
{
  recognizer.say("ooooooooooooooooooo!");
  draw_game_over_screen(0, "Fell in a pit.");
  state = game_over_delay;
}

void game_over_wumpus() 
{
  draw_game_over_screen(0, "Oops. Eaten by Wumpus.");
  state = game_over_delay;
}

void game_over_win() 
{
  draw_game_over_screen(1,"The Wumpus is dead! Yay! Congrats. You win!");  
  state = game_over_delay;
}

void game_over_delay() 
{
  delay(10000);
  state = begin_splash_screen;
}

