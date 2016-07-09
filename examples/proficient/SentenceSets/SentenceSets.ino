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
 * Sometimes, especially with more complex dialogs, it can be desirable  
 * to have MOVI compare the recognized words only against a subset of the sentences
 * rather than against all of them. MOVI does not support this feature natively. 
 * However, Arduino is fast enough so that these function can be implemented 
 * in a sketch.
 *
 * CAUTION: 
 * This example requires advanced programming skills and knowledge in 
 * computer science. It is meant as a guidance for larger projects with MOVI.  
 * Do not start learning about MOVI using this example. 
 *
 * Using the example of a simple phone-number storage device, this sketch shows 
 * two different type of matchings: 
 * - Sentence matching (similar to what's internally implemented in MOVI) 
 * - Word-by-word correction
 * 
 * The functions of this sketch are as follows:
 * - "Store number": asks for a phone number, the individual numbers are then word-by-word corrected eg., "on to" will be corrected to "one two".
 * - "Recall number": Reads the stored number or complains that no number was stored.
 * - "Scratch number": Asks for confirmation via "yes" or "no". "Yes" and "no" are sentence matched against the second set only. If 'yes', the phone number is deleted.
 *
 * Just like the security question, the three basic commands are also manually matched. So an uttered "one two three" will result in "store number". In order to avoid this behavior,
 * a threshold against the Levenshtein matching function can be used to filter sentences that are "too far away". 
 *
 * Circuitry:
 * Arduino UNO R3, MEGA2560 R3, or Arduino Leonardo R3.
 * Connect speaker to MOVI.
 * IMPORTANT: Always use external power supply with MOVI. 
 * 
 * Other Arduino-compatible boards:  
 * Consult MOVI's User Manual before connecting MOVI.
 *
 */

#include "MOVIShield.h"     // Include MOVI library, needs to be *before* the next #include

#ifdef ARDUINO_ARCH_AVR 
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR architecture, other boards need to use Serial1 
#endif

MOVI recognizer(true);      // Get a MOVI object, true enables serial monitor interface, rx and tx can be passed as parameters for alternate communication pins on AVR architecture

// Define sentence sets. Sentences must be defined without any punctuation and in UPPERCASE as MOVI's raw results are in this format. 
const String SentenceSet0[]= // Sentence set 0, basic commands
{ 
  "STORE NUMBER", 
  "RECALL NUMBER", 
  "SCRATCH NUMBER",  
};

const String SentenceSet1[]= // Sentence set 1, confirmation
{ 
  "YES", 
  "NO", 
};

const String SentenceSet2[]= // Sentence set 2, digits
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
  "ZERO",
  "O",
};

const int setsizes[]={3,2,11}; // Cardinality of each sentence set.
const int numsets=3;           // Number of sets

// OPTIONAL but useful: Bind the sets together for easier handling (they appear as 2D array).
const String *SentenceSets[] = {
  SentenceSet0, SentenceSet1, SentenceSet2,
};

int activeset=0;              // Stores the currenlty active set.
String phonenumber="";        // Stores the phone number.

void setup()  
{
   recognizer.init();         // Initialize MOVI (waits for it to boot)

  //*
  // Usual note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  recognizer.callSign("ARDUINO"); // Train callsign Arduino (may take 20 seconds)
  
  for (int i=0;i<numsets;i++) {
    for (int j=0;j<setsizes[i];j++) { // Add all sentences from set 0
      recognizer.addSentence(SentenceSets[i][j]); 
    }
  }
  
  recognizer.train();              // Train (may take 20seconds) 
  //*/

  recognizer.responses(false);     // Since we are not using MOVI's built-in sentence matcher, we don't want to hear from it either...
  //  recognizer.setThreshold(5);		// uncomment and set to a higher value (valid range 2-95) if you have a problems due to a noisy environment.
}

