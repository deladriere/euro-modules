/* This sketch is enumerated as USB MIDI device. 
 * Following library is required
 * - MIDI Library by Forty Seven Effects
 *   https://github.com/FortySevenEffects/arduino_midi_library
 */

//#define SERIAL_MIDI

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <limits>

#include "MCP23008.h" //from http://gtbtech.com/?p=875
#include "Wire.h"
#include "SDU.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "SDU.h" // to allow updates from SD card

#define MCP23008_ADDR 0x20

/*

   ██   ██  █████  ██████  ██████  ██     ██  █████  ██████  ███████
   ██   ██ ██   ██ ██   ██ ██   ██ ██     ██ ██   ██ ██   ██ ██
   ███████ ███████ ██████  ██   ██ ██  █  ██ ███████ ██████  █████
   ██   ██ ██   ██ ██   ██ ██   ██ ██ ███ ██ ██   ██ ██   ██ ██
   ██   ██ ██   ██ ██   ██ ██████   ███ ███  ██   ██ ██   ██ ███████

 */

//                        +-----------------+
//                      9 |[ ]A/R     OUT[ ]| Audio
//                      4 |[ ]R/W     RS0[ ]| 11
//                      5 |[ ]SEN     RS1[ ]| 31
//                        |[ ]SCK     RS2[ ]| 30
//                        |[ ]NC      SCL[ ]|
//                        |[ ]SDI     SDA[ ]|
//                    3V3 |[ ]3V3      5V[ ]| 5V
//                        |[ ]GND     GND[ ]| GND
//                        +________________/

// Emy
#define BUSY 38
#define SW0 0
#define SW1 1
#define GATE 2
#define PUSH 3
#define ROTA 6 // Rotary pin a
#define ROTB 7 // Rotary pin b
#define GREEN_LED 12
#define RED_LED 13

// SC0-02
#define RS2 30
#define RS1 31
#define RS0 11

#define AR 9
#define RW 4

// LTC6903
#define SEN 5            //Serial Enable for LTC6903
#define SCLK PIN_SPI_SCK //Serial Clock for clocking in data
#define SDI PIN_SPI_MOSI //Serial Data Input, D15 first

#define DEBOUNCE 200
long lastSW1 = 0; // to debounce SW1

int Word = 0;        //shifting word sent to ltc6903
bool dispStatus = 0; // to switch of display

/*
   ██████  ██████       ██ ███████  ██████ ████████ ███████
   ██    ██ ██   ██      ██ ██      ██         ██    ██
   ██    ██ ██████       ██ █████   ██         ██    ███████
   ██    ██ ██   ██ ██   ██ ██      ██         ██         ██
   ██████  ██████   █████  ███████  ██████    ██    ███████
 */

MCP23008 SC02(MCP23008_ADDR);
SSD1306AsciiWire display;

#ifndef SERIAL_MIDI
// USB MIDI object
Adafruit_USBD_MIDI usb_midi;
#endif

// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
#ifdef SERIAL_MIDI
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#else
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);
#endif

/*

   ██    ██  █████  ██████  ██  █████  ██████  ██      ███████ ███████
   ██    ██ ██   ██ ██   ██ ██ ██   ██ ██   ██ ██      ██      ██
   ██    ██ ███████ ██████  ██ ███████ ██████  ██      █████   ███████
    ██  ██  ██   ██ ██   ██ ██ ██   ██ ██   ██ ██      ██           ██
     ████   ██   ██ ██   ██ ██ ██   ██ ██████  ███████ ███████ ███████

 */
#ifdef SERIAL_MIDI
#define VERSION "SERIAL MODE"
#else
#define VERSION "USB MODE"
#endif
#define ON 0
#define OFF 1

//#define DEBUG_TRACE

#ifdef DEBUG_TRACE
#define Sprintln(MSG) Serial.println(MSG)
#define Sprint(MSG) Serial.print(MSG)
#else
#define Sprintln(MSG)
#define Sprint(MSG)
#endif

struct Phonem
{
  uint8_t sc02_id;
  const char *label;
};

