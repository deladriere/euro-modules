
#include "AquesTalk.h"


/////////////////////////////////
// AquesTalkTerm -  Termnal と AquesTalk Pico LSIの中継スケッチ
//   ターミナルの送信改行コードはCRに設定すること
// by N.Yamazaki AQUEST Corp.  <http://www.a-quest.com>

// I2Cのクロックを変更するときは、Wire\utility\twi.h #define TWI_FREQ 100000L を書き換える







byte c;

void setup()
{
	Wire.begin();  // for Master
	Serial.begin(9600);
 
	pinMode(12, INPUT);   
  SetSpeed(100);
  delay(10);
  SetPitch(64);
  Synthe("konichiwa."); 
 delay(10);
 SetAccent(0255);
}

// do we need delays ??

void loop()
{
delay(10);
Synthe("sayofukete.");
delay(10);
Synthe("hitomoosi."); 
delay(10);
Synthe("waaagaaataaatsuuusooomaaaniii.");

}





