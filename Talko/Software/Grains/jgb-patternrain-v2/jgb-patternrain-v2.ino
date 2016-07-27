// Patternrain 2.0, for V2 pcb of Grains
// Selects a 16 step trigger pattern from predefined patterns and plays it. 
// Top knob, select pattern
// Middle knob, select bank for patterns
// Lower knob, stop/reset and then pattern rotate (to be able to make it fit better to other parts of your music)
// Outputs trigger the same length as the high part of clock on
// 
// based on the Auduino code by Peter Knight and Ginkosynthese, revised and finetuned by Janne G:son Berg to convert it to noisering like functionality


// Modified to run on Talko 1.1 & Talko 1.2  by Jean-Luc Deladrière

// TALKO 1.1 control
// Gate   : clock in
// Sound  : select pattern
// Pitch :  select bank for patterns
// Speed : stop/reset and then pattern rotate (to be able to make it fit better to other parts of your music)



// TALKO 1.2 control
// Gate   : clock in
// Pitch  : select pattern
// Speed : select bank for patterns
// Bend : stop/reset and then pattern rotate (to be able to make it fit better to other parts of your music)




#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

uint16_t patternTable[8*16]  = 
{
  // Euclidian
  0x8000, //B1000 0000 << 8 + B0000 0000,
  0x8080, //B1000 0000 << 8 + B1000 0000,
  0x8420, //B1000 0100 << 8 + B0010 0000,
  0x8888, //B1000 1000 << 8 + B1000 1000,
  0x9248, //B1001 0010 << 8 + B0100 1000,
  0x9292, //B1001 0010 << 8 + B1001 0010,
  0x952a, //B1001 0101 << 8 + B0010 1010,
  0xaaaa, //B1010 1010 << 8 + B1010 1010,
  0xab55, //B1010 1011 << 8 + B0101 0101,
  0xb5b5, //B1011 0101 << 8 + B1011 0101,
  0xdb6e, //B1101 1011 << 8 + B0110 1110,
  0xeeee, //B1110 1110 << 8 + B1110 1110,
  0xf7be, //B1111 0111 << 8 + B1011 1110,
  0xfefe, //B1111 1110 << 8 + B1111 1110,
  0xfffe, //B1111 1111 << 8 + B1111 1110,
  0xffff, //B1111 1111 << 8 + B1111 1111

  // Kick
  0x8000,  //x... .... .... ....
  0x8200,  //x... ..x. .... ....
  0x8080,  //x... .... x... ....
  0x8220,  //x... ..x. ..x. ....
  0x8888,  //x... x... x... x...
  0x8822,  //x... x... ..x. ..x.
  0x8241,  //x... ..x. .x.. ...x
  0x8282,  //x... ..x. x... ..x.
  0x8889,  //x... x... x... x..x
  0x9221,  //x..x ..x. ..x. ...x
  0x9641,  //x..x .xx. .x.. ...x
  0x888e,  //x... x... x... xxx.
  0x9292,  //x..x ..x. x..x ..x.
  0xa2a2,  //x.x. ..x. x.x. ..x.
  0x88ff,  //x... x... xxxx xxxx
  0xffff,  //xxxx xxxx xxxx xxxx

  // Snare
  0x0808,  //.... x... .... x...
  0x0812,  //.... x... ...x ..x.
  0x0908,  //.... x..x .... x...
  0x122a,  //...x ..x. ..x. x.x.
  0x888b,  //x... x... x... x.xx
  0x4908,  //.x.. x..x .... x...
  0x490a,  //.x.. x..x .... x.x.
  0x2492,  //..x. .x.. x..x ..x.
  0x0a19,  //.... x.x. ...x x..x
  0x0e0e,  //.... xxx. .... xxx.
  0x4b1b,  //.x.. x.xx ...x x.xx
  0x6e4e,  //.xx. xxx. .x.. xxx.
  0x6e6e,  //.xx. xxx. .xx. xxx.
  0xadbf,  //x.x. xx.x x.xx xxxx
  0xdbbb,  //xx.x x.xx x.xx x.xx
  0xffff,  //xxxx xxxx xxxx xxxx

  // Hihat
  0x0002,  //.... .... .... ..x.
  0x0202,  //.... ..x. .... ..x.
  0x0242,  //.... ..x. .x.. ..x.
  0x4242,  //.x.. ..x. .x.. ..x.
  0x2222,  //..x. ..x. ..x. ..x.
  0x2262,  //..x. ..x. .xx. ..x.
  0x2525,  //..x. .x.x ..x. .x.x
  0x2a6a,  //..x. x.x. .xx. x.x.
  0x3333,  //..xx ..xx ..xx ..xx
  0xf0f0,  //xxxx .... xxxx ....
  0xff01,  //xxxx xxxx .... ...x
  0xe2e2,  //xxx. ..x. xxx. ..x.
  0xe776,  //xxx. .xxx .xxx .xx.
  0xdddd,  //xx.x xx.x xx.x xx.x
  0xfcfc,  //xxxx xx.. xxxx xx..
  0xffff   //xxxx xxxx xxxx xxxx
};

