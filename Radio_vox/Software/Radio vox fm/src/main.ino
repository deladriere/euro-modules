#include <Arduino.h>
#include "SDU.h"

///
/// \file SerialRadio.ino
/// \brief Radio implementation using the Serial communication.
///
/// \author Matthias Hertel, http://www.mathertel.de
/// \copyright Copyright (c) 2014 by Matthias Hertel.\n
/// This work is licensed under a BSD style license.\n
/// See http://www.mathertel.de/License.aspx
///
/// \details
/// This is a full function radio implementation that uses a LCD display to show the current station information.\n
/// It can be used with various chips after adjusting the radio object definition.\n
/// Open the Serial console with 57600 baud to see current radio information and change various settings.
///
/// Wiring
/// ------
/// The necessary wiring of the various chips are described in the Testxxx example sketches.
/// The boards have to be connected by using the following connections:
///
/// Arduino port | SI4703 signal | RDA5807M signal
/// :----------: | :-----------: | :-------------:
/// GND (black)  | GND           | GND
/// 3.3V (red)   | VCC           | VCC
/// 5V (red)     | -             | -
/// A5 (yellow)  | SCLK          | SCLK
/// A4 (blue)    | SDIO          | SDIO
/// D2           | RST           | -
///
///
/// More documentation and source code is available at http://www.mathertel.de/Arduino
///
/// History:
/// --------
/// * 05.08.2014 created.
/// * 04.10.2014 working.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OneButton.h"
#include <Adafruit_I2CDevice.h>

#include "avdweb_AnalogReadFast.h"

#include <radio.h>
#include <RDA5807M.h>
#include <SI4703.h>
#include <SI4705.h>
#include <TEA5767.h>

#include <RDSParser.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

/*
 ▄         ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄   ▄         ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ 
▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌          
▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░▌       ▐░▌▐░▌   ▄   ▐░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░▌  ▐░▌  ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀█░█▀▀ ▐░▌       ▐░▌▐░▌ ▐░▌░▌ ▐░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀█░█▀▀ ▐░█▀▀▀▀▀▀▀▀▀ 
▐░▌       ▐░▌▐░▌       ▐░▌▐░▌     ▐░▌  ▐░▌       ▐░▌▐░▌▐░▌ ▐░▌▐░▌▐░▌       ▐░▌▐░▌     ▐░▌  ▐░▌          
▐░▌       ▐░▌▐░▌       ▐░▌▐░▌      ▐░▌ ▐░█▄▄▄▄▄▄▄█░▌▐░▌░▌   ▐░▐░▌▐░▌       ▐░▌▐░▌      ▐░▌ ▐░█▄▄▄▄▄▄▄▄▄ 
▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░░░░░░░░░░▌ ▐░░▌     ▐░░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌
 ▀         ▀  ▀         ▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀   ▀▀       ▀▀  ▀         ▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀ 
*/

#define BUSY 38
#define SW0 0
#define SW1 1
#define GATE 2
#define PUSH 3
#define ROTA 6
#define ROTB 7
#define GREEN_LED 12
#define RED_LED 13

#define ON 0
#define OFF 1

#define UP 2
#define MIDDLE 3
#define BOTTOM 1

// Define some stations available at your locations here:
// 89.40 MHz as 8940

RADIO_FREQ preset[] = {
    8770,
    8810, // hr1
    8820,
    8850, // Bayern2
    8890, // ???
    8930, // * hr3
    9050,
    9080,
    9220, 9350,
    9440, // * hr1
    9510, // - Antenne Frankfurt
    9530,
    9560, // Bayern 1
    9630, 9880,
    10020, // planet
    10090, // ffh
    10110, // SWR3
    10030, 10260, 10380, 10400,
    10500 // * FFH
};

