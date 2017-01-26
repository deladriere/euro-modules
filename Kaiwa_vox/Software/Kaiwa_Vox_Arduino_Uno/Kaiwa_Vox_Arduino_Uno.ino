s#include "AquesTalk.h"  // http://www.a-quest.com/download/package/Arduino_AquesTalk_Library.zip
#include <Wire.h>  

// Arduino Uno Click Shield

/*

         +-----------------+
         |[ ]NC      OUT[ ]| Audio
      A3 |[ ]RST   !PLAY[ ]| 2
         |[ ]NC       NC[ ]|
         |[ ]NC       NC[ ]|
         |[ ]NC      SCL[ ]| A5
         |[ ]NC      SDA[ ]| A4
         |[ ]3V3      5V[ ]| 5V
         |[ ]GND     GND[ ]| GND
         +________________/
*/

AquesTalk atp;  // I2C address : 0x2E
#define RESET 17 //  A3 is digital pin 17
#define NPLAY 2 // busy playing when LOW



void setup()
{
	pinMode(RESET, OUTPUT);
	digitalWrite(RESET, HIGH);
	pinMode(NPLAY,INPUT_PULLUP);
	
  delay(100);
  atp.Synthe("konnnichiwa.");
 
}

void loop()
{  
   atp.Synthe("arigato.");
   atp.SetAccent(random(200));
   atp.SetPitch(random(200));
   atp.SetSpeed(random(200));
   atp.Synthe("<ALPHA VAL=0 1 2 3 4 5 6 7 8 9>."); 

}