uint16_t getPattern(uint16_t bankNumber, uint16_t patternNumber) {
  return patternTable[(bankNumber << 4) + patternNumber];
}


boolean clockHigh;
boolean previousClockHigh;
boolean nextValue;
uint16_t bankSelect;
uint16_t patternSelect;
uint16_t pattern;
uint8_t patternPosition;

uint16_t rotationRead;
uint8_t rotationValue;


// Map Analogue channels
#define ROTATION_CONTROL        (3)
#define BANK_SELECT_CONTROL     (2)
#define PATTERN_SELECT_CONTROL  (1)

// Changing these will also requires rewriting audioOn()
//    Output is on pin 11
#define PWM_PIN       3
#define PWM_VALUE     OCR2B
#define LED_PIN       13
#define LED_PORT      PORTB
#define LED_BIT       5
#define PWM_INTERRUPT TIMER2_OVF_vect

#define CLOCK_IN 2

void audioOn() {
 //Set up PWM to 31.25kHz, phase accurate

    TCCR2A = _BV(COM2B1) | _BV(WGM20);
  TCCR2B = _BV(CS20);
  TIMSK2 = _BV(TOIE2);
}

void setup() 
{
  // Initialize clock variables
  clockHigh = false;
  previousClockHigh = false;
  rotationValue = 0;
  patternPosition = 15;

  // Initialize the four random banks, ie bank 0-3 are preprogrammed and 4-7 filled in with random
  for (int j = 0; j < 4; j++)
  {
    for (int i = 0; i < 16; i++)
    {
      patternTable[64 + j*16 + i] = random(0,65535);
    }
  }

  //Serial.begin(9600);
  pinMode(PWM_PIN,OUTPUT);
  audioOn();
  pinMode(LED_PIN,OUTPUT);
  pinMode(CLOCK_IN,INPUT);
}

void loop() 
{
  rotationRead = analogRead(ROTATION_CONTROL);
  uint16_t clockIn = digitalRead(CLOCK_IN);
  // Going from low to high, ie incoming clock pulse
  if ((clockIn ==1 ) && !clockHigh)
  {
    clockHigh = true;
  }

  // Going from high to low, end of incoming clock pulse
  if ((clockIn == 0) && clockHigh)
  {
    clockHigh = false;
  }

  // read value for pattern rotation
  rotationValue = rotationRead >> 6;

  // Select pattern based on knob values, eight banks at the moment
  bankSelect = analogRead(BANK_SELECT_CONTROL) >> 7;
  patternSelect = analogRead(PATTERN_SELECT_CONTROL) >> 6;
  pattern = getPattern(bankSelect,patternSelect);
}

SIGNAL(PWM_INTERRUPT)
{
  bool output;
  // Only enter trigger output routine when incoming clock is rising for the first time
  if (!previousClockHigh && clockHigh)
  {
    // Rising edge of clock!
    previousClockHigh = true;

    // Get the next trigger to play, including rotation
    nextValue = bitRead(pattern, ((patternPosition - rotationValue) & B00001111));

    // Change pattern pointer and ensure that it stays within 0-15
    patternPosition--;
    patternPosition &= B00001111;

  }

  if (previousClockHigh && !clockHigh)
  {
    // Falling edge of clock!
    previousClockHigh = false;
  }

  // Lowest bit output as trigger, only when incoming clock is high
  output = clockHigh && nextValue;

  // Indicate using onboard red LED
  LED_PORT = output << LED_BIT;
  
  // Output to PWM (this is faster than using analogWrite)  
  PWM_VALUE = output ? 255 : 0;
}