void loop() // run over and over
{
  String result;                      // Stores MOVI's raw result
  boolean processed=false;            // Make sure we react to a result only once
  signed int event=recognizer.poll(); // Get result from MOVI, 0 denotes nothing happened, negative values denote events (see docs) 
  
  if (event==RAW_WORDS) {             // If words are available...       
    result=recognizer.getResult();    // ... read them.
    if (result=="---") processed=true; // Ignore silence
    if (result=="?") processed=true;   // Ignore unknown sentences 
    if (activeset==0 && !processed) {  // basic commands
      int res=matchsentence(activeset,result); // get the match for the basic commands
      if (res==0) {                            // First sentence of set 0 ("store number")
        processed=true;                        // Mark result as processed
        if (phonenumber!="") {                 // If a number exists
          recognizer.say(F("I already have a number.")); // complain
        } else {                                   // otherwise
          recognizer.ask(F("Phone number, please!")); // ask for number
          activeset=2;                             // Activate set 2
        }  
      }   
      if (res==1) {                            // Second sentence of set 0 ("recall number")
        processed=true;                        // Mark sentence as processed
        if (phonenumber=="") {                 // Say the number if one exists
          recognizer.say(F("No number saved"));
        } else {  
          recognizer.say("The number is "+phonenumber);
        }
      }
      if (res==2) {                             // Third sentence of set 0 ("scratch number")
        processed=true;                         // Mark result as processed
        if (phonenumber=="") {                  // If phone number exists... 
          recognizer.say(F("No number saved"));
        } else {              
          activeset=1;                          // Activate set 1 (yes/no)    
          recognizer.ask(F("Are you sure?"));      // and ask the question.
        } 
      }   
    }
    if (activeset==1 && !processed) {           // Mode after "Are you sure?"
      int res=matchsentence(activeset,result);  // get the match for the yes/no sentence set
      processed=true;                           // Mark result as processed
      if (res==0) {                             // First sentence of set 1 ("yes")
          phonenumber="";                       // Delete number 
          activeset=0;                          // back to basic commands
          recognizer.say(F("Number deleted."));    // inform user
      } else {                                  // Second sentence of set 1 ("no")
          activeset=0;                          // back to basic commands
          recognizer.say(F("No harm done."));      // just kidding
      } 
    }
    if (activeset==2 && !processed) {           // Mode after "Phone number, please!"
      processed=true;                           // Mark result as processed
      phonenumber=correctwords(activeset,result); // Match the individual numbers from set 2.
      recognizer.say("Saved: "+phonenumber);    // Say them as a confirmation
      activeset=0;                              // Back to basic commands
    }
  }    
}

/*
 * The following function uses the Levenshtein distance to match a given sentence against a result set.
 * The minimum distance wins. If there's more than one minimum, the first minimum wins. 
 * This is where an ambiguity detector and/or a threshold could be implemented to force increased robustness.
 * The Levenshtein distance is a common metric in the speech recognition community for this kind of task. 
 */
int matchsentence(int setno, String result)
{
  int matchscore=30000; // unachievably high number
  int matchindex=0;
  const String *sentences=SentenceSets[setno]; // chose the sentences from the set
  
  for (int i=0;i<setsizes[setno];i++) {  // Find the minimally distant sentence.
    int score=levenshtein(sentences[i],result);
    if (score<matchscore) {
        matchscore=score;
        matchindex=i;
    }
  }
  return matchindex; 
}

/*
 * This function uses matchsentence() to replace each word in the given raw result string with the closest match in a sentence set, thereby returning a corrected result. 
 */
String correctwords(int setno, String rawresult)
{
    int index=0;
    String oneword;
    String matchedstring="";
    const String *sentences=SentenceSets[setno]; // chose the sentences from the set
    
    do {                                  // this loop  
      oneword=getword(rawresult,index);  // extracts each word
      if (oneword!="") {
          int res=matchsentence(setno,oneword);  /// and then matches it
          matchedstring=matchedstring+sentences[res]+" ";  // the matching result is then added to the string, thereby correcting whatever input was with the closest match
      }
      index++;  
    } while (oneword!="");   // until no words are left in the input
    return matchedstring;    // return the result
}

/*********** HELPER FUNCTIONS ********************/

/*
 * This function returns a single string that is separated by a space at a given index. 
 */
String getword(String str, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = str.length()-1;
  char separator = ' ';
  for(int i=0; i<=maxIndex && found<=index; i++){
    if(str.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? str.substring(strIndex[0], strIndex[1]) : "";
}


/*
 * This function calculates the Levenshtein, or edit-distance, counting the number of insertions, deletions, and substitutions needed to go from string s1 to string s2.
 * More information about the function can be found here: https://en.wikipedia.org/wiki/Levenshtein_distance
 * This particular implementation is optimized to use O(min(m,n)) space instead of O(mn) as memory on the Arduino is scarce. 
 * More information on this function can be found from the original source it was adopted from: https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
 */
 
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(String s1, String s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len=s1.length();
    s2len=s2.length();
    unsigned int column[s1len+1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return(column[s1len]);
}

