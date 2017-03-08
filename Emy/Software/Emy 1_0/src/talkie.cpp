// Talkie library
// Copyright 2011 Peter Knight
// This code is released under GPLv2 license.

// Though the Wave Shield DAC pins are configurable, much else in this code
// is still very Uno-specific; the timers and timer control registers, the
// PWM output pin, etc.  Compatibility with other boards is not guaranteed.

#include "talkie.h"

#if defined(__SAMD21G18A__) || defined(__SAMD21E18A__) || defined(__SAMD21J18A__)
 #define __SAMD__
 #define TIMER       TC4
 #define IRQN        TC4_IRQn
 #define IRQ_HANDLER TC4_Handler
 #define PORTTYPE    uint32_t
#else
 #define PORTTYPE    uint8_t
//#define PIEZO      // If set, connect piezo on pins 3 & 11, is louder
#endif

#define FS    8000      // Speech engine sample rate
#define TICKS (FS / 40) // Speech data rate

// Some of these variables could go in the Talkie object, but the hardware
// specificity (reliance on certain timers and/or PWM pins) kills any point
// in multiple instances; there can be only one.  So they're declared as
// static here to keep the header simple and self-documenting.
#if TICKS < 255
static volatile uint8_t   interruptCount;
#else
static volatile uint16_t  interruptCount;
#endif
static volatile PORTTYPE *csPort, *clkPort, *datPort;
static volatile uint16_t  synthEnergy;
static volatile int16_t   synthK1, synthK2;
static volatile int8_t    synthK3, synthK4, synthK5, synthK6,
                          synthK7, synthK8, synthK9, synthK10;
static uint16_t           buf, synthRand = 1;
static int16_t            x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;
static uint8_t            periodCounter, synthPeriod, bufBits;
static PORTTYPE           csBitMask, clkBitMask, datBitMask;
static const uint8_t     *ptrAddr;
#ifdef __SAMD__
static uint16_t           nextPwm = 0x200; // 10-bit PWM
#else
static uint8_t            nextPwm = 0x80;  // 8-bit PWM
#endif

static const int16_t PROGMEM
  tmsK1[]     = {0x82C0,0x8380,0x83C0,0x8440,0x84C0,0x8540,0x8600,0x8780,
                 0x8880,0x8980,0x8AC0,0x8C00,0x8D40,0x8F00,0x90C0,0x92C0,
                 0x9900,0xA140,0xAB80,0xB840,0xC740,0xD8C0,0xEBC0,0x0000,
                 0x1440,0x2740,0x38C0,0x47C0,0x5480,0x5EC0,0x6700,0x6D40},
  tmsK2[]     = {0xAE00,0xB480,0xBB80,0xC340,0xCB80,0xD440,0xDDC0,0xE780,
                 0xF180,0xFBC0,0x0600,0x1040,0x1A40,0x2400,0x2D40,0x3600,
                 0x3E40,0x45C0,0x4CC0,0x5300,0x5880,0x5DC0,0x6240,0x6640,
                 0x69C0,0x6CC0,0x6F80,0x71C0,0x73C0,0x7580,0x7700,0x7E80};

static const int8_t PROGMEM
  tmsK3[]     = {0x92,0x9F,0xAD,0xBA,0xC8,0xD5,0xE3,0xF0,
                 0xFE,0x0B,0x19,0x26,0x34,0x41,0x4F,0x5C},
  tmsK4[]     = {0xAE,0xBC,0xCA,0xD8,0xE6,0xF4,0x01,0x0F,
                 0x1D,0x2B,0x39,0x47,0x55,0x63,0x71,0x7E},
  tmsK5[]     = {0xAE,0xBA,0xC5,0xD1,0xDD,0xE8,0xF4,0xFF,
                 0x0B,0x17,0x22,0x2E,0x39,0x45,0x51,0x5C},
  tmsK6[]     = {0xC0,0xCB,0xD6,0xE1,0xEC,0xF7,0x03,0x0E,
                 0x19,0x24,0x2F,0x3A,0x45,0x50,0x5B,0x66},
  tmsK7[]     = {0xB3,0xBF,0xCB,0xD7,0xE3,0xEF,0xFB,0x07,
                 0x13,0x1F,0x2B,0x37,0x43,0x4F,0x5A,0x66},
  tmsK8[]     = {0xC0,0xD8,0xF0,0x07,0x1F,0x37,0x4F,0x66},
  tmsK9[]     = {0xC0,0xD4,0xE8,0xFC,0x10,0x25,0x39,0x4D},
  tmsK10[]    = {0xCD,0xDF,0xF1,0x04,0x16,0x20,0x3B,0x4D},
  chirp[]     = {0x00,0x2A,0xD4,0x32,0xB2,0x12,0x25,0x14,
                 0x02,0xE1,0xC5,0x02,0x5F,0x5A,0x05,0x0F,
                 0x26,0xFC,0xA5,0xA5,0xD6,0xDD,0xDC,0xFC,
                 0x25,0x2B,0x22,0x21,0x0F,0xFF,0xF8,0xEE,
                 0xED,0xEF,0xF7,0xF6,0xFA,0x00,0x03,0x02,0x01};

