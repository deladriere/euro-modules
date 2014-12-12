/*

       Tim Barrass 2012-13, CC by-nc-sa.
       12-déc.-2014 Jean-Luc Deladriere
        Knob A0: Not used
        Knob A1: Not used
        CV A2: VCA in
        CV 13: Not used
        Gate: Not used
        Audio : VCA controlled brown sound
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/brownnoise8192_int8.h> // recorded audio wavetable
#include <mozzi_rand.h>
// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above

Oscil<BROWNNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(BROWNNOISE8192_DATA);

int gain;

void setup()
	{
	startMozzi(); // uses the default control rate of 64, defined in mozzi_config.h
	aNoise.setFreq((float)AUDIO_RATE/BROWNNOISE8192_SAMPLERATE);
	}


void updateControl()
	{
	aNoise.setPhase(rand((unsigned int)BROWNNOISE8192_NUM_CELLS));
	gain = map(mozziAnalogRead(A2),0,1023,0,255);


	}


int updateAudio()
	{

	return (aNoise.next()* gain)>>2; // 8 bits scaled up to 14 bits
	}


void loop()
	{
	audioHook(); // required here
	}
