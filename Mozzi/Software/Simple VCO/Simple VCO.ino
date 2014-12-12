/*  Example playing a sinewave at a frequency set with a potentiometer
    using Mozzi sonification library.
  
    Demonstrates the use of Oscil to play a wavetable and how to add a potentiometer reading to the sound synthesis.
  
    Circuit: 
	Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
	Control input (Potentiometer) on analog pin 1
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012, extended by Till Bovermann 2014.
    This example code is in the public domain.
    30-nov.-2014 : Jean-Luc Deladrière Change input pin to pin 2
    	Knob A0: Not used
        Knob A1: Not used
        CV A2: VCO CVin
        CV 13: Not used
        Gate: Not used
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // powers of 2 please

const char INPUT_PIN = 2; // set the input for the CV input
int sensor_value = 100;   // value of the knob


void setup(){
  startMozzi(CONTROL_RATE); // set a control rate of 64 (powers of 2 please)
  aSin.setFreq(sensor_value); // set the frequency
}


void updateControl(){
  // put changing controls in here
  sensor_value = mozziAnalogRead(INPUT_PIN); // value is 0-1023
  aSin.setFreq(sensor_value); // set the frequency straight from the sensor reading
	
}


int updateAudio(){
  return aSin.next()<<6; // return an int signal centred around 0
}


void loop(){
  audioHook(); // required here
}