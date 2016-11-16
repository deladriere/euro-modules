#include "AquesTalk.h"
#include <Wire.h>  

// Arduino Uno Click Shield

/*

         +-----------------+
      A0 |[ ]AO      OUT[ ]| Audio
      A3 |[ ]WN     REQN[ ]| 2
         |[ ]NC       NC[ ]|
         |[ ]NC       NC[ ]|
         |[ ]NC      SCL[ ]| A5
         |[ ]NC      SDA[ ]| A4
     3V3 |[ ]3V3      5V[ ]| 5V
         |[ ]GND     GND[ ]| GND
         +________________/
*/

AquesTalk atp;  // I2C address : 0x2E
#define RESET 17 //  A3 is digital pin 17
#define NPLAY 2 // busy playing when LOW
#define AUDIO 6  // dummy


void setup()
{
	pinMode(RESET, OUTPUT);
	digitalWrite(RESET, HIGH);
	pinMode(NPLAY,INPUT_PULLUP);
	pinMode(AUDIO,INPUT_PULLUP);
	delay(100);
}

void loop()
{
	atp.SetAccent(0x00);
  atp.SetPitch(0x40);
	atp.SetSpeed(100);
	atp.Synthe("konnnichiwa.");  
  
  atp.SetAccent(0x20);
  atp.SetPitch(0x20);
  atp.SetSpeed(50);
  atp.Synthe("<ALPHA VAL=123456789>."); 

}