// Robovox midi->phonem map.
// Original mapping has been designed for Votrax VS6.
// SC02 phonems have been matched to the VS6 phonem set
// (which resulted in duplicate usage of a few phonems)
static const Phonem phonems[] = {
    {0x16, "U"},
    {0x3D, ":UH"},
    {0x14, "IU"},
    {0x3C, ":U"},
    {0x11, "O"},
    {0x13, "OO"},
    {0x3B, ":OH"},
    {0x10, "AW"},
    {0x3B, ":OH"},
    {0x0E, "AH"},
    {0x08, "A"},
    {0x1C, "ER"},
    {0x3E, "E2"},
    {0x0A, "EH"},
    {0x07, "I"},
    {0x01, "E"},
    {0x06, "IE"},
    {0x04, "YI"},
    {0x39, "NG"},
    {0x37, "M"},
    {0x20, "L"},
    {0x38, "N"},
    {0x1F, "R2"},
    {0x2C, "HF"},
    {0x2D, "HFC"},
    {0x31, "J"},
    {0x32, "SCH"},
    {0x2F, "Z"},
    {0x30, "S"},
    {0x34, "F"},
    {0x33, "V"},
    {0x2C, "HF"},
    {0x24, "B"},
    {0x27, "P"},
    {0x25, "D"},
    {0x28, "T"},
    {0x29, "K"},
    {0x26, "KV"},
    {0x00, "PA0"},
    {0x12, "OU"},
    {0x18, "UH"},
    {0x0C, "AE"},
    {0x3A, ":A"},
    {0x05, "AY"},
    {0x03, "Y"},
    {0x2A, "HV"},
    {0x2B, "HVC"},
    {0x2E, "HN"},
    {0x22, "LF"},
    {0x21, "L1"},
    {0x3F, "LB"},
    {0x36, "TH"},
    {0x35, "THV"},
    {0x1D, "R"},
    {0x1E, "R1"},
    {0x23, "W"},
    {0x26, "KV"},
    {0x2B, "HVC"},
    {0x02, "E1"},
    {0x09, "AI"},
    {0x0B, "EH1"},
    {0x0D, "AE1"},
    {0x0F, "AH1"},
    {0x15, "IU1"},
    {0x17, "U1"},
    {0x19, "UH1"},
    {0x1A, "UH2"},
    {0x1B, "UH3"}};

static const size_t kNumPhonems = sizeof(phonems) / sizeof(Phonem);
static const size_t kPA0PhonemIdx = 38;

int i;
byte hello[] = {44, 10, 32, 17, 35, 0, 0x23, 0x1C, 0x20, 0x25};
byte last_note_on = 0;

struct SC02Config
{
  byte filter_freq = 0;
  byte inflection = 0;
  byte rate = 0;
  float randomization = 0.0f;
} sc02_config;

bool cv_control_enabled = false;
bool trigger_sc02_reset = false;
bool trigger_cv_phoneme = false;
bool trigger_pa0_phoneme = false;

byte DivClock = 0; // To ouptut the MIDI clock

volatile int interruptCount = 1; // The rotary counter
volatile bool rotF = 0;          // to know that the rotary was rotated
                                 // because use in rot
                                 // 1 to force first channel update

byte SetChannel = 1; // to store the MIDI channel : set to 1 to start

/*
███████╗██╗   ██╗ ██████╗███╗   ██╗████████╗██╗ ██████╗ ███╗   ██╗███████╗
██╔════╝██║   ██║██╔════╝████╗  ██║╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
█████╗  ██║   ██║██║     ██╔██╗ ██║   ██║   ██║██║   ██║██╔██╗ ██║███████╗
██╔══╝  ██║   ██║██║     ██║╚██╗██║   ██║   ██║██║   ██║██║╚██╗██║╚════██║
██║     ╚██████╔╝╚██████╗██║ ╚████║   ██║   ██║╚██████╔╝██║ ╚████║███████║
╚═╝      ╚═════╝  ╚═════╝╚═╝  ╚═══╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝
                                                                          
*/

void updateChannel()
{
  if (rotF)
  {
    Wire.setClock(1500000L); // speed the display to the max
    display.setCursor(0, 6);
    display.clearToEOL();
    display.print("Channel ");
    display.println(interruptCount);
    Wire.setClock(500000L);
  }
  rotF = 0;
  SetChannel = interruptCount;
}
void rot()
{

  if (digitalRead(ROTA))
  {
    if (digitalRead(ROTB))
    {
      interruptCount--;
    }
    else
    {
      interruptCount++;
    }
    // set rotflag on
    rotF = 1;
  }
  interruptCount = constrain(interruptCount, 1, 16); // to remove later when using the rotary for something else
}

