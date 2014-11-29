/*
 *
 *       Triggered Bown noise with exponential enveloppe

	     Moddified from Ead_Enveloppe by
		Tim Barrass 2012, CC by-nc-sa

        24-nov.-2014 Jean-Luc Deladriere remplaced event delay by gate triggering & added Attack & Decay control via Control Knob
        Knob A0: Attack duration
        Knob A1: Decay duration
        CV A2: Not used
        CV 13: Not used
        Gate: Trigger sound

 */

//#include <ADC.h>  // Teensy 3.0/3.1 uncomment this line and install http://github.com/pedvide/ADC

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/brownnoise8192_int8.h> // recorded audio wavetable
#include <Ead.h> // exponential attack decay

#include <mozzi_rand.h>
#include <PinChangeInt.h>

#define CONTROL_RATE 64 // powers of 2 please
#define GATE 4
#define ATTACK A0
#define DECAY A1

Oscil<BROWNNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(BROWNNOISE8192_DATA);
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain;
unsigned int attack;
unsigned int decay ;




void setup()
	{
	// use float to set freq because it will be small and fractional
	startMozzi(CONTROL_RATE);
	aNoise.setFreq((float)AUDIO_RATE/BROWNNOISE8192_SAMPLERATE);
	randSeed(); // fresh random, MUST be called before startMozzi - wierd bug
	
	pinMode(GATE,INPUT_PULLUP);
	PCintPort::attachInterrupt(GATE,Trigger ,FALLING);
	}


void updateControl()
	{
	// jump around in audio noise table to disrupt obvious looping
	aNoise.setPhase(rand((unsigned int)BROWNNOISE8192_NUM_CELLS));
	attack = map(mozziAnalogRead(A0),0,1023,1000,10);
    decay = map(mozziAnalogRead(A1),0,1023,2000,10);

	gain = (int) kEnvelope.next();
	}

void Trigger()
	{

	kEnvelope.start(attack,decay);

	}


int updateAudio()
	{
	return (gain*aNoise.next())>>2;
	}


void loop()
	{
	audioHook(); // required here
	}