static const uint8_t PROGMEM
  tmsEnergy[] = {0x00,0x02,0x03,0x04,0x05,0x07,0x0A,0x0F,
                 0x14,0x20,0x29,0x39,0x51,0x72,0xA1,0xFF},
  tmsPeriod[] = {0x00,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
                 0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,
                 0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,
                 0x27,0x28,0x29,0x2A,0x2B,0x2D,0x2F,0x31,
                 0x33,0x35,0x36,0x39,0x3B,0x3D,0x3F,0x42,
                 0x45,0x47,0x49,0x4D,0x4F,0x51,0x55,0x57,
                 0x5C,0x5F,0x63,0x66,0x6A,0x6E,0x73,0x77,
                 0x7B,0x80,0x85,0x8A,0x8F,0x95,0x9A,0xA0};

// Constructor for PWM mode
Talkie::Talkie(void) {
#ifdef __SAMD__
	analogWriteResolution(10);
#elif defined(__AVR_ATmega32U4__) // Circuit Playground
	pinMode(5, OUTPUT);  // !OC4A
#else // Arduino Uno
	pinMode(3, OUTPUT);  // OC2B
 #ifdef PIEZO
	pinMode(11, OUTPUT); // OC2A
 #endif
#endif
	csBitMask = 0;       // DAC not in use
}

// Constructor for DAC mode
Talkie::Talkie(uint8_t cs, uint8_t clk, uint8_t dat) {
	csPort     = portOutputRegister(digitalPinToPort(cs));
	csBitMask  = digitalPinToBitMask(cs);
	clkPort    = portOutputRegister(digitalPinToPort(clk));
	clkBitMask = digitalPinToBitMask(clk);
	datPort    = portOutputRegister(digitalPinToPort(dat));
	datBitMask = digitalPinToBitMask(dat);
	pinMode(cs , OUTPUT);
	pinMode(clk, OUTPUT);
	pinMode(dat, OUTPUT);
	*csPort   |=  csBitMask;  // Deselect
	*clkPort  &= ~clkBitMask; // Clock low
}

void Talkie::say(const uint8_t *addr, boolean block) {

	// Enable the speech system whenever say() is called.

	if(!csBitMask) {
#if defined(__AVR_ATmega32U4__)
		// Set up Timer4 for fast PWM on !OC4A
		PLLFRQ = (PLLFRQ & 0xCF) | 0x30;   // Route PLL to async clk
		TCCR4A = _BV(COM4A0) | _BV(PWM4A); // Clear on match, PWMA on
		TCCR4B = _BV(PWM4X)  |_BV(CS40);   // PWM invert, 1:1 prescale
		TCCR4D = 0;                        // Fast PWM mode
		TCCR4E = 0;                        // Not enhanced mode
		TC4H   = 0;                        // Not 10-bit mode
		DT4    = 0;                        // No dead time
		OCR4C  = 255;                      // TOP
		OCR4A  = 127;                      // 50% duty to start
#elif defined(__AVR__)
		// Set up Timer2 for 8-bit, 62500 Hz PWM on OC2B
		TCCR2A  = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
		TCCR2B  = _BV(CS20); // No prescale
		TIMSK2  = 0;         // No interrupt
		OCR2B   = 0x80;      // 50% duty cycle
 #ifdef PIEZO
		OCR2A   = 0x80;
		TCCR2A |= _BV(COM2A1) | _BV(COM2A0); // OC2A inverting mode
 #endif // endif PIEZO
#endif // endif AVR
		// SAMD uses onboard DAC; no init needed here
	}

	// Reset synth state and 'ROM' reader
	x0 = x1 = x2 = x3 = x4 = x5 = x6 = x7 = x8 =
	  periodCounter = buf = bufBits = 0;
	ptrAddr        = addr;
	interruptCount = TICKS;

#ifdef __SAMD__

	// Enable GCLK for TC4 and COUNTER (timer counter input clock)
	GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN |
	  GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5));
	while(GCLK->STATUS.bit.SYNCBUSY == 1);

	// Counter must first be disabled to configure it
	TIMER->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	while(TIMER->COUNT16.STATUS.bit.SYNCBUSY);

	TIMER->COUNT16.CTRLA.reg =  // Configure timer counter
	  TC_CTRLA_PRESCALER_DIV1 | // 1:1 Prescale
	  TC_CTRLA_WAVEGEN_MFRQ   | // Match frequency generation mode (MFRQ)
	  TC_CTRLA_MODE_COUNT16;    // 16-bit counter mode
	while(TIMER->COUNT16.STATUS.bit.SYNCBUSY);

	TIMER->COUNT16.CTRLBCLR.reg = TCC_CTRLBCLR_DIR; // Count up
	while(TIMER->COUNT16.STATUS.bit.SYNCBUSY);

	TIMER->COUNT16.CC[0].reg = ((F_CPU + (FS / 2)) / FS) - 1;
	while(TIMER->COUNT16.STATUS.bit.SYNCBUSY);

	TIMER->COUNT16.INTENSET.reg = TC_INTENSET_OVF; // Overflow interrupt

	NVIC_DisableIRQ(IRQN);
	NVIC_ClearPendingIRQ(IRQN);
	NVIC_SetPriority(IRQN, 0); // Top priority
	NVIC_EnableIRQ(IRQN);

	// Enable TCx
	TIMER->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
	while(TIMER->COUNT16.STATUS.bit.SYNCBUSY);

	if(block) while(!(TIMER->COUNT16.STATUS.reg & TC_STATUS_STOP));

