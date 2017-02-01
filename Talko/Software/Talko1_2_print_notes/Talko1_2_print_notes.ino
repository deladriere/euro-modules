// Inspired by Talkie library
// Copyright 2011 Peter Knight
// This code is released under GPLv2 license.
// Jean-Luc Deladrière
// Talko version 1.2 Hardware

#include "Talko1_2.h"
#include <TimeLib.h>
#include <EnableInterrupt.h>
#include "5220_alphon.h"
#include <BasicTerm.h>


// Define various ADC prescaler to speedup reading the potentiometers
//const unsigned char PS_16 = (1 << ADPS2); // commented because not used
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
//const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);



#define PIN_A 7
#define PIN_B 6

Talkie voice;
BasicTerm term(&Serial);



const uint8_t *alphons[72]  = {sp_alphon1,sp_alphon2,sp_alphon3,sp_alphon4,sp_alphon5,sp_alphon6,sp_alphon7,sp_alphon8,sp_alphon9,sp_alphon10,sp_alphon11,sp_alphon12,sp_alphon13,sp_alphon14,sp_alphon15,sp_alphon16,sp_alphon17,sp_alphon18,sp_alphon19,sp_alphon20,sp_alphon21,sp_alphon22,sp_alphon23,sp_alphon24,sp_alphon25,sp_alphon26,sp_alphon27,sp_alphon28,sp_alphon29,sp_alphon30,sp_alphon31,sp_alphon32,sp_alphon33,sp_alphon34,sp_alphon35,sp_alphon36,sp_alphon37,sp_alphon38,sp_alphon39,sp_alphon40,sp_alphon41,sp_alphon42,sp_alphon43,sp_alphon44,sp_alphon45,sp_alphon46,sp_alphon47,sp_alphon48,sp_alphon49,sp_alphon50,sp_alphon51,sp_alphon52,sp_alphon53,sp_alphon54,sp_alphon55,sp_alphon56,sp_alphon57,sp_alphon58,sp_alphon59,sp_alphon60,sp_alphon61,sp_alphon62,sp_alphon63,sp_alphon64,sp_alphon65,sp_alphon66,sp_alphon67,sp_alphon68,sp_alphon69,sp_alphon70,sp_alphon71,sp_alphon7};
 byte vco_OK[25] = {22,25,26,28,30,31,33,36,37,39,44,46,51,52,53,54,57,58,59,61,63,67,68,69,70};

const char *alloL[]  ={"AE1","AE1N","AH1","AH1N","AW1","AW1N","E1","E1N","EH1","EH1N","ER1N","I1","I1N","OO1","OW1N","U1","U1N","UH1","UH1M","UH1N","Y1","Y1N","ER1","OW1","Y2","AE2","AH2","AI2","AR2","AU2","AW2","E2","EER2","EH2","EHR2","EI2","ER2","I2","OI2","OO2","OOR2","OR2","OW2","U2","UH2","UU2","AE3","AH3","AI3","AR3","AU3","AW3","E3","EELL","EER3","EH3","EHR3","EI3","ER3","I3","ILL","ING2","OI3","OO3","OOR3","OR3","OW3","U3","UH3","ULL","UHL","UU3","L","L-","LL","M","MM","N","NN","NG1","NG2","R","W","WH","Y","B","BB","D","DD","G1","G2","GG","J","JJ","THV","THV-","V","VV","Z","ZZ","ZH","ZH-","K2","KH","KH-","KH1","KH2","P","PH","PH-","T","TH","TH-","CH","F","FF","HI","HO","HUH","S","SS","SH","SH-","THF","THF-","Pause1","Pause2"};

const char *noteL[]={"C(-1)","Cs(-1)","D(-1)","Ds(-1)","E(-1)","F(-1)","Fs(-1)","G(-1)","Gs(-1)","A(-1)","As(-1)","B(-1)","C0","Cs0","D0","Ds0","E0","F0","Fs0","G0","Gs0","A0","As0","B0","C1","Cs1","D1","Ds1","E1","F1","Fs1","G1","Gs1","A1","As1","B1","C2","Cs2","D2","Ds2","E2","F2","Fs2","G2","Gs2","A2","As2","B2","C3","Cs3","D3","Ds3","E3","F3","Fs3","G3","Gs3","A3","As3","B3","C4","Cs4","D4","Ds4","E4","F4","Fs4","G4","Gs4","A4","As4","B4","C5","Cs5","D5","Ds5","E5","F5","Fs5","G5","Gs5","A5","As5","B5","C6","Cs6","D6","Ds6","E6","F6","Fs6","G6","Gs6","A6","As6","B6","C6","Cs7","D7","Ds7","E7","F7","Fs7","G7","Gs7","A7","As7","B7","C8","Cs8","D8","Ds8","E8","F8","Fs8","G8","Gs8","A8","As8","B8","C9","Cs9","D9","Ds9","E9","F9","Fs9","G9"};


