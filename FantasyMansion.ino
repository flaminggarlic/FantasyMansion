#include <avr/pgmspace.h>

byte bootMode = 0; //0=normal 1=tones&sync 4=beat&sync
/*
    TO DO
    toggle AMP multiplier to turn dist on and off
    make it play arpeggios
    make it switch to neart every 8 steps perhaps

*/

# define birthdate 30798 //birthdate

byte mode = 10;
#define numberOfModes 10
#define LED 1    //digital pin 1
#define LDR 1    //analog pin 1
#define LDRpin 2 //digital pin 2 
#define SW1 0
#define SW2 3
#define tonepin 4
#define portBpin 1

byte playMode = 0;
byte octaveselect = 0;
byte oct;
int x = 0; //ldr value 0-600
byte diff = 0;
byte xMode = 0;
int oldX = 0;
byte selex = 0;
int t = 0;             //drum portB timer
byte s = 0;

byte melodyOffset = 4;
byte melodyOffsetOffset = 4;
byte scalesOffset = 0;

int barTicker = 1;
//int scalesOffset = 10;   was used to select from premade scales, new code will generate scales on the fly ?
int portBlength = 200;
byte selector = random(1, 17);
byte partTicker = 1;
byte songTicker = 1;
byte modulationinterval = 2;
byte octOffset = 2;
byte distAmount = 0;
byte beatSeqSelex = 0;
//byte beatSeqSelexLookahead = 0;

unsigned long dists = 0B00000000001000000100000001000001;

struct bools {
  bool leftSwitch: 1;
  bool rightSwitch: 1;
  bool slowMo: 1;
  bool portBticker: 1;
  bool tickerFlag: 1;
  bool dist: 1;                              //not used (replaced by dists[]
  bool oldLeftSwitch: 1;
  bool oldRightSwitch: 1;
  bool play: 1;
  bool slolo: 1;                             //not used
  bool Blink: 1;
  bool blinkTicker: 1;
  bool doubleButt: 1;
  bool bend: 1;
  bool disablePortB: 1;
  bool firstRun: 1;
  bool myFirstSongMode: 1;
  bool myFirstBeatMode: 1;
  bool plex: 1;
  bool writeNote: 1;
  bool eraseNote: 1;
  bool noteWritten: 1;                 //not used
  bool preserveMelody: 1;              
  bool preserveBeat: 1;               //not used (preservemelody does this too
  bool allowNoteAddition: 1;
  bool allowTranspose: 1;             //notUsed
  bool BANG_L: 1;
  bool BONG_L: 1;
  bool BANG_R: 1;
  bool BONG_R: 1;
  bool beatWrite: 1;
  bool beatErase: 1;
  bool ownBeat: 1;
  bool BASS: 1;
  bool MELODY: 1;
} bools = {
  .leftSwitch = false,
  .rightSwitch = false,
  .slowMo = false,
  .portBticker = true,
  .tickerFlag = true,
  .dist = false,
  .oldLeftSwitch = false,
  .oldRightSwitch = false,
  .play = false,
  .slolo = false,
  .Blink = false,
  .blinkTicker = false,
  .doubleButt = false,
  .bend = false,
  .disablePortB = false,
  .firstRun = true,
  .myFirstSongMode = false,
  .myFirstBeatMode = false,
  .plex = false,
  .writeNote = false,
  .eraseNote = false,
  .noteWritten = false,
  .preserveMelody = false,
  .preserveBeat = false,
  .allowNoteAddition = false,
  .allowTranspose = true,
  .BANG_L = false,
  .BONG_L = false,
  .BANG_R = false,
  .BONG_R = false,
  .beatWrite = false,
  .beatErase = false,
  .ownBeat = false,
  .BASS = true,
  .MELODY = true,
};


int bender = 0;

byte a = random(1, 20);
byte b = random(1, 20);
byte c = random(5, 31);

unsigned int BDseq =   0b1000000010000000;
unsigned int SDseq =   0b0101100000001000;
unsigned int HHseq =   0b0000000000000000;
unsigned int modsSeq = 0b0001000010000000;
//byte portBlengths[8] = {50000,1000,500,600,2500,300,500,666};

//byte octArray[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
byte octArray = 0B0000000000000000;


byte arpLength = 9;
#define initBarLength 16

byte barLength = initBarLength;

// here are the notes pitches bitches


const uint16_t PROGMEM Scale[32] = {
  681,  721,  764,  809,  858,  909,  963, 1020, 1080, 1145, 1213, 1286,
  1362, 1429, 1529, 1620, 1716, 1818, 1926, 2041, 2162, 2291, 2427, 2571,
  2724, 2886, 3058, 3240, 3432, 3636, 3850, 4080
};

byte bassTempo = 4;
byte scaleSelect = 0;
byte currentScale[30] {
  33, 31, 28, 24, 19, 16, 12, 7, 4, 0, // CMinor    MAKE THESE STRICTER TO FIt THE CHORDS??
  0, 1, 5, 8, 12, 13, 17, 20, 24, 29, // CMAJOR
  0, 2, 4, 7, 9, 12, 14, 16, 19, 22 //penta
};