/*
 ▄               ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄   ▄            ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░▌             ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░▌          ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
 ▐░▌           ▐░▌ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░▌          ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ 
  ▐░▌         ▐░▌  ▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌          ▐░▌          ▐░▌          
   ▐░▌       ▐░▌   ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌     ▐░▌     ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░▌          ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄ 
    ▐░▌     ▐░▌    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░▌          ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
     ▐░▌   ▐░▌     ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀█░█▀▀      ▐░▌     ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░▌          ▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀█░▌
      ▐░▌ ▐░▌      ▐░▌       ▐░▌▐░▌     ▐░▌       ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌          ▐░▌                    ▐░▌
       ▐░▐░▌       ▐░▌       ▐░▌▐░▌      ▐░▌  ▄▄▄▄█░█▄▄▄▄ ▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄█░▌
        ▐░▌        ▐░▌       ▐░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░▌ ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
         ▀          ▀         ▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀   ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀ 
                                                                                                                           
*/

char *mainFunctions[] = {
    "Radio",
    "Code",
    "",
    "",
    ""};

int numFunctions = (sizeof(mainFunctions) / sizeof(mainFunctions[0]));
int function;

struct menuCode
{
  char *label;
};

menuCode Code[]{
    ">Version",
    ">Calibrate",
    ">Flash",
};

int i_sidx = 14; ///< Start at Station with index=5
// mode
u_int8_t mode;
u_int8_t modeState;
u_int8_t lastMode;
// serial
static char command;
static int16_t value;
// chip parameter
u_int8_t volume;
u_int8_t lastVolume;
u_int8_t bassBoost;
u_int8_t lastBassBoost;
u_int8_t ten;
u_int8_t lastTen;
u_int8_t unit;
u_int8_t lastUnit;
u_int8_t decimal;
u_int8_t lastDecimal;
u_int8_t band;
u_int8_t lastband;

u_int16_t Rfrequency;
u_int16_t lastRfrequency;

u_int16_t Pfrequency;
u_int16_t lastPfrequency;

int moy;           // for analogfast
int mainState = 0; // for mainState

// rotary
volatile int interruptCount = 0; // The rotary counter
//volatile int endRange;           // because use in rot
volatile bool touched = 0; // for screensaver
volatile bool rotF;

char c;
// station name
char sName[8];
static RADIO_FREQ lastf = 0;
RADIO_FREQ f = 0;
bool gotRDS = 0; // to refresh display

bool pressed;  // detect button or keyboard pressed
bool Lpressed; // detect button long press
unsigned long lastPressed = 0;
unsigned long now = millis();
unsigned long lastchanged;             // to clear message line
static unsigned long nextFreqTime = 0; // to display freq
int seekState = 0;
bool redLedToggle = 0;
bool displayCleared = false;

// screen saver
unsigned long lastTouch;
unsigned long lastBlink;
int ssaverState = 0;

// scroll
char message[64];
int x, minX;

// vumeter
int hMeter = 65; // horizontal center for needle animation
int vMeter = 85; // vertical center for needle animation (outside of dislay limits)
int rMeter = 80; // length of needle animation or arch of needle travel

float MeterValue;

