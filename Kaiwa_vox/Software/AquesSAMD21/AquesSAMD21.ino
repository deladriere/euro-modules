
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
  Write("konichiwa."); 
 delay(1000);
}

void loop()
{

delay(100);
Synthe("jyu' nbi dekitayo.");
delay(100);
Synthe("yamanote."); 


}
// todo debug by reading getone cela revient au même car on attent aussi que la réponse soit libre




