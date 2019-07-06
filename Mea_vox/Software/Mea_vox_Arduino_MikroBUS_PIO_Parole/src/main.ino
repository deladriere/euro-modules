#include <Arduino.h>
#include "Wire.h"
#include "MCP23008.h" //from http://gtbtech.com/?p=875
#include "DS1077.h"   // http://ferretrobotics.blogspot.be/2013/08/ds1077-programmable-oscillator.html
#include "phon40.h"
#include "nombres.h"

/*

          +-----------------+
      4   |[ ]AO      OUT[ ]| Audio
      3   |[ ]WN     REQN[ ]| 2
          |[ ]NC       NC[ ]|
          |[ ]NC       NC[ ]|
          |[ ]NC      SCL[ ]| A5
          |[ ]NC      SDA[ ]| A4
     3V3  |[ ]3V3      5V[ ]| 5V
          |[ ]GND     GND[ ]| GND
           +________________/
 *
 */

#define REQN 2  //
#define AO 14   // A0 is digital pin 14 on MikroBUS 1(we cannot use the label A0 taken by the IDE)
#define WN 17  // A3 is digital pin 17 on MikroBUS 1

#define MCP23008_ADDR 0x40
#define CONTINU 1
#define ARRET_LENT 0

MCP23008 MyMCP(MCP23008_ADDR);

int FD = 0;
int Pl = 0; // (cannot use PI)
bool whisper = 0;
byte position;
/*
   ███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████
   ██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██
   █████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████
   ██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
   ██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████
 */

void Speak(int index)
{

        int start;
        int end;
        char c;
        digitalWrite(AO, 0);
      

        start = pgm_read_byte(&(phon[index * 2])) * 256 + pgm_read_byte(&(phon[index * 2 + 1]));

        Serial.print(start, HEX);

        end = pgm_read_byte(&(phon[start + 1])); // Adresse sur 2 bytes

      

        switch (index)
        {

        case 41:
                Pl++;
                Pl = constrain(Pl, 0, 31);
                return;
        case 42:
                Pl--;
                Pl = constrain(Pl, 0, 31);

                return;

        case 43:
                Pl = 0;

                return;
        case 44:
                FD++;
                FD = constrain(FD, 0, 3);
                return;

        case 45:
                FD--;
                FD = constrain(FD, 0, 3);
                return;

        case 46:
                whisper = !whisper;
                Pl = whisper * 16;
                return;
        }

        for (int i = start + 4; i < start + end; i++)
        {

                TREQ();
                c = pgm_read_byte(&(phon[i]));
                if ((i + 1) % 4 == 0) // 4th byte
                {

                        // Pitch increment
                        c = c & B11100000; // clear PI = Robot voice
                        c = c + Pl;        // 16 = noise = Whisper mode

                        // Frame duration
                        c = c & B10011111; // clear Frame duration
                        c = c + (FD << 5); // Frame duration
                }

                MyMCP.writeGPIO(c);
                STROBE();

                //delay(8); // speed fun only if mode is 1F // holding note
        }

        //FIN();// not here  with allophone ta
}

int find_index(int value)
{
        if (value == 32)
                return (39); // space == silence
        for (int i = 0; i < sizeof(table) - 1; i++)
        {
                if (table[i] == value)
                {
                        return (i);
                }
        }

        return (39); // if not found send silence
}

void spell(char *message)
{

        for (int i = 0; i < strlen(message); i++)
        {

                STOP(CONTINU);
                initialPitch(0x49);
                Phon(find_index(message[i]));
                delay(500);
        }
        STOP(ARRET_LENT);
}

void dire(char *message)
{

        FD = 1; // default frame duration 0 is too fast
        Pl = 0;
        whisper = 0;
        STOP(0);
        initialPitch(0x39);
        for (int i = 0; i < strlen(message); i++)
        {

                position = find_index(message[i]);
                Serial.print(position);
                Speak(position);
        }
        FIN();
}

void Say(const unsigned char *str)
{

        INIT();

        char c;
        int l;
        byte b;

        l = pgm_read_byte(str + 0); // Number of byte to read

        for (int i = 1; i <= l; i++)
        {
                c = pgm_read_byte(str + i);

                if (i % 4 == 0) // 4th byte
                {
                        //Serial.print(i);
                        //Serial.print(" ");
                        //Serial.print(byte(c),HEX);
                        // c = c & B11100000; // clear PI = Robot voice
                        //c = b +16; // PI = noise = Whisper mode

                        //c= c +  B01100000;
                        //  b= map(analogRead(0),0,1000,0,4); // ajouter  le test sur 4 pour skiper
                        //  if (b<4){
                        //  c = c & B10011111; // clear Frame duration
                        //  c= c +  B00100000;
                        //   Serial.println(b);
                        //c = c + b<<5; // Frame duration
                        //}
                        //  c = c + 0<<5; // fun
                        //Serial.println(byte(c),HEX);
                }

                //  PORTD=c;
                MyMCP.writeGPIO(c);
                STROBE();
                TREQ();
                //  delay(10);// speed fun only if mode is 1F // holding note
        }

        FIN();
}

