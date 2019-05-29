#include <Arduino.h>
#include "MCP23008.h" //from http://gtbtech.com/?p=875
#include "Wire.h"
#include <SPI.h>
#include "DS1077.h"
#define MCP23008_ADDR 0x40 // TODO: correct address in lib in fact it's 20
   // I2C write address 

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
//#define RST 7


// ltc 

// LTC6903 
const int SEN = 7;  //Serial Enable for LTC6903
const int SCLK = PIN_SPI_SCK;  //Serial Clock for clocking in data
const int SDI = PIN_SPI_MOSI;  //Serial Data Input, D15 first
int Word=0;      //shifting word sent to ltc6903

// TODO: move to real spi https://skyduino.wordpress.com/2013/10/04/arduino-code-de-demo-pour-chipset-ltc6903/


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
  int value=0;        //set D15 test value to zero;

  digitalWrite(SEN,LOW); //set Serial Enable True at ltc6903
  for(int i=0;i!=16;i++) //Got 16 bits to clock in
  {
    value=sdi&32768;    //Strip for testing if D15 
    if (value!=0)      // 1?
    {
      digitalWrite(SDI,HIGH); //Set SDI to TRUE
    }
    else
    {
      digitalWrite(SDI,LOW); //Else set to LOW
    }
    digitalWrite(SCLK,LOW);  //Drop the clock line
    digitalWrite(SCLK,HIGH); //Trigger in the SDI low
    sdi<<=1;            //Shift next bit into D15 position
  }
  digitalWrite(SEN,HIGH);  //deselect serial transfer
  digitalWrite(SDI,HIGH); //put data high as initial state
}

void ltc6903(int oct,unsigned int dac)
{
  Word=oct;                // do OCT first
  Word<<=12;              // move it to D15
  dac<<=2;                //align the DAC value
  Word|=dac;            //OR it in, leave CN1 and CN0 at zero
  Transfer(Word);      //Send word as sdi to ltc6903
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

void setup()
{
  
  Wire.begin();
  
  DS1077_init(56); // 56 = 1.8 MHz // min 53

  // Set LTC6903  
  pinMode(SEN, OUTPUT); //define SEN enable
  pinMode(SCLK, OUTPUT);  //define SCLK clock
  pinMode(SDI, OUTPUT);  //define SDI data
  digitalWrite(SEN,HIGH);  //set safely high
  digitalWrite(SCLK,HIGH);  //set safely high
  digitalWrite(SDI,HIGH);  //set safely high

  

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(RS0, OUTPUT);
  pinMode(RS1, OUTPUT);
  pinMode(RS2, OUTPUT);

  pinMode(RW, OUTPUT);
  pinMode(AR, INPUT);
  //pinMode(RST, OUTPUT);

  //digitalWrite(RST, HIGH);
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
  Command(1, 127); // inflection

 
  Serial.begin(9600);
  Serial.println("starting");

  ltc6903(10,11); //Set pitch to default value
  
  
  
}

void loop()
{
 // delay(1000);
  
  Phoneme(10);
  
  do
  {
      ltc6903(10,map(analogRead(A1),0,1023,0,1023));
  } while (1);
  
  

  
  



  for (int y = 0; y < 11; y++)
  {
   // Command(1, map(analogRead(A1), 0, 255, 0, 255));
   
   //Command(1, map(analogRead(A1), 0, 255, 255, 0));
   //ltc6903(10,map(analogRead(A1),0,1023,0,1023));
    Phoneme(hello[y]);
    //delay(200);
  }

}