void toggleCarrier()
{
  if (millis() - lastSW1 > DEBOUNCE)
  {
    // TODO:  needs debouncing
    digitalWrite(MISO, digitalRead(SW1));
    lastSW1 = millis();
  }
}
void toggleDisplay()
{
  // TODO: add very long press to disable display
  // TODO: may need debouncing
  dispStatus = !dispStatus;
  display.ssd1306WriteCmd(SSD1306_CHARGEPUMP);
  display.ssd1306WriteCmd(0X10 + (dispStatus << 3));
}

void StatusLED()
{
  digitalWrite(LED_BUILTIN, !digitalRead(AR));
}

void Transfer(word sdi)
{
  int value = 0; //set D15 test value to zero;

  digitalWrite(SEN, LOW);       //set Serial Enable True at ltc6903
  for (int i = 0; i != 16; i++) //Got 16 bits to clock in
  {
    value = sdi & 32768; //Strip for testing if D15
    if (value != 0)      // 1?
    {
      digitalWrite(SDI, HIGH); //Set SDI to TRUE
    }
    else
    {
      digitalWrite(SDI, LOW); //Else set to LOW
    }
    digitalWrite(SCLK, LOW);  //Drop the clock line
    digitalWrite(SCLK, HIGH); //Trigger in the SDI low
    sdi <<= 1;                //Shift next bit into D15 position
  }
  digitalWrite(SEN, HIGH); //deselect serial transfer
  digitalWrite(SDI, HIGH); //put data high as initial state
}

void ltc6903(int oct, unsigned int dac)
{
  Word = oct;     // do OCT first
  Word <<= 12;    // move it to D15
  dac <<= 2;      //align the DAC value
  Word |= dac;    //OR it in, leave CN1 and CN0 at zero
  Transfer(Word); //Send word as sdi to ltc6903
}
void Strobe()
{
  digitalWrite(RW, LOW);
  delayMicroseconds(1);
  digitalWrite(RW, HIGH);
}

void Command(byte registre, byte value)
{

  digitalWrite(RS0, registre & B00000001);
  digitalWrite(RS1, registre & B00000010);
  digitalWrite(RS2, registre & B00000100);

  SC02.writeGPIO(value);
  delayMicroseconds(1);

  Strobe();
}

void resetSC02Config()
{
  Command(3, B01111111); // Set articulation to normal and amplitude to maximum  & CTL to 0
  Command(4, B11110000); // Set Filter frequency to normal (231)
  Command(2, B11001000); // Set Speech rate to normal (168)
  Command(1, B01111111); // inflection

  sc02_config.filter_freq = B11110000;
  sc02_config.inflection = B01111111;
  sc02_config.rate = B1100;
  ltc6903(10, 516); //Set pitch to middle of pitch wheel
}

void toggleCVControl()
{
  cv_control_enabled = !cv_control_enabled;
  if (!cv_control_enabled)
  {
    trigger_sc02_reset = true;
  }
}

void WritePhonemToSc02(byte value)
{
  // Hack to avoid occasional carrier noise when external carrier is used.
  // Switching to the internal carrier seem to silence the noise floor.
  static bool reactivate_ext_input = false;
  if (reactivate_ext_input && value != 0)
  {
    digitalWrite(MISO, digitalRead(SW1));
    reactivate_ext_input = false;
  }

  Command(0, value + B11000000);

  if (digitalRead(SW1) && value == 0)
  {
    delay(10);
    digitalWrite(MISO, 0);
    reactivate_ext_input = true;
  }

  //while (digitalRead(AR))
  //  ;
}

void controlChange(byte channel, byte number, byte value)
{
  switch (number)

  {
  case 1: // modulation
    Command(4, map(value, 0, 127, 200, 251));
    break;

  case 2:
    /* it should be                                    // breath control used to control Inflection
    Word = value << 5;                            // convert to 12 bits  using temp variable;
    Command(2, 0b11000000 + ((Word >> 11) << 3)); // keeping Rate untouched
    Command(1, (Word & 0b011111111000) >> 3);
    but the next command seems to work ?
  */ 
   Command(1, (value<<1));
    break;

  case 64:
    digitalWrite(MISO, value >> 6);
    break;
  }
}