// VU meter background mask image:
static const unsigned char PROGMEM VUMeter[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x09, 0x04, 0x80, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x98, 0x08, 0x06, 0x03, 0x80, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xA4, 0x10, 0x09, 0x00, 0x80, 0x21, 0x20, 0x07, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xA4, 0x10, 0x06, 0x03, 0x00, 0x20, 0xC0, 0x00, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x71, 0x80, 0xA4, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0A, 0x40, 0x98, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3C, 0x00, 0x00,
    0x00, 0x00, 0x3A, 0x40, 0x00, 0x00, 0x02, 0x01, 0x00, 0x40, 0x80, 0x07, 0x00, 0x20, 0x00, 0x00,
    0x00, 0x00, 0x42, 0x40, 0x00, 0x08, 0x02, 0x01, 0x08, 0x40, 0x80, 0x00, 0x00, 0x38, 0x00, 0x00,
    0x00, 0x00, 0x79, 0x80, 0x04, 0x08, 0x02, 0x01, 0x08, 0x81, 0x10, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x08, 0x02, 0x01, 0x08, 0x81, 0x11, 0x04, 0x00, 0x38, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x02, 0x01, 0x08, 0x81, 0x21, 0x04, 0x00, 0x00, 0x08, 0x00,
    0x00, 0x00, 0x00, 0x84, 0x02, 0x04, 0x0F, 0xFF, 0xFF, 0xC3, 0xE2, 0x04, 0x00, 0x00, 0x08, 0x00,
    0x00, 0x00, 0x00, 0xC2, 0x01, 0x07, 0xF0, 0x00, 0x00, 0x3B, 0xFE, 0x08, 0x40, 0x40, 0x08, 0x00,
    0x00, 0xFE, 0x00, 0x62, 0x01, 0xF8, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xE8, 0x40, 0x80, 0x7F, 0x00,
    0x00, 0x00, 0x00, 0x21, 0x1E, 0x00, 0x04, 0x00, 0x80, 0x00, 0x7F, 0xFE, 0x80, 0x80, 0x08, 0x00,
    0x00, 0x00, 0x03, 0x31, 0xE0, 0x00, 0x04, 0x00, 0x80, 0x04, 0x01, 0xFF, 0xC1, 0x00, 0x08, 0x00,
    0x00, 0x00, 0x07, 0x1E, 0x00, 0x40, 0x00, 0x00, 0x00, 0x04, 0x00, 0x1F, 0xFA, 0x00, 0x08, 0x00,
    0x00, 0x00, 0x07, 0xF0, 0x00, 0x40, 0x3B, 0x07, 0x60, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x34, 0x81, 0x90, 0xCC, 0xC0, 0x00, 0x3F, 0xC0, 0x00, 0x00,
    0x00, 0x00, 0x0C, 0x00, 0x03, 0x30, 0x0C, 0x82, 0x90, 0x53, 0x20, 0x00, 0x07, 0xF8, 0x00, 0x00,
    0x00, 0x00, 0x70, 0x40, 0x00, 0xC8, 0x3B, 0x02, 0x60, 0x53, 0x20, 0x00, 0x00, 0xFE, 0x00, 0x00,
    0x00, 0x01, 0x80, 0x20, 0x01, 0xC8, 0x00, 0x00, 0x00, 0x4C, 0xC0, 0x00, 0x00, 0x3F, 0x80, 0x00,
    0x00, 0x06, 0x00, 0x00, 0x03, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00,
    0x00, 0x08, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFC, 0x00,
    0x00, 0x30, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00,
    0x00, 0x00, 0x40, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x00, 0x00, 0xA0, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x02, 0x02, 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03, 0x06, 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x8C, 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0xD8, 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x70, 0x19, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x20, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*
 ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▀▀▀▀▀█░█▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ 
▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌    ▐░▌          ▐░▌               ▐░▌     ▐░▌          
▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄█░▌     ▐░▌    ▐░█▄▄▄▄▄▄▄▄▄ ▐░▌               ▐░▌     ▐░█▄▄▄▄▄▄▄▄▄ 
▐░▌       ▐░▌▐░░░░░░░░░░▌      ▐░▌    ▐░░░░░░░░░░░▌▐░▌               ▐░▌     ▐░░░░░░░░░░░▌
▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀█░▌     ▐░▌    ▐░█▀▀▀▀▀▀▀▀▀ ▐░▌               ▐░▌      ▀▀▀▀▀▀▀▀▀█░▌
▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌    ▐░▌          ▐░▌               ▐░▌               ▐░▌
▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▄▄▄▄▄█░▌    ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄      ▐░▌      ▄▄▄▄▄▄▄▄▄█░▌
▐░░░░░░░░░░░▌▐░░░░░░░░░░▌▐░░░░░░░▌    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░░▌
 ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀      ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀       ▀       ▀▀▀▀▀▀▀▀▀▀▀ 
*/

// RADIO radio;       ///< Create an instance of a non functional radio.
RDA5807M radio; ///< Create an instance of a RDA5807 chip radio
// SI4703   radio;    ///< Create an instance of a SI4703 chip radio.
//SI4705   radio;    ///< Create an instance of a SI4705 chip radio.
// TEA5767  radio;    ///< Create an instance of a TEA5767 chip radio.

/// get a RDS parser
RDSParser rds;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4);

/// State definition for this radio implementation.
enum RADIO_STATE
{
  STATE_PARSECOMMAND, ///< waiting for a new command character.

  STATE_PARSEINT, ///< waiting for digits for the parameter.
  STATE_EXEC      ///< executing the command.
};

