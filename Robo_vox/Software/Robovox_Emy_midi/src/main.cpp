#include <Arduino.h>
#include "MCP23008.h" //from http://gtbtech.com/?p=875
#include "Wire.h"
#include "SDU.h"

#include <MIDIUSB.h>

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

int Word = 0;                 //shifting word sent to ltc6903

/*
   ██████  ██████       ██ ███████  ██████ ████████ ███████
   ██    ██ ██   ██      ██ ██      ██         ██    ██
   ██    ██ ██████       ██ █████   ██         ██    ███████
   ██    ██ ██   ██ ██   ██ ██      ██         ██         ██
   ██████  ██████   █████  ███████  ██████    ██    ███████
 */

MCP23008 SC02(MCP23008_ADDR);

int i;
byte hello[] = {44, 10, 32, 17, 35, 0, 0x23, 0x1C, 0x20, 0x25};

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

void Phoneme(byte value)
{
  digitalWrite(RS0, 0);
  digitalWrite(RS1, 0);
  digitalWrite(RS2, 0);

  SC02.writeGPIO(value);
  while (digitalRead(AR))
    ;
  Strobe();
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

void noteOn(byte channel, byte pitch, byte velocity)
{
  digitalWrite(LED_BUILTIN, HIGH);
  //  Command(1, map(analogRead(A1), 0, 255, 0, 255));
  Phoneme(pitch);
}

void noteOff(byte channel, byte pitch, byte velocity)
{
  digitalWrite(LED_BUILTIN, LOW);
  Phoneme(0);
}

void controlChange(byte channel, byte control, byte value)
{
  digitalWrite(LED_BUILTIN, LOW);
}

// the setup function runs once when you press reset or power the board
void setup()
{
  Wire.begin();

  // Set LTC6903
  pinMode(SEN, OUTPUT);     //define SEN enable
  pinMode(SCLK, OUTPUT);    //define SCLK clock
  pinMode(SDI, OUTPUT);     //define SDI data
  digitalWrite(SEN, HIGH);  //set safely high
  digitalWrite(SCLK, HIGH); //set safely high
  digitalWrite(SDI, HIGH);  //set safely high

  
  ltc6903(10,11); //Set pitch to default value
  
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(RS0, OUTPUT);
  pinMode(RS1, OUTPUT);
  pinMode(RS2, OUTPUT);

  pinMode(RW, OUTPUT);
  pinMode(AR, INPUT);
  //pinMode(RST, OUTPUT);

  //digitalWrite(RST, HIGH);
  digitalWrite(RW, HIGH);

  delay(1000);
  //Reset();
  SC02.writeIODIR(0x0);

  Command(3, 128); //Control bit to 1 (128)
  Command(0, 192); // load DR1 DR2 bit to 1 (to activate A/R request mode) (192)
  Command(3, 0);   // //Control bit to 0

  Command(3, B01111111); // Set articulation to normal and amplitude to maximum  & CTL to 0
  Command(4, 240);       // Set Filter frequency to normal (231)
  Command(2, 200);       // Set Speech rate to normal (168)
  Command(1, 127);       // inflection

  for (int y = 0; y < 11; y++)
  {
    Phoneme(hello[y]);
  }
}

// the loop function runs over and over again forever
void loop()
{
  midiEventPacket_t rx = MidiUSB.read();

  switch (rx.header)
  {
  case 0:
    break; //No pending events

  case 0x9:
    noteOn(
        rx.byte1 & 0xF, //channel
        rx.byte2,       //pitch
        rx.byte3        //velocity
    );
    break;

  case 0x8:
    noteOff(
        rx.byte1 & 0xF, //channel
        rx.byte2,       //pitch
        rx.byte3        //velocity
    );
    break;

  case 0xB:
    controlChange(
        rx.byte1 & 0xF, //channel
        rx.byte2,       //control
        rx.byte3        //value
    );
    break;

  default:
    break;
  }
}