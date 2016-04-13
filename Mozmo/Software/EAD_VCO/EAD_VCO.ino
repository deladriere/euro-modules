/*
 *
 *       Triggered Bown noise with exponential enveloppe

	     Moddified from Ead_Enveloppe by
		Tim Barrass 2012, CC by-nc-sa

        24-nov.-2014 Jean-Luc Deladriere remplaced event delay by gate triggering & added Attack & Decay control via Control Knob
        Knob A0: Attack duration
        Knob A1: Decay duration
        CV A2: Pitch
        CV A3: Not used
        Gate: Trigger sound

 */

//#include <ADC.h>  // Teensy 3.0/3.1 uncomment this line and install http://github.com/pedvide/ADC

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <Ead.h> // exponential attack decay


#include <mozzi_rand.h>


#define CONTROL_RATE 512 // powers of 2 please
#define GATE 4
#define ATTACK A0
#define DECAY A1

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
//EventDelay kDelay;
Ead kEnvelope(CONTROL_RATE);

int gain;
unsigned int attack;
unsigned int decay ;




void setup()
	{

	startMozzi(CONTROL_RATE);
	pinMode(GATE,INPUT_PULLUP);

	}


void updateControl()
	{
	int gate_status;
	decay = map(mozziAnalogRead(A1),0,1023,1000,20);
	attack = map(mozziAnalogRead(A0),0,1023,1000,20);

	aSin.setFreq(mozziAnalogRead(A2));
	gain = (int) kEnvelope.next();


		{
		if(digitalRead(GATE))
			{

			kEnvelope.start(attack,decay);
			gate_status=LOW;

			}

		}

	}
int updateAudio()
	{

	return (gain*aSin.next())>>2;
	}


void loop()
	{
	audioHook(); // required here
	}