void Clock()
{

  DivClock++;
  if (DivClock > 5) // divide by 6
  {
    digitalWrite(BUSY, 1);
    DivClock = 0;
  }
  else
  {
    digitalWrite(BUSY, 0);
  }
}

void Start()
{
  DivClock = 0;
  Sprintln("Start");
}

void Stop()
{
  Sprint("Stop");
}

void pitchBend(byte channel, int bend)
{

  Sprintln(bend);

  ltc6903(10, map(bend, 8191, -8192, 1023, 8));
}

void TriggerPhonem(const size_t phonem_idx)
{
  digitalWrite(LED_BUILTIN, ON);
  size_t idx = phonem_idx;
  const float randf = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  if (phonem_idx != kPA0PhonemIdx &&
      randf < sc02_config.randomization)
  {
    idx = rand() % (kNumPhonems - 1);
  }
  const Phonem &phonem = phonems[idx];
  WritePhonemToSc02(phonem.sc02_id); // let's start speech first to avoid delays from Oled

  //  Command(1, map(analogRead(A1), 0, 255, 0, 255));
  Wire.setClock(1500000L); // speed the display to the max
  display.setCursor(30, 2);
  display.clearToEOL();
  display.print(phonem.sc02_id, HEX);
  display.setCursor(60, 2);
  display.clearToEOL();
  display.print(phonem.label);
  Wire.setClock(500000L); //Restore I2C speed to allow speech
                          // if (phonem->sc02_id)    // to filter out the PA0 (id = 0)
                          // {
  Serial.println(phonem.label);
}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  // Log when a note is pressed.
  //Serial.printf("Note on: channel = %d, pitch = %d, velocity - %d", channel, pitch, velocity);
  //Serial.println();

  if (channel == SetChannel)
  {

    last_note_on = pitch;
    pitch = constrain(pitch, 36, 93);

    // digitalWrite(BUSY, ON); // to measure latency from MIDI Note On to speech
    // Apply Velocity.
    Command(3, map(velocity, 0, 127, 0, 15) + B00110000);
    TriggerPhonem(pitch - 36);
    // }
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  // Log when a note is released.
  //Serial.printf("Note off: channel = %d, pitch = %d, velocity - %d", channel, pitch, velocity);
  //Serial.println();

  if (channel == SetChannel)
  {
    if (last_note_on == pitch)
    {
      digitalWrite(LED_BUILTIN, OFF);
      //   digitalWrite(BUSY, OFF);
      TriggerPhonem(kPA0PhonemIdx);
    }
  }
}

void cvTrigger()
{
  if (!digitalRead(GATE))
  {
    trigger_cv_phoneme = true;
    return;
  }
  trigger_cv_phoneme = false;
  trigger_pa0_phoneme = true;
}