byte chordIntervalSelector = 0; //0 Minor, 1 Major, 3 Power
const byte PROGMEM chordIntevals[9] {
  0, 3,  7,      //MINOR
  0, 4,  7,      //MAJOR
  0, 5, 12       //POWER
};

#define Channels 4
byte baseTempo = 3;
byte Tempo = 4;      // 4 = 4 beats per second
byte Decay = 6;    // Length of note decay; max 10

volatile unsigned int Acc[Channels];
volatile unsigned int Freq[Channels];
unsigned int Amp[Channels];

//unsigned long Chords[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //This is the chords we will be playing and modifying
unsigned long Chords[16] = {
  0B00000000000010000000000000000000,
  0B00000000000000000000000000000000,
  0B00000000000010000000000000000000,
  0B00000000000000000001000000000000,
  0B00000000000010000000000000000000,
  0B00000000000000000000000000000000,
  0B00000000000010000000000000000000,
  0B00000000000000000000000000000000,
  0B00000000000010000000000000000000,
  0B00000000000000000001000000000000,
  0B00000000000010000000000000000000,
  0B00000000000000000000000000000000,
  0B00000000000010000000000000000000,
  0B00000000000010000000000000000000,
  0B00000000000010000000000000000000,
  0B00000000000010000000000000000000
};

int doublers = 0B0001000100000000;                                                                            //NOT YET IMPLEMENTED

unsigned int ChordsB[16] = {
  0B0000000000000000,
  0B0000000000001000,
  0B0000000000000000,
  0B1000000000000000,
  0B0000000000000000,
  0B0000000000001000,
  0B0000000000000000,
  0B0000000000000000,
  0B0000000000001000,
  0B0000000000000000,
  0B0000000000000000,
  0B0000000000000000,
  0B0000000000001000,
  0B0000000000000000,
  0B0000000000000000,
  0B0000000000000000

};

byte decays[16] {
   5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};



const uint8_t PROGMEM soloScales[20] {
  0, 3, 7, 12, 15, 19, 24, 27, 31, 35, //minor
  0, 4, 7, 12, 6,  19, 14, 24, 28, 31 //pentatonic
};

//Globals persist throughout tune
byte TunePtr = 0, Chan = 0;


void setup() {


  pinMode(tonepin, OUTPUT);
  pinMode(portBpin, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(LDRpin, INPUT_PULLUP);
if(!digitalRead(SW1)){
  bootMode = 1;
} else if(!digitalRead(SW2)){
  bootMode = 4;
} else {
  bootMode = 0;
}
  int randSeed = (analogRead(LDR));
  //digitalWrite(LED, HIGH);
  delay(200);
  
  //digitalWrite(LED, LOW);
  //delay(400);
  randSeed = randSeed + (analogRead(LDR)) + birthdate;

  randomSeed(randSeed);
  //randomSeed(100);
  //refreshRandom();
  //gener8BD();

  // Enable 64 MHz PLL and use as source for Timer1
  PLLCSR = 1 << PCKE | 1 << PLLE;                                                       //can remove

  // Set up Timer/Counter1 for PWM output
  TIMSK = 0;                     // Timer interrupts OFF
  TCCR1 = 1 << CS10;             // 1:1 prescale
  GTCCR = 1 << PWM1B | 2 << COM1B0; // PWM B, clear on match

  OCR1B = 128;  //timer counts to 128 then loops back


  // Set up Timer/Counter0 for 20kHz interrupt to output samples.
  TCCR0A = 3 << WGM00;           // Fast PWM                                            //can remove
  TCCR0B = 1 << WGM02 | 2 << CS00; // 1/8 prescale
  OCR0A = 99;                    // Divide by 100
  TIMSK = 1 << OCIE0A;           // Enable compare match, disable overflow

  // Set up Watchdog timer for 4 Hz interrupt for note output.
  WDTCR = 1 << WDIE | Tempo << WDP0; // 4 Hz interrupt

  //generate beats and melodies
  gener8BDbeat();
  gener8SDbeat();
  gener8hats();
  //gener8Melody();
  melodyTEST();
}


void loop() {
//  handleSyncOut();
  pinRead();          //check the states of the pins
  BANGdetectors();
  portB();       // run the portB function
  modeHandle();       //cycle throuth the modes when necessary
  xManip(xMode);   // manipulate x value : 1=insanepitchrange 2=megapitchrange 0 = donothing
  if (!bools.bend) {
    bender = 0;
  }
  modeSelect();

  //ticker code
  /*
    if ( ((barTicker % 2) == 0) && bools.tickerFlag) {    //onetime code here!!!  THESE PREMISES SEEM WEIRD
      bools.tickerFlag = false;

      //if (random(0, 3) == 0) {                                // randomicate it maybe make this happen on every iteration of barticker?                                        // shorten barLength if its half time
      //}

    } else if ((barTicker % 2 ) && !bools.tickerFlag) { //onetime off code here
      bools.tickerFlag = true;
    }
    //gener8Melody();
    //barTicker = 0;
    //if (millis()-timer < 100){
    // solo(x);
    //}
  */
}



ISR(TIMER0_COMPA_vect) {
  generatePolyTones();
}