RADIO_STATE state; ///< The state variable is used for parsing input characters.
OneButton button(PUSH, true);

// - - - - - - - - - - - - - - - - - - - - - - - - - -

/*
 ▄▄▄▄▄▄▄▄▄▄▄  ▄         ▄  ▄▄        ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄        ▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░▌      ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░▌      ▐░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░▌░▌     ▐░▌▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀█░█▀▀▀▀  ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░▌░▌     ▐░▌▐░█▀▀▀▀▀▀▀▀▀ 
▐░▌          ▐░▌       ▐░▌▐░▌▐░▌    ▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌▐░▌    ▐░▌▐░▌          
▐░█▄▄▄▄▄▄▄▄▄ ▐░▌       ▐░▌▐░▌ ▐░▌   ▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌ ▐░▌   ▐░▌▐░█▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░▌  ▐░▌  ▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌  ▐░▌  ▐░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░▌   ▐░▌ ▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌   ▐░▌ ▐░▌ ▀▀▀▀▀▀▀▀▀█░▌
▐░▌          ▐░▌       ▐░▌▐░▌    ▐░▌▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌    ▐░▌▐░▌          ▐░▌
▐░▌          ▐░█▄▄▄▄▄▄▄█░▌▐░▌     ▐░▐░▌▐░█▄▄▄▄▄▄▄▄▄      ▐░▌      ▄▄▄▄█░█▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌▐░▌     ▐░▐░▌ ▄▄▄▄▄▄▄▄▄█░▌
▐░▌          ▐░░░░░░░░░░░▌▐░▌      ▐░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌      ▐░░▌▐░░░░░░░░░░░▌
 ▀            ▀▀▀▀▀▀▀▀▀▀▀  ▀        ▀▀  ▀▀▀▀▀▀▀▀▀▀▀       ▀       ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀        ▀▀  ▀▀▀▀▀▀▀▀▀▀▀ 
*/
void codeFunctions()
{

}
void displayFunctionList(int p)
{

  for (int i = 0; i < 4; i++)
  { // nbr of lines to display

    display.setCursor(15, i * 20); //
    String str2 = mainFunctions[i + p];
    display.println(str2);
    display.display();
  }
}
void longClick()
{
  Lpressed = true;
}
void Click()
{
  pressed = true;
}

