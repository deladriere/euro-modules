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
        Gate: Trigger sound + sustain
        Audio : Quantized notes

 */

//#include <ADC.h>  // Teensy 3.0/3.1 uncomment this line and install http://github.com/pedvide/ADC

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/square_no_alias_2048_int8.h> // square table for oscillator
#include <tables/sin2048_int8.h> // sine table for oscillator

#include <Ead.h> // exponential attack decay

#include <mozzi_rand.h>
#include <mozzi_midi.h>

#define CONTROL_RATE 1024 // powers of 2 please
#define GATE 4
#define ATTACK A0
#define DECAY A1
#define DEBUG 1


// todo:square



// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSquare(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int variant=0;
int current;
int gain;
int previous=0;
int note;
int i = 0;
int nseq=0;
int gate_status;

unsigned int attack;
unsigned int decay ;
int seqlength = 4;
int sequence[32];
int mute;

Q8n0 octave_start_note = 42;

void setup()
	{

	startMozzi(CONTROL_RATE);
	pinMode(GATE,INPUT_PULLUP);
#if defined(DEBUG)
	Serial.begin(9600);
	delay(1000);
#endif
	newSequence();

	}


byte rndPentatonic()
	{
	byte note = rand((byte)5);
	switch(note)
		{
		case 0:
			note = 0;
			break;
		case 1:
			note = 3;
			break;
		case 2:
			note = 5;
			break;
		case 3:
			note = 7;
			break;
		case 4:
			note = 10;
			break;
		}
	return note;
	}

byte rndCMajor()
	{

	byte note = rand(7);
	switch(note)
		{
		case 0:
			note = 0;
			break;
		case 1:
			note = 2;
			break;
		case 2:
			note = 4;
			break;
		case 3:
			note = 5;
			break;
		case 4:
			note = 7;
			break;
		case 5:
			note = 9;
			break;
		case 6:
			note = 11;
			break;


		}


	return note;
	}



byte rndCMinor()
	{

	byte note = rand(7);
	switch(note)
		{
		case 0:
			note = 0;
			break;
		case 1:
			note = 2;
			break;
		case 2:
			note = 3;
			break;
		case 3:
			note = 5;
			break;
		case 4:
			note = 7;
			break;
		case 5:
			note = 9;
			break;
		case 6:
			note = 10;
			break;


		}


	return note;
	}





byte rndEMinor()
	{

	byte note = rand(7);
	switch(note)
		{
		case 0:
			note = 4;
			break;
		case 1:
			note = 6;
			break;
		case 2:
			note = 7;
			break;
		case 3:
			note = 9;
			break;
		case 4:
			note = 11;
			break;
		case 5:
			note = 13;
			break;
		case 6:
			note = 14;
			break;


		}


	return note;
	}





byte rndEMajor()
	{

	byte note = rand(7);
	switch(note)
		{
		case 0:
			note = 4;
			break;
		case 1:
			note = 6;
			break;
		case 2:
			note = 8;
			break;
		case 3:
			note = 9;
			break;
		case 4:
			note = 11;
			break;
		case 5:
			note = 13;
			break;
		case 6:
			note = 15;
			break;


		}


	return note;
	}


void newSequence()
	{
	int flip;
	int ratio;
	ratio =map(mozziAnalogRead(A1),0,1023,6,1);
	//Serial.print(ratio);
	octave_start_note = (rand((byte)1)*12)+36;
	for(int i=0; i<seqlength; i++)
		{



		flip=rand(ratio);
		//Serial.print(flip);
		if(flip>0)
			{
			sequence[i]=rndEMinor()+octave_start_note;
			//sequence[i]=0;
			}
		else
			{
			sequence[i]=0;
			//sequence[i]=rndEMajor()+octave_start_note;
			}

#if defined(DEBUG)
		//Serial.print(sequence[i]);
		//Serial.print(" ");
		}
	Serial.println("");
#else
		}
#endif

	}



void updateControl()
	{
	//decay = map(mozziAnalogRead(A1),0,1023,1000,20);

	attack = mozziAnalogRead(A2);// normal= A0

	if((abs(attack-nseq))>400)
		{
		newSequence();
		nseq=attack;
		}

	gate_status = digitalRead(GATE);

	attack=30;
	decay=100;

	if(gate_status==1)
		{
		if(previous==0)
			{
			i = (i + 1) % seqlength; // modulo operator rolls over variable


			note=sequence[i];

			previous=1;
			if(note!=0)
				{
				kEnvelope.start(attack,decay);
				}

			}


		}
	else
		{
		previous=0;
		}
	mute=1;
	aSin.setFreq_Q16n16(Q16n16_mtof(Q8n0_to_Q16n16(note))); // accurate frequency
	gain = (int) kEnvelope.next();
	}



int updateAudio()
	{

	return (gain*mute*aSin.next())>>2;
	}


void loop()
	{
	audioHook(); // required here

	}








