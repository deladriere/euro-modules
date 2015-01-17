/*      Example playing an enveloped noise source
		using Mozzi sonification library.

		Demonstrates Ead (exponential attack decay).

		Circuit: Audio output on digital pin 9 on a Uno or similar, or
		DAC/A14 on Teensy 3.0/3.1, or
		check the README or http://sensorium.github.com/Mozzi/

		Mozzi help/discussion/announcements:
		https://groups.google.com/forum/#!forum/mozzi-users

		Tim Barrass 2012, CC by-nc-sa

        modified 12-déc.-2014 Jean-Luc Deladriere 
        Knob A0: Not used
        Knob A1: Not used
        CV A2: Not used
        CV 13: Not used
        Gate: Not used
        Audio : Sound of sea shore 


 */

//#include <ADC.h>  // Teensy 3.0/3.1 uncomment this line and install http://github.com/pedvide/ADC
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/brownnoise8192_int8.h> // recorded audio wavetable
#include <Ead.h> // exponential attack decay
#include <EventDelay.h>
#include <mozzi_rand.h>

#define CONTROL_RATE 64 // powers of 2 please

Oscil<BROWNNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(BROWNNOISE8192_DATA);
EventDelay kDelay; // for triggering envelope start
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain;


void setup()
	{
	// use float to set freq because it will be small and fractional
	aNoise.setFreq((float)AUDIO_RATE/BROWNNOISE8192_SAMPLERATE);
	//randSeed(); buggy 
	startMozzi(CONTROL_RATE);

	}


void updateControl()
	{
	// jump around in audio noise table to disrupt obvious looping
	aNoise.setPhase(rand((unsigned int)BROWNNOISE8192_NUM_CELLS));

	if(kDelay.ready())
		{
		// set random parameters // change these set up to vary tempo
		unsigned int delay = rand(2000);
		unsigned int attack = rand(500);
		unsigned int decay = rand(3000);
		kEnvelope.start(500+attack,2000+decay);

		kDelay.start(5000+delay);
		}
	gain = (int) kEnvelope.next();
	}


int updateAudio()
	{
	return (gain*aNoise.next())>>2;
	}


void loop()
	{
	audioHook(); // required here
	}








