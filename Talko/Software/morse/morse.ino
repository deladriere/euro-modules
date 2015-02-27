// http://forum.arduino.cc/index.php?topic=41922.0

#include "TimerOne.h"

char sineovertones[] = {0,84,106,99,113,127,107,79,71,51,1,-26,7,43,23,-17,-26,-23,-43,-60,-49,-44,-66,-63,0,63,66,44,49,60,43,23,26,17,-23,-43,-8,26,-1,-51,-71,-79,-107,-127,-113,-99,-106,-84};


char sine[] = {0, 22, 44, 64, 82, 98, 111, 120, 126, 128, 126, 120, 111, 98, 82, 64, 44, 22, 0, -22, -44, -64, -82, -98, -111, -120, -126, -128, -126, -120, -111, -98, -82, -64, -44, -22};
//a simple sine wave with 36 samples

long analog0 = 160; //variable to store input from analog input pin 0

byte speakerpin = 3;  //audio playback on pin 3.  This can also be set to pin 11.

volatile byte waveindex = 0; //index variable for position in waveform array Sine[]
volatile byte currentvalue = 0;

void setup() {
 
Serial.begin(57600);
 
 
/************************** PWM audio configuration ****************************/
// Configures PWM on pins 3 and 11 to run at maximum speed, rather than the default
// 500Hz, which is useless for audio output

pinMode(speakerpin,OUTPUT); //Speaker on pin 3

cli(); //disable interrupts while registers are configured

bitSet(TCCR2A, WGM20);
bitSet(TCCR2A, WGM21); //set Timer2 to fast PWM mode (doubles PWM frequency)

bitSet(TCCR2B, CS20);
bitClear(TCCR2B, CS21);
bitClear(TCCR2B, CS22);
/* set prescaler to /1 (no prescaling).  The timer will overflow every 
*  62.5nS * 256ticks = 16uS, giving a frequency of 62,500Hz, I think.   */

sei(); //enable interrupts now that registers have been set
 
 
/************************* Timer 1 interrupt configuration *************************/

Timer1.initialize(0);         // initialize timer1, and set a 1/2 second period
Timer1.attachInterrupt(playtone);  // attaches playtone() as a timer overflow interrupt
}



void playtone() {
/* This function is called by the timer1 interrupt.  Every time it is called it sets
*  speakerpin to the next value in Sine[].  frequency modulation is done by changing
*  the timing between successive calls of this function, e.g. for a 1KHz tone,
*  set the timing so that it runs through Sine[] 1000 times a second. */
if (waveindex > 36) {
 waveindex = 0;
 }

analogWrite(speakerpin, sine[waveindex] + 128);
waveindex++;
}

void loop()
{

delay(100);
Timer1.setPeriod(20);
delay(500);
Timer1.setPeriod(40);
delay(500);
Timer1.setPeriod(50);
}