void Radio()
{

  getUser();
  getSerial();
  radio.checkRDS();
  seekCheck();
  infoUpdate();
  displayMode();
  Rfrequency = interruptCount;

  if (Rfrequency != lastRfrequency)
  {
    lastRfrequency = Rfrequency;
    radio.setFrequency(Rfrequency);
    Serial.println(Rfrequency);
    if (Rfrequency == 8750 || Rfrequency == 7600) //hack to avoid clearing message line to fast
    {
    }
    else
    {
      clearText();
    }
    clearRDS();
  }
}
void screenSaver()
{
  switch (ssaverState)
  {
  case 0: // idle

    if (touched)
    {
      lastTouch = millis();
      ssaverState = 0;
      touched = 0;
      break;
    }

    if (millis() - lastTouch > 6000) // start screen saver after 10 minutes
    {
      display.ssd1306_command(SSD1306_DISPLAYOFF);
      ssaverState = 2;
    }

    break;
  case 2:
    if (millis() - lastBlink > 3000) // time since last LED OFF
    {
      digitalWrite(GREEN_LED, ON);
      lastBlink = millis(); //memorize LED ON
      ssaverState = 3;
    }
    else if (touched) // exit screen saver
    {
      ssaverState = 0;
      // touched=0;
      // lastRot = millis();
      display.ssd1306_command(SSD1306_DISPLAYON);
      digitalWrite(GREEN_LED, OFF);
    }
    break;
  case 3:
    if (millis() - lastBlink > 30) // LED on duration
    {
      digitalWrite(GREEN_LED, OFF);
      lastBlink = millis(); // memorize LED ON
      ssaverState = 2;
    }

    break;
  }
}
void rot()
{
  if (digitalRead(ROTA))
  {
    if (digitalRead(ROTB))
    {
      interruptCount = interruptCount - 10;
    }
    else
    {
      interruptCount = interruptCount + 10;
    }
    touched = 1;
    rotF = 1;
  }
}
void clearScreen()
{
  display.clearDisplay();
  display.display();
}
void displayMode()
{
  char s[12];
  radio.formatFrequency(s, sizeof(s));

  if (f != lastf)
  {
    digitalWrite(RED_LED, OFF);
    interruptCount = f;
  }

  if (mode != lastMode)
  {
    lastMode = mode;
    touched = 1;
    lastf = 0;
    clearScreen();
    modeState = 0;
  }
  switch (mode)
  {
  case BOTTOM:
    lastTouch = 0; // force screen saver mode
    break;
  case UP:

    if (1) //if gotRDS ?
    {
      display.setCursor(0, 24);
      display.println(sName);
      gotRDS = 0;
    }

    scrollText();

    if (f != lastf)
    {

      display.setCursor(0, 0); //58
      display.println(s);
      display.display();
      lastf = f;
    }

    break;
  case MIDDLE:
    if (!displayCleared)
    {
      break; // avoid display freq until message is gone
    }

    //if (f != lastf)
    //{

    // MeterValue = 2*MeterValue - 34; // shifts needle to zero position
    //MeterValue = -34;
    display.clearDisplay();

    // refresh display for next step
    display.drawBitmap(0, 0, VUMeter, 128, 64, WHITE); // draws background
    display.setTextSize(1);                            // Normal 1:1 pixel scale
    display.setTextColor(WHITE, BLACK);
    display.setCursor(54, 33);
    display.println(F("RSSI"));

    //Serial.print("FREQ:");
    //Serial.println(s);

    int a1 = (hMeter + (sin(MeterValue / 57.296) * rMeter)); // meter needle horizontal coordinate
    int a2 = (vMeter - (cos(MeterValue / 57.296) * rMeter)); // meter needle vertical coordinate
    display.drawLine(a1, a2, hMeter, vMeter, WHITE);         // draws needle

    display.setTextSize(2);
    display.setCursor(0, 48); //48
    display.println(s);
    display.display();
    lastf = f; // error ?
               /*
    char s[12];
    radio.formatFrequency(s, sizeof(s));
    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(WHITE, BLACK);
    display.setTextWrap(false);
    display.setCursor(0, 48); //58
    display.println(s);
    display.display();
    */

    break;
  }
}

void infoUpdate()
{
  RADIO_INFO info;

  // update the display from time to time
  now = millis();
  if (now > nextFreqTime)
  {
    f = radio.getFrequency();
    radio.getRadioInfo(&info);
    MeterValue = (info.rssi * 2.5) - 70;
    nextFreqTime = now + 400;
  } // if
}
void clearText()
{
  display.setCursor(0, 48);
  display.print("             ");
  display.display();
  for (int i = 0; i < 63; i++)
  {
    message[i] = 0;
  }
}
void scrollText()
{
  if (!displayCleared)
  {
    return;
  }
  // display.setTextColor(WHITE, BLACK);
  // display.setTextWrap(false);
  display.setCursor(x, 48);
  display.print(message);
  display.display();
  x = x - 2; // scroll speed, make more positive to slow down the scroll
  if (x < minX)
    x = display.width();
}
int potReadFast(int pot, int readings)
{
  moy = 0;
  for (int i = 0; i < readings; i++)
  {
    moy = moy + analogReadFast(pot);
  }
  return moy / readings;
}