#else // AVR

	// Set up Timer1 to trigger periodic synth calc at 'FS' Hz
	TCCR1A = 0;                             // No output
	TCCR1B = _BV(WGM12) | _BV(CS10);        // CTC mode, no prescale
	OCR1A  = ((F_CPU + (FS / 2)) / FS) - 1; // 'FS' Hz (w/rounding)
	TCNT1  = 0;                             // Reset counter
	TIMSK1 = _BV(OCIE1A);                   // Compare match interrupt on

	if(block) while(TIMSK1 & _BV(OCIE1A));

#endif // AVR
}

boolean Talkie::talking(void) {
#ifdef __SAMD__
	return !(TIMER->COUNT16.STATUS.reg & TC_STATUS_STOP);
#else
	return TIMSK1 & _BV(OCIE1A);
#endif
}

static inline uint8_t rev(uint8_t a) { // Reverse bit sequence in 8-bit value
	a = ( a         >> 4) | ( a         << 4); // 76543210 -> 32107654
	a = ((a & 0xCC) >> 2) | ((a & 0x33) << 2); // 32107654 -> 10325476
	a = ((a & 0xAA) >> 1) | ((a & 0x55) << 1); // 10325476 -> 01234567
	return a;
}

static uint8_t getBits(uint8_t bits) {
	uint8_t value;
	if(bits > bufBits) {
		buf     |= rev(pgm_read_byte(ptrAddr)) << (8 - bufBits);
		bufBits += 8;
		ptrAddr++; // Don't post-inc in pgm_read_byte! Is a macro.
	}
	value    = buf >> (16 - bits);
	buf    <<= bits;
	bufBits -= bits;
	return value;
}

static void dacOut(uint8_t value) {
	uint8_t bit;

	*csPort  &= ~csBitMask; // Select DAC

	// Clock out 4 bits DAC config (not in loop because it's constant)
	*datPort &= ~datBitMask; // 0 = Select DAC A, unbuffered
	*clkPort |=  clkBitMask; *clkPort &= ~clkBitMask;
	*clkPort |=  clkBitMask; *clkPort &= ~clkBitMask;
	*datPort |=  datBitMask; // 1X gain, enable = 1
	*clkPort |=  clkBitMask; *clkPort &= ~clkBitMask;
	*clkPort |=  clkBitMask; *clkPort &= ~clkBitMask;

	// Output is expanded from 8 to 12 bits for DAC.  Perhaps the
	// synthesizer math could be fiddled to generate 12-bit values.
	for(bit=0x80; bit; bit>>=1) { // Clock out first 8 bits of data
		if(value & bit) *datPort |=  datBitMask;
		else            *datPort &= ~datBitMask;
		*clkPort |= clkBitMask; *clkPort &= ~clkBitMask;
	}
	for(bit=0x80; bit >= 0x10; bit>>=1) { // Low 4 bits = repeat hi 4
		if(value & bit) *datPort |=  datBitMask;
		else            *datPort &= ~datBitMask;
		*clkPort |= clkBitMask; *clkPort &= ~clkBitMask;
	}
	*csPort  |=  csBitMask; // Unselect DAC
}

#define read8(base, bits)  pgm_read_byte(&base[getBits(bits)]);
#define read16(base, bits) pgm_read_word(&base[getBits(bits)]);

