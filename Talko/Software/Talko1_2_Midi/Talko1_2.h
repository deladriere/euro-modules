// Talkie library
// Copyright 2011 Peter Knight
// This code is released under GPLv2 license.

#ifndef _Talkie_h_
#define _Talkie_h_

#include <inttypes.h>
#include <avr/io.h>

//SWITCHES
#define PIN_BEND 4
#define PIN_GATE 2 // gate in
#define PIN_SPEECH 5
#define PIN_VCO 8

//POTs
#define POT_SOUND 0
#define POT_PITCH 1
#define POT_SPEED 2
#define POT_BEND 3

//MODE
#define MODE_VCO 1
#define MODE_SPEECH 2
#define MODE_REPEAT 3



class Talkie
{
public:
								void say(const uint8_t* address);
								uint8_t* ptrAddr;
								uint8_t ptrBit;
               int mode=0;
               byte velocityByte;
private:
								// Setup
								uint8_t setup;

								// Bitstream parser
								void setPtr(uint8_t* addr);
								uint8_t rev(uint8_t a);
								uint8_t getBits(uint8_t bits);
};


#endif