void getUser()
{
  /*
┌───┐
│ 2 │
│ 3 │
│ 1 │
└───┘
*/
  mode = digitalRead(SW0) + digitalRead(SW1) * 2;

  volume = map(potReadFast(A5, 20), 4090, 20, 0, 15);

  bassBoost = map(potReadFast(A4, 2), 0, 4095, 0, 2);
  bassBoost = constrain(bassBoost, 0, 1);

  band = map(potReadFast(A6, 2), 0, 4095, 0, 2);
  band = constrain(band, 0, 1);

  ten = map(potReadFast(A1, 20), 4090, 20, 7, 10);
  unit = map(potReadFast(A2, 20), 4090, 20, 0, 9);
  decimal = map(potReadFast(A3, 20), 4090, 20, 0, 9);

  Pfrequency = ten * 1000 + unit * 100 + decimal * 10;

  if (band == 0)
  {
    Pfrequency = constrain(Pfrequency, 8750, 10800);
  }
  else
  {
    Pfrequency = constrain(Pfrequency, 7600, 10800);
  }
  if (lastPfrequency != Pfrequency)
  {
    Serial.println(Pfrequency);
    lastPfrequency = Pfrequency;
    interruptCount = Pfrequency; // to force display attention
                                 // radio.setFrequency(Rfrequency)
    touched = 1;
  }

  if (band != lastband)
  {

    display.setCursor(0, 48);
    touched = 1;
    if (band == 0)
    {
      display.println("87.5-108MHz");

      radio.setBandFrequency(RADIO_BAND_FM, 8750);
      radio.clearRDS(); // force RDS read

      interruptCount = 8750;
      for (int i = 0; i < 63; i++)
      {
        message[i] = 0;
      }
    }
    else
    {
      display.println("76.0-108MHz");

      radio.setBandFrequency(RADIO_BAND_FMWORLD, 7600);
      radio.clearRDS(); // force RDS read

      interruptCount = 7600;
      for (int i = 0; i < 63; i++)
      {
        message[i] = 0;
      }
    }

    display.display();
    lastband = band;
    lastchanged = millis();
    displayCleared = false;
  }

  if (bassBoost != lastBassBoost)
  {
    touched = 1;

    display.setCursor(0, 48);

    if (bassBoost == 1)
    {
      display.println("Bassbst OFF");
      radio.setBassBoost(0);
    }
    else
    {
      display.println("Bassbst ON ");
      radio.setBassBoost(1);
    }
    display.display();
    lastBassBoost = bassBoost;
    lastchanged = millis();
    displayCleared = false;
  }
  if (volume != lastVolume)
  {
    touched = 1;

    display.setCursor(0, 48);
    display.print("Volume ");
    if (volume < 10)
    {
      display.print("0");
      if (volume == 0)
      {
        radio.setMute(1);
      }
      else
      {
        radio.setMute(0);
      }
    }
    display.print(volume);
    display.println("  ");
    display.display();
    lastVolume = volume;
    lastchanged = millis();
    displayCleared = false;
    radio.setVolume(volume);
  }
  if (millis() - lastchanged > 1000 && !displayCleared) // clear display after 2 seconds
  {
    display.setCursor(0, 48);
    display.print("           ");
    display.display();
    displayCleared = true;
  }
}
void clearRDS()
{
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 24);
  display.println("          ");
  display.display();
  for (int i = 0; i < 8; i++)
  {
    sName[i] = 0;
  }
}

void justPressed()
{
  pressed = true;
}

void initPins()
{
  pinMode(ROTA, INPUT_PULLUP);
  pinMode(ROTB, INPUT_PULLUP);

  pinMode(SW0, INPUT_PULLUP);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(PUSH, INPUT_PULLUP);
  pinMode(GATE, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
}

void seekCheck()

{
  if (pressed)
  {
    now = millis();
    lastTouch = millis();
    pressed = false;
    touched = 1;
    if (now - lastPressed > 200)
    {
      lastPressed = now;
      digitalWrite(RED_LED, ON);
      clearRDS();
      clearText();
      radio.seekUp(true);
    }
  }
}

void getSerial()
{
  if (Serial.available() > 0)
  {
    // read the next char from input.
    c = Serial.peek();

    if ((state == STATE_PARSECOMMAND) && (c < 0x20))
    {
      // ignore unprintable chars
      Serial.read();
    }
    else if (state == STATE_PARSECOMMAND)
    {
      // read a command.
      command = Serial.read();
      state = STATE_PARSEINT;
    }
    else if (state == STATE_PARSEINT)
    {
      if ((c >= '0') && (c <= '9'))
      {
        // build up the value.
        c = Serial.read();
        value = (value * 10) + (c - '0');
      }
      else
      {
        // not a value -> execute
        runSerialCommand(command, value);
        command = ' ';
        state = STATE_PARSECOMMAND;
        value = 0;
      } // if
    }   // if
  }     // if
}

/// Update the Frequency on the LCD display.
void DisplayFrequency(RADIO_FREQ f)
{
  char s[12];
  radio.formatFrequency(s, sizeof(s));
  Serial.print("FREQ:");
  Serial.println(s);
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE, BLACK);
  display.setTextWrap(false);
  display.setCursor(0, 0); //58
  display.println(s);
  display.display(); // Show initial text

} // DisplayFrequency()

