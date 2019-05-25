#include <Arduino.h>
#include "MCP23008.h" //from http://gtbtech.com/?p=875
#include "Wire.h"
#include "DS1077.h"
#include <MIDIUSB.h>

#define MCP23008_ADDR 0x40 // TODO: corriger ? en fait c'est 20

/*

   ██   ██  █████  ██████  ██████  ██     ██  █████  ██████  ███████
   ██   ██ ██   ██ ██   ██ ██   ██ ██     ██ ██   ██ ██   ██ ██
   ███████ ███████ ██████  ██   ██ ██  █  ██ ███████ ██████  █████
   ██   ██ ██   ██ ██   ██ ██   ██ ██ ███ ██ ██   ██ ██   ██ ██
   ██   ██ ██   ██ ██   ██ ██████   ███ ███  ██   ██ ██   ██ ███████

 */

#define RS2 8
#define RS1 9
#define RS0 10

#define AR 5
#define RW 6
#define RST 7

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



void Reset()
{
  digitalWrite(RST, LOW);
  delayMicroseconds(1);
  digitalWrite(RST, HIGH);
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
  Command(1, map(analogRead(A1), 0, 255, 0, 255));
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
  analogReadResolution(8);
  DS1077_init(54); // 56 = 1.8 MHz

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(RS0, OUTPUT);
  pinMode(RS1, OUTPUT);
  pinMode(RS2, OUTPUT);

  pinMode(RW, OUTPUT);
  pinMode(AR, INPUT);
  pinMode(RST, OUTPUT);

  digitalWrite(RST, HIGH);
  digitalWrite(RW, HIGH);

  delay(100);
  //Reset();
  SC02.writeIODIR(0x0);

  Command(0, 192); // load DR1 DR2 bit to 1 (to activate A/R request mode) (192)
  Command(3, 128); //Control bit to 1 (128)
  //Command(3, 0);   // Control bit to 0 (to activate A/R request mode by going to 0) (0)
  Command(3, 110); // Set articulation to normal and amplitude to maximum (116)
  Command(4, 240); // Set Filter frequency to normal (231)
  Command(2, 200); // Set Speech rate to normal (168)
  Command(1, 127);
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