#ifdef __SAMD__
void IRQ_HANDLER() {
	TIMER->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF;
#else
ISR(TIMER1_COMPA_vect) {
#endif
	int16_t u0;

	if(csBitMask) {
		dacOut(nextPwm);
	} else {
#ifdef __SAMD__
		analogWrite(A0, nextPwm);
#elif defined(__AVR_ATmega32U4__) // Circuit Playground
		OCR4A = nextPwm;
#else // Uno
 #ifdef PIEZO
		OCR2A = OCR2B = nextPwm;
 #else
		OCR2B = nextPwm;
 #endif // endif PIEZO
#endif // endif Uno
	}

	if(++interruptCount >= TICKS) {
		// Read speech data, processing the variable size frames
		uint8_t energy;
		if((energy = getBits(4)) == 0) {  // Rest frame
			synthEnergy = 0;
		} else if(energy == 0xF) {        // Stop frame; silence
#ifdef __SAMD__
			// Disable timer/counter
			TIMER->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
			while(TIMER->COUNT16.STATUS.bit.SYNCBUSY);
			nextPwm = 0x200;          // Neutral
#else
			TIMSK1 &= ~_BV(OCIE1A);   // Stop interrupt
			nextPwm = 0x80;           // Neutral
#endif
			if(csBitMask) {
				dacOut(nextPwm);
			} else {
				// Stop PWM/DAC out:
#ifdef __SAMD__
				analogWrite(A0, nextPwm);
#elif defined(__AVR_ATmega32U4__)
				TCCR4A = 0;
#else
				TCCR2A = 0;
#endif
			}
			return;
		} else {
			synthEnergy    = pgm_read_byte(&tmsEnergy[energy]);
			uint8_t repeat = getBits(1);
			synthPeriod    = pgm_read_byte(&tmsPeriod[getBits(6)]);
			if(!repeat) { // A repeat frame uses last coefficients
				synthK1 = read16(tmsK1, 5); // All frames
				synthK2 = read16(tmsK2, 5); // use the first
				synthK3 = read8( tmsK3, 4); // 4 coefficients
				synthK4 = read8( tmsK4, 4);
				if(synthPeriod) {
					synthK5  = read8(tmsK5 , 4); // Voiced
					synthK6  = read8(tmsK6 , 4); // frames
					synthK7  = read8(tmsK7 , 4); // use
					synthK8  = read8(tmsK8 , 3); // six
					synthK9  = read8(tmsK9 , 3); // extra
					synthK10 = read8(tmsK10, 3); // coeffs
				}
			}
		}
		interruptCount = 0;
	}

	if(synthPeriod) { // Voiced source
		if(++periodCounter >= synthPeriod) periodCounter = 0;
		u0 = (periodCounter >= sizeof(chirp)) ? 0 :
		     (pgm_read_byte(&chirp[periodCounter]) *
                     (uint32_t)synthEnergy) >> 8;
	} else {          // Unvoiced source
		synthRand = (synthRand >> 1) ^ ((synthRand & 1) ? 0xB800 : 0);
		u0        = (synthRand & 1) ? synthEnergy : -synthEnergy;
	}
	u0     -=       ((synthK10 *          x9) +
	                 (synthK9  *          x8)) >>  7;
	x9      = x8  + ((synthK9  *          u0 ) >>  7);
	u0     -=       ((synthK8  *          x7 ) >>  7);
	x8      = x7  + ((synthK8  *          u0 ) >>  7);
	u0     -=       ((synthK7  *          x6 ) >>  7);
	x7      = x6  + ((synthK7  *          u0 ) >>  7);
	u0     -=       ((synthK6  *          x5 ) >>  7);
	x6      = x5  + ((synthK6  *          u0 ) >>  7);
	u0     -=       ((synthK5  *          x4 ) >>  7);
	x5      = x4  + ((synthK5  *          u0 ) >>  7);
	u0     -=       ((synthK4  *          x3 ) >>  7);
	x4      = x3  + ((synthK4  *          u0 ) >>  7);
	u0     -=       ((synthK3  *          x2 ) >>  7);
	x3      = x2  + ((synthK3  *          u0 ) >>  7);
	u0     -=       ((synthK2  * (int32_t)x1 ) >> 15);
	x2      = x1  + ((synthK2  * (int32_t)u0 ) >> 15);
	u0     -=       ((synthK1  * (int32_t)x0 ) >> 15);
	x1      = x0  + ((synthK1  * (int32_t)u0 ) >> 15);

	if(     u0 >  511) u0 =  511; // Output clamp
	else if(u0 < -512) u0 = -512;

	x0      =  u0;
#ifdef __SAMD__
	nextPwm = u0 + 0x200; // 10-bit
#else
	nextPwm = (u0 >> 2) + 0x80; // 8-bit
#endif
}
