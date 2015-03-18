/*

       Tim Barrass 2012-13, CC by-nc-sa.
        Jean-Luc Deladriere
        Knob A0: Choose noise color
        Knob A1: Not used
        CV A2: VCA in
        CV 13: Not used
        Gate: Not used
        Audio : VCA controlled noise
*/

#include <MozziGuts.h>
#include <mozzi_rand.h>
#include <Oscil.h> // oscillator template
#include <tables/brownnoise8192_int8.h> // recorded audio wavetable
#include <tables/pinknoise8192_int8.h>
#include <tables/whitenoise8192_int8.h>
// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above

Oscil<BROWNNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(BROWNNOISE8192_DATA);



void choosetable(int waveNumber)
{
	switch(waveNumber)
		{

		case 0:
			aNoise.setTable(BROWNNOISE8192_DATA);
			break;
		case 1:
			aNoise.setTable(PINKNOISE8192_DATA);
			break;
		case 2:
			aNoise.setTable(WHITENOISE8192_DATA);
			break;

		}
}

int gain;

void setup()
{
	startMozzi(); // uses the default control rate of 64, defined in mozzi_config.h
	aNoise.setFreq((float)AUDIO_RATE/BROWNNOISE8192_SAMPLERATE);
}


void updateControl()
{

	int sensor_wave = map(mozziAnalogRead(1),1023,400,2,0); // selection of noise table

	choosetable(sensor_wave);

	aNoise.setPhase(rand((unsigned int)BROWNNOISE8192_NUM_CELLS));
	gain = map(mozziAnalogRead(A2),0,1023,0,255);


}


int updateAudio()
{

	return (aNoise.next()* gain)>>2; // 16 bits scaled up to 14 bits
}


void loop()
{
	audioHook(); // required here
}