/// Update the ServiceName text on the LCD display.
void DisplayServiceName(char *name)
{
  gotRDS = 1;
  Serial.print("RDS:");
  Serial.println(name);
  // display.setTextSize(2); // Draw 2X-scale text
  // display.setTextColor(WHITE, BLACK);
  // display.setCursor(0, 24);
  // display.println(name);
  // display.display();
  strncpy(sName, name, 8);

} // DisplayServiceName()

/// Update the Time
void DisplayTime(uint8_t hour, uint8_t minute)
{
  Serial.print("Time: ");
  if (hour < 10)
    Serial.print('0');
  Serial.print(hour);
  Serial.print(':');
  if (minute < 10)
    Serial.print('0');
  Serial.println(minute);

} // DisplayTime()

/// Update the ServiceName text on the LCD display.
void DisplayText(char *txt)
{
  Serial.print("Text: <");
  Serial.print(txt);
  Serial.println('>');
  //message = txt;
  strncpy(message, txt, 64);

} // DisplayText()

// - - - - - - - - - - - - - - - - - - - - - - - - - -

void RDS_process(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4)
{
  rds.processData(block1, block2, block3, block4);
}

/// Execute a command identified by a character and an optional number.
/// See the "?" command for available commands.
/// \param cmd The command character.
/// \param value An optional parameter for the command.
void runSerialCommand(char cmd, int16_t value)
{
  if (cmd == '?')
  {
    Serial.println();
    Serial.println("? Help");
    Serial.println("+ increase volume");
    Serial.println("- decrease volume");
    Serial.println("> next preset");
    Serial.println("< previous preset");
    Serial.println(". scan up   : scan up to next sender");
    Serial.println(", scan down ; scan down to next sender");
    Serial.println("fnnnnn: direct frequency input");
    Serial.println("i station status");
    Serial.println("s mono/stereo mode");
    Serial.println("b bass boost");
    Serial.println("u mute/unmute");
  }

  // ----- control the volume and audio output -----

  else if (cmd == '+')
  {
    // increase volume
    int v = radio.getVolume();
    if (v < 15)
      radio.setVolume(++v);
  }
  else if (cmd == '-')
  {
    // decrease volume
    int v = radio.getVolume();
    if (v > 0)
      radio.setVolume(--v);
  }

  else if (cmd == 'u')
  {
    // toggle mute mode
    radio.setMute(!radio.getMute());
  }

  // toggle stereo mode
  else if (cmd == 's')
  {
    radio.setMono(!radio.getMono());
  }

  // toggle bass boost
  else if (cmd == 'b')
  {
    radio.setBassBoost(!radio.getBassBoost());
  }

  // ----- control the frequency -----

  else if (cmd == '>')
  {
    // next preset
    if (i_sidx < (sizeof(preset) / sizeof(RADIO_FREQ)) - 1)
    {
      i_sidx++;
      radio.setFrequency(preset[i_sidx]);
    } // if
  }
  else if (cmd == '<')
  {
    // previous preset
    if (i_sidx > 0)
    {
      i_sidx--;
      radio.setFrequency(preset[i_sidx]);
    } // if
  }
  else if (cmd == 'f')
  {
    radio.setFrequency(value);
  }

  else if (cmd == '.')
  {
    radio.seekUp(false);
  }
  else if (cmd == ':')
  {
    radio.seekUp(true);
    clearText();
    clearRDS();
  }
  else if (cmd == ',')
  {
    radio.seekDown(false);
  }
  else if (cmd == ';')
  {
    radio.seekDown(true);
  }

  // not in help:
  else if (cmd == '!')
  {
    if (value == 0)
      radio.term();
    if (value == 1)
      radio.init();
  }
  else if (cmd == 'i')
  {
    char s[12];
    radio.formatFrequency(s, sizeof(s));
    Serial.print("Station:");
    Serial.println(s);
    Serial.print("Radio:");
    radio.debugRadioInfo();
    Serial.print("Audio:");
    radio.debugAudioInfo();

  } // info

  else if (cmd == 'x')
  {
    radio.debugStatus(); // print chip specific data.
  }
  else if (cmd == 'y')
  {
    radio.debugScan();
  }
} // runSerialCommand()

