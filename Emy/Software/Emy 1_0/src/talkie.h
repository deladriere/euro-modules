// Talkie library
// Copyright 2011 Peter Knight
// This code is released under GPLv2 license.

#ifndef _Talkie_h_
#define _Talkie_h_

#include <Arduino.h>

class Talkie {
  public:
	Talkie(void);                                 // PWM
	Talkie(uint8_t cs, uint8_t clk, uint8_t dat); // DAC
	void    say(const uint8_t *address, boolean block=true);
	boolean talking(void); // Poll this when block=false
};

#endif