void setup()
{

  USBDevice.setManufacturerDescriptor("Polaxis");
  USBDevice.setProductDescriptor("Robovox");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(MISO, OUTPUT);

  pinMode(ROTA, INPUT_PULLUP);
  pinMode(ROTB, INPUT_PULLUP);

  pinMode(GATE, INPUT);

  digitalWrite(MISO, LOW);

  // Initialize MIDI, and listen to all MIDI channels
  // This will also call usb_midi's begin()
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Attach the handleNoteOn function to the MIDI Library. It will
  // be called whenever the Bluefruit receives MIDI Note On messages.
  MIDI.setHandleNoteOn(handleNoteOn);

  // Do the same for MIDI Note Off messages.
  MIDI.setHandleNoteOff(handleNoteOff);

  MIDI.setHandleControlChange(controlChange);

  MIDI.setHandlePitchBend(pitchBend);

  MIDI.setHandleClock(Clock);

  MIDI.setHandleStart(Start);

  MIDI.setHandleStop(Stop);

  Serial.begin(115200);

  // wait until device mounted
  // not needed ?
  // while (!USBDevice.mounted())
  //   delay(1);

  Wire.begin();

  // Set LTC6903
  pinMode(SEN, OUTPUT);     //define SEN enable
  pinMode(SCLK, OUTPUT);    //define SCLK clock
  pinMode(SDI, OUTPUT);     //define SDI data
  digitalWrite(SEN, HIGH);  //set safely high
  digitalWrite(SCLK, HIGH); //set safely high
  digitalWrite(SDI, HIGH);  //set safely high

  display.begin(&Adafruit128x64, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
  display.clear();
  display.set1X();
  display.setFont(fixed_bold10x15);
  display.println("Robovox MIDI");
  display.setRow(4);
  display.println("Ver. 0.05");
  display.setRow(6);
  display.println(VERSION);

  ltc6903(10, 516); //Set pitch to middle of pitch wheel

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(GREEN_LED, OFF);
  digitalWrite(RED_LED, OFF);

  pinMode(RS0, OUTPUT);
  pinMode(RS1, OUTPUT);
  pinMode(RS2, OUTPUT);

  pinMode(RW, OUTPUT);
  pinMode(AR, INPUT);
  digitalWrite(RW, HIGH);

  pinMode(BUSY, OUTPUT);
  pinMode(PUSH, INPUT);
  digitalWrite(BUSY, OFF);

  delay(1000);
  attachInterrupt(PUSH, toggleCVControl, FALLING);
  attachInterrupt(SW1, toggleCarrier, CHANGE);
  attachInterrupt(ROTA, rot, CHANGE);

  //Reset();
  SC02.writeIODIR(0x0);

  Command(3, 128); //Control bit to 1 (128)
  Command(0, 192); // load DR1 DR2 bit to 1 (to activate A/R request mode) (192)
  Command(3, 0);   // //Control bit to 0

  resetSC02Config();

  // Set initial carrier mode.
  toggleCarrier();

  for (int y = 0; y < 10; y++)
  {
    WritePhonemToSc02(hello[y]);
    delay(100);
  }

  attachInterrupt(GATE, cvTrigger, CHANGE);
}

void updateSC02()
{
  if (trigger_sc02_reset)
  {
    resetSC02Config();
    trigger_sc02_reset = false;
  }

  static bool last_cv_control_enabled = false;
  if (cv_control_enabled != last_cv_control_enabled)
  {
    Wire.setClock(1500000L); // speed the display to the max
    display.setCursor(0, 4);
    display.clearToEOL();
    display.println(cv_control_enabled ? "CV On" : "CV Off");
    Wire.setClock(500000L); //Restore I2C speed to allow speech
    last_cv_control_enabled = cv_control_enabled;
  }

  if (trigger_cv_phoneme)
  {
    const size_t cv_phonem_idx = map(analogRead(A6), 0, 0x03FF, 0, kNumPhonems - 1);
    Command(3, B00111111); // Max. velocity.
    TriggerPhonem(cv_phonem_idx);
    trigger_cv_phoneme = false;
  }

  if (trigger_pa0_phoneme)
  {
    Command(3, B00111111);        // Max. velocity.
    TriggerPhonem(kPA0PhonemIdx); // PA0 phonem
    trigger_pa0_phoneme = false;
  }

  sc02_config.randomization = 1.0f -
                              static_cast<float>(analogRead(A5)) / static_cast<float>(0x03FF);

  if (!cv_control_enabled)
  {
    return;
  }
  // Pitch control via master clock
  ltc6903(10, map(analogRead(A1), 0, 0x03FF, 0x03FF, 0));

  byte inflection = map(analogRead(A2), 0, 0x03FF, 0xFF, 0);
  if (inflection != sc02_config.inflection)
  {
    Command(1, inflection);
    sc02_config.inflection = inflection;
  }
  byte filter_freq = map(analogRead(A3), 0, 0x03FF, 0, 0xFF);
  if (filter_freq != sc02_config.filter_freq)
  {
    Command(4, filter_freq);
    sc02_config.filter_freq = filter_freq;
  }
  byte rate = map(analogRead(A4), 0, 0x03FF, 0, 0x0F);
  if (rate != sc02_config.rate)
  {
    Command(2, (rate << 4) + B1000);
    sc02_config.rate = rate;
  }
}

void loop()
{
  // read any new MIDI messages
  MIDI.read();
  //digitalWrite(MISO, digitalRead(SW1));
  updateSC02();
  updateChannel();
}