void initialPitch(byte pitch)
{

        digitalWrite(AO, 0);
        MyMCP.writeGPIO(pitch);
        delay(2); // delay(2) needed

        STROBE();
        TREQ();
}

void STOP(boolean continu)

{
        digitalWrite(AO, 1);

        MyMCP.writeGPIO(0x1B + (continu << 2)); // Arret lent REQN validée 1B, 1F pour long

        STROBE();
        TREQ();
}

// Wait for MEA
void TREQ()

{
        while (digitalRead(REQN))
                ;
}

void FIN()
{

        MyMCP.writeGPIO(0x00);
        STROBE();
        TREQ();

        MyMCP.writeGPIO(0x00);
        STROBE();
        TREQ();

        MyMCP.writeGPIO(0x00);
        STROBE();
        TREQ();

        MyMCP.writeGPIO(0x00);
        STROBE();
        TREQ();

        STOP(0);
}

void INIT()
{
        STOP(0);
        TREQ();
        digitalWrite(AO, 0);
        delay(2); // 2needed

        MyMCP.writeGPIO(0x34);

        delay(2); // delay(2) needed
        TREQ();   // ***
        STROBE();
        TREQ();
}

void STROBE()
{
        //delay(1);
        digitalWrite(WN, 0); // was 0
        //delay(2); // delay(2) needed
        digitalWrite(WN, 1); // was 1
        //delay(2); // delay(2) needed
}

void Phon(int index)
{

        int start;
        int end;
        char c;
        digitalWrite(AO, 0);

        Serial.print(index);
        Serial.print(" ");
        //Serial.print(tPhon[index]);
        Serial.print(" ");

        start = pgm_read_byte(&(phon[index * 2])) * 256 + pgm_read_byte(&(phon[index * 2 + 1]));
        Serial.print(start, HEX);

        end = start + pgm_read_byte(&(phon[start])) * 256 + pgm_read_byte(&(phon[start + 1])) - 1; // Adresse sur 2 bytes

        Serial.print(" ");
        Serial.println(end, HEX);
        //

        for (int i = start + 4; i < end; i++) // skip header ( page 38 / figure 18)
        {

                TREQ();
                c = pgm_read_byte(&(phon[i]));

                if ((i + 1) % 4 == 0) // 4th byte

                {

                        // Pitch increment
                        //c = c & B11100000; // clear PI = Robot voice
                        // c = c + 16; // 16 = noise = Whisper mode

                        // Frame duration
                        // c = c & B10011111; // clear Frame duration
                        // c = c + (FD<<5); // Frame duration
                }

                // Serial.print(c, HEX);
                // Serial.print(" ");

                MyMCP.writeGPIO(c);
                STROBE();

                //delay(8);// speed fun only if mode is 1F // holding note
        }
        // Serial.println("*");

        //FIN(); not here  with allophone ta
}

/*
   ███████ ███████ ████████ ██    ██ ██████
   ██      ██         ██    ██    ██ ██   ██
   ███████ █████      ██    ██    ██ ██████
        ██ ██         ██    ██    ██ ██
   ███████ ███████    ██     ██████  ██
 */

void setup()
{
        Serial.begin(9600);
        Wire.begin();
        DS1077_init(18); // pitch via clock control 22= 3.2Mhz 18 = 4Mhz 14 = 5Mhz
        MyMCP.writeIODIR(0x0);
        pinMode(REQN, INPUT);
        pinMode(AO, OUTPUT);
        pinMode(WN, OUTPUT);
        
        INIT();
        FIN();
        Say(spUNE);
        Say(spDEUX);
        Say(spTROIS);
        // Say(spQUATRE);
}
/*
   ██       ██████   ██████  ██████
   ██      ██    ██ ██    ██ ██   ██
   ██      ██    ██ ██    ██ ██████
   ██      ██    ██ ██    ██ ██
   ███████  ██████   ██████  ██
 */

void loop()
{

        dire("/bonj+w_r");
        delay(400);
        dire("&l&kt€Ronik miwzik");
        delay(400);
        dire("i$ b+i-n  ka+I+n -ma/$=+_ine");
        delay(400);
        dire("+?je pE ossi murmur*?");
        delay(400);
        dire("je suis un arduino qui parle"); // à reformater ;-)
        delay(400);
}

