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
  atp.SetAccent(random(200));
  atp.SetPitch(random(200));
  atp.SetSpeed(random(200));
  atp.Synthe("<ALPHA VAL=0 1 2 3 4 5 6 7 8 9>."); 

}
