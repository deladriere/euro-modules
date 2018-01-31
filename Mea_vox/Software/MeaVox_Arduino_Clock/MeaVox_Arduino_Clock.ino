#include <Arduino.h>
#include "Wire.h"
#include "MCP23008.h"
#include "DS1077.h"
#include "horloge.h"
#include <TimeLib.h>
#include <DS1307RTC.h>

//from http://gtbtech.com/?p=875


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

+ A temporary switch between pin 11 and GND to have it to say the time when pressing the button

*/



// attention req au demarrage ?


#define REQN 2 //
#define AO 14 // A0 is digital pin 14 (define cannot use A0 taken by the IDE)
#define WN 17 // A3 is digital pin 17
#define AUDIO  6 // (to define as input to avoid short-cut)

#define MCP23008_ADDR 0x40

MCP23008 MyMCP(MCP23008_ADDR);

int m;
int heures ;
int s ;

/*
   ███████ ███████ ████████ ██    ██ ██████
   ██      ██         ██    ██    ██ ██   ██
   ███████ █████      ██    ██    ██ ██████
        ██ ██         ██    ██    ██ ██
   ███████ ███████    ██     ██████  ██
*/


void setup() {
  Wire.begin();

  DS1077_init(18); //  22= 3.2Mhz 18 = 4Mhz 14 = 5Mhz
  MyMCP.writeIODIR(0x0);
  pinMode(REQN, INPUT);
  pinMode(REQN, INPUT);
  pinMode(AO, OUTPUT);
  pinMode (WN, OUTPUT);
  pinMode (11, INPUT_PULLUP);
  //  pinMode(AUDIO,OUTPUT);
  Serial.begin(9600);
  //  DDRD = B11111111; // direction register for Port D all pin as output
  //  delay(2000);
  INIT();
  FIN();
Say(spATTENTION);
Say(spSVP);


}

/*
   ███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████
   ██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██
   █████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████
   ██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
   ██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████
*/


void Say(const unsigned char  *str)
{

  INIT();

  char c; int l; byte b;

  l = pgm_read_byte(str + 0); // Number of byte to read

  for (int i = 1; i <= l; i++)
  {
    c = pgm_read_byte(str + i);
    MyMCP.writeGPIO(c);
    STROBE();
    TREQ();
    //  delay(10);// speed fun only if mode is 1F // holding note
  }

  FIN();
}



// Stop Synth
void STOP()

{
  digitalWrite(AO, 1);
  MyMCP.writeGPIO(0x1B);// Slow Stop
  STROBE();
  TREQ();
}


// Wait for MEA
void TREQ()

{
  while (digitalRead(REQN));

}



void FIN()
{

  //PORTD=0;
  MyMCP.writeGPIO(0x00);
  STROBE();
  TREQ();

  //PORTD=0;
  MyMCP.writeGPIO(0x00);
  STROBE();
  TREQ();


  //PORTD=0;
  MyMCP.writeGPIO(0x00);
  STROBE();
  TREQ();

  //PORTD=0;
  MyMCP.writeGPIO(0x00);
  STROBE();
  TREQ();
  STOP();
}

void INIT()
{
  STOP();
  TREQ();
  digitalWrite(AO, 0);
  delay(2); // 2needed
  MyMCP.writeGPIO(0x34); // initial Pitch
  delay(2); // delay(2) needed
  TREQ(); // ***
  STROBE();
  TREQ();

}

void STROBE()
{

  digitalWrite(WN, 0);
  digitalWrite(WN, 1);

}



void dire(int valeur)
{
  if (valeur < 21)
  {

    switch (valeur) {
      case 0:
        Say(spZERO);
        break;
      case 1:
        Say(spUNE);
        break;
      case 2:
        Say(spDEUX);
        break;
      case 3:
        Say(spTROIS);
        break;
      case 4:
        Say(spQUATRE);
        break;
      case 5:
        Say(spCINQ);
        break;
      case 6:
        Say(spSIX);
        break;
      case 7:
        Say(spSEPT);
        break;
      case 8:
        Say(spHUIT);
        break;
      case 9:
        Say(spNEUF);
        break;
      case 10:
        Say(spDIX);
        break;
      case 11:
        Say(spONZE); Say(spZ);
        break;
      case 12:
        Say(spDOUZE);
        break;
      case 13:
        Say(spTREIZE);
        break;
      case 14:
        Say(spQUATORZ);
        break;
      case 15:
        Say(spQUINZE);
        break;
      case 16:
        Say(spSEIZE);
        break;
      case 17:
        Say(spDIXSEPT);
        break;
      case 18:
        Say(spDIXHUIT);
        break;
      case 19:
        Say(spDIXNEUF);
        break;
      case 20:
        Say(spVINGT);Say(spT);
        break;

    }


  }
  else if (valeur < 30)
  {
      Say(spVINGT);
    if (valeur == 21)
    {
     Say(spTET); Say(spUNE);
    }

    if (valeur == 22)
    {
     Say(spT); Say(spDEUX);
    }

    if (valeur >22)
    {

      dire(valeur - 20);
    }
  }
  else if (valeur < 40)
  {
   Say(spTRENTE);
    if (valeur == 31)
    {
     Say(spTET); Say(spUNE);
    }
    if (valeur >31)
    {

      dire(valeur - 30);
    }
  }

  else if (valeur < 50)
  {
    Say(spQUARANTE);
    if (valeur == 41)
    {
     Say(spTET); Say(spUNE);
    }
    if (valeur >41)
    {

      dire(valeur - 40);
    }
  }

  else if (valeur < 60)
  {
    Say(spCINQUANTE);
    if (valeur == 51)
    {
      Say(spTET); Say(spUNE);
    }
    if (valeur >51)
    {

      dire(valeur - 50);
    }
  }


}

/*
   ██       ██████   ██████  ██████
   ██      ██    ██ ██    ██ ██   ██
   ██      ██    ██ ██    ██ ██████
   ██      ██    ██ ██    ██ ██
   ███████  ██████   ██████  ██
*/


void loop() {
  tmElements_t tm;


 //delay(1000);
 while(digitalRead(11));
// Say(spATTENTION);
//Say(spSVP);

  RTC.read(tm);

  heures = (tm.Hour);
  Serial.print(heures);
  Serial.print(" Heures ");

  dire(heures);
    if (heures== 2 || heures== 3 || heures== 6|| heures== 10) Say(spZ);
    if (heures== 18) Say(spT);
  Say(spHEURE);



  m = (tm.Minute);
  Serial.print(m);
  Serial.print(" minutes ");
  dire(m);


  Say(spMINUTE);

  s = (tm.Second);
  //DS1077_init(s);
  Serial.print(s);
  Serial.print(" secondes ");
  dire (s);
  Say(spSECONDE);
  Serial.println();

}