/*
 ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄         ▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀█░█▀▀▀▀ ▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀█░▌
▐░▌          ▐░▌               ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌
▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄      ▐░▌     ▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄█░▌
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░▌       ▐░▌▐░░░░░░░░░░░▌
 ▀▀▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀      ▐░▌     ▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀▀▀ 
          ▐░▌▐░▌               ▐░▌     ▐░▌       ▐░▌▐░▌          
 ▄▄▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄      ▐░▌     ▐░█▄▄▄▄▄▄▄█░▌▐░▌          
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░▌          
 ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀       ▀       ▀▀▀▀▀▀▀▀▀▀▀  ▀           
 */

/// Setup a FM only radio configuration with I/O for commands and debugging on the Serial port.
void setup()
{
  Wire.setClock(1500000);
  analogReadResolution(12);
  initPins();
  // open the Serial port
  Serial.begin(115200);
  Serial.print("Radio...");
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  //delay(500);
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE, BLACK);
  display.setTextWrap(false);
  display.setCursor(0, 0);
  display.println(F("Radio Vox"));
  display.display(); // Show initial text
  // Initialize the Radio
  radio.term();
  delay(1000);
  radio.init();

  // Enable information to the Serial port
  radio.debugEnable();

  radio.setBandFrequency(RADIO_BAND_FM, preset[i_sidx]); // 5. preset.

  // delay(100);

  radio.setMono(true);
  radio.setMute(false);
  radio.setSoftMute(false);

  // radio._wireDebug();
  radio.setVolume(8);

  Serial.write('>');

  state = STATE_PARSECOMMAND;

  // setup the information chain for RDS data.
  radio.attachReceiveRDS(RDS_process);
  rds.attachServicenNameCallback(DisplayServiceName);
  rds.attachTextCallback(DisplayText);
  rds.attachTimeCallback(DisplayTime);

  runSerialCommand('?', 0);
  // attachInterrupt(PUSH, justPressed, FALLING); no more needed
  attachInterrupt(GATE, justPressed, FALLING);
  attachInterrupt(ROTA, rot, CHANGE);

  button.attachClick(Click);
  button.attachLongPressStart(longClick);

  x = display.width();
  minX = -12 * 64; // 12 = 6 pixels/character * text size 2
  mainState = 1;

} // Setup

/// Constantly check for serial input commands and trigger command execution.
/*
 ▄            ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░▌          ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░▌          ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌
▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌
▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄█░▌
▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌
▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀▀▀ 
▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌          
▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░▌          
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌          
 ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀           
*/

void loop()

{
  button.tick();
  screenSaver();
  if (Lpressed && mainState) // check for long press
  {

    radio.term();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(">");
    display.display();

    interruptCount = 0;
    mainState = 0;
    touched = 1; // to stop screen saver
    pressed = false;
    Lpressed = false;
    rotF = 1; // force display
  }
  switch (mainState)
  {
  case 0:
    if (rotF) // rotary is rotated
    {
      interruptCount = constrain(interruptCount, 0, numFunctions - 4);
      function = interruptCount;
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(">");
      display.display();
      displayFunctionList(function);
      rotF = 0;
      // touched = 0; // will be done in screen saver
    }
    if (pressed) // function is selected
    {
      mainState = function + 1;
      display.clearDisplay();
      String str2 = mainFunctions[function];
      display.print(str2);
      display.display();
      pressed = false;
      if (mainState == 1)
      {
        radio.init();
      }
    }
    break;

  case 1:
    Radio();
  default:
    break;
  case 2:
    mainState = 0;
    codeFunctions();
    break;
  }

} // loop

// End.