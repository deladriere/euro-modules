/*  
 *   LFO based on SineWave_HIFI by Tim Barrass 2012-13, CC by-nc-sa.
    
        Knob A0: Frequency
        Knob A1: Waveform
        CV A2: VCO CVin
        CV 13: Not used
        Gate: Not used
        OUTPUT : LFO out
    
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/square_no_alias512_int8.h>
#include <tables/sin512_int8.h>
#include <tables/triangle512_int8.h>
#include <tables/saw512_int8.h>





// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <TRIANGLE512_NUM_CELLS, AUDIO_RATE> aOscil(TRIANGLE512_DATA);



void setup()
{
	startMozzi(); // uses the default control rate of 64, defined in mozzi_config.h
	
}



void choosetable(int waveNumber)
{
	switch(waveNumber)
		{

		case 0:
			aOscil.setTable(SIN512_DATA);
			break;
		case 1:
			aOscil.setTable(SQUARE_NO_ALIAS512_DATA);
			break;
		case 2:
			aOscil.setTable(TRIANGLE512_DATA);
			break;
		case 3:
			aOscil.setTable(SAW512_DATA);
			break;
		}


}

void updateControl()
{

	float sensor_value = map(mozziAnalogRead(0),1023,0,1,1000); // value is 0-1023
	int sensor_wave = map(mozziAnalogRead(1),1020,0,0,3);
	choosetable(sensor_wave);
	aOscil.setFreq(sensor_value/10); // set the frequency straight from the sensor reading

}

int updateAudio()
{
	
	return aOscil.next()<<6; // 8 bits scaled up to 14 bits for Hi-Fi
}

void loop()
{
	audioHook(); // required here
}