//  4511 pins
#define  LedA 9
#define  LedB 10
#define  LedC 11
#define  LedD 12



//boolean speaking = false;
int count=0;
volatile int interruptCount=8000; // The rotary counter

int preset =0;
int transpose=0;
int C3=0;
int C5=0;

unsigned int ADCValue;
double Voltage;
double Vcc;



volatile int c=4000; // The rotary counter

/* Say any number between -999,999 and 999,999 */













void display(byte n)
{
                digitalWrite(LedD, ((n >> 3) & 1) ? HIGH : LOW);
                digitalWrite(LedC, ((n >> 2) & 1) ? HIGH : LOW);
                digitalWrite(LedB, ((n >> 1) & 1) ? HIGH : LOW);
                digitalWrite(LedA, (n & 1) ? HIGH : LOW);
}

void setup()
{

              Serial.begin(115200);
                  

                // http://www.microsmart.co.za/technical/2014/03/01/advanced-arduino-adc/
                // set up the ADC
                ADCSRA &= ~PS_128; // remove bits set by Arduino library

                // you can choose a prescaler from above.
                // PS_16, PS_32, PS_64 or PS_128
                ADCSRA |= PS_32; //  ## was 16 but give problem with getmode unstable 32 seems ok


                pinMode(13,OUTPUT);

                pinMode(LedA, OUTPUT);
                pinMode(LedB, OUTPUT);
                pinMode(LedC, OUTPUT);
                pinMode(LedD, OUTPUT);


                pinMode(PIN_BEND, INPUT_PULLUP);
                pinMode(PIN_SPEECH, INPUT_PULLUP);
                pinMode(PIN_VCO, INPUT_PULLUP);

                pinMode(PIN_GATE, INPUT);

                pinMode(PIN_A, INPUT_PULLUP); // See http://arduino.cc/en/Tutorial/DigitalPins
                pinMode(PIN_B, INPUT_PULLUP);

                enableInterrupt(PIN_A, rot, CHANGE);

                term.init();
                term.cls();
                 term.show_cursor(false);

                  display(3);

                  do
                  {
                   C3=analogRead(POT_SPEED);
                   Serial.print(C3);
                    
                  }while (digitalRead(PIN_GATE)==0);
                  delay(400);
                  display(5);
                  do
                  {
                  C5=analogRead(POT_SPEED);
                  Serial.print(C5);
                    
                  }while (digitalRead(PIN_GATE)==0);
                  delay(400);
                  display(15);
                  

               
 
                

}

// get rotary


void rot()
{

                if(digitalRead(PIN_B))


                                if(digitalRead(PIN_A)) { interruptCount=interruptCount-10; }
                                else
                                {

                                                interruptCount=interruptCount+10;
                                }

                interruptCount =constrain(interruptCount,1000,10000 ); // 2 dents per unit

              

               

}




void loop()
{

term.cls();
term.position(0,0);
for (int i=0;i<80;i++)
{
  Serial.print(i%10);
}
term.position(1,0);
term.print(F("Potsound  Allophone#  Voltage  Note#   Note   SampleR   Period\t"));


                voice.mode =  (PIND & B100000)>>4;
                voice.mode += (PINB & B1);


                if(voice.mode==MODE_SPEECH) // 1 = VCO  ; 2= SPEECH ; 3= REPEAT

                {

                                while(digitalRead(PIN_GATE)==1) ;

                                while(digitalRead(PIN_GATE)==0) ;
                }


                               //Print Pot Sound//
                               preset=map(analogRead(POT_SOUND),0,1013,0,24);  // choose sound
                               term.position(2,0);
                               term.print(preset);
                               
                               //Print Allophone #//
                               term.position(2,10);
                               term.print(vco_OK[preset ]+1);       // to match table starting with 1 not 0                         
                              


                               // Print Allophone Label
                              // term.position(2,22);
                               //term.print(alloL[vco_OK[preset ]]);
                               

                              // Print Allophone Voltage
                              term.position(2,22);
                           
                              ADCValue = analogRead(POT_SPEED);
                              term.print(ADCValue);
                               
                               
                                 
                               //Print Note number
                               voice.note=map(ADCValue,607,1015,48,72);
                               term.position(2,33);
                              
                               term.print(voice.note);



                               //Print Note Label
                               
                               term.position(2,41); 
                              // term.print(voice.note);
                              term.print(noteL[voice.note+transpose]);
                             
                               

                               // voice.clock=interruptCount;
                               
                              

                                
                               // voice.clock=130.60367*pow(1.059485051,voice.note);
                                voice.clock=265.82325*pow(1.003401755,ADCValue);

    
                                  
                                voice.say(alphons[ vco_OK[preset ]]);
                   
                                //term.position(1,12);
                               
                                
                                
                              
               

}

