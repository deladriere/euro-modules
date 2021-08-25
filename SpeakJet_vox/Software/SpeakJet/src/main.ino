#include <Arduino.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "SDU.h"
#include <SD.h> // 868
#include "OneButton.h"
#include "speakjet.h"
// voir first test arduino for better sound

/*
electronic font
 ▄         ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄   ▄         ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ 
▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░▌          
▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░▌       ▐░▌▐░▌   ▄   ▐░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░▌  ▐░▌  ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀█░█▀▀ ▐░▌       ▐░▌▐░▌ ▐░▌░▌ ▐░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀█░█▀▀ ▐░█▀▀▀▀▀▀▀▀▀ 
▐░▌       ▐░▌▐░▌       ▐░▌▐░▌     ▐░▌  ▐░▌       ▐░▌▐░▌▐░▌ ▐░▌▐░▌▐░▌       ▐░▌▐░▌     ▐░▌  ▐░▌          
▐░▌       ▐░▌▐░▌       ▐░▌▐░▌      ▐░▌ ▐░█▄▄▄▄▄▄▄█░▌▐░▌░▌   ▐░▐░▌▐░▌       ▐░▌▐░▌      ▐░▌ ▐░█▄▄▄▄▄▄▄▄▄ 
▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░░░░░░░░░░▌ ▐░░▌     ▐░░▌▐░▌       ▐░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌
 ▀         ▀  ▀         ▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀   ▀▀       ▀▀  ▀         ▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀ 
                                                                                                        
*/

#define BUSY 38
#define SW0 0
#define SW1 1

#define GREEN_LED 12
#define RED_LED 13

#define PIN_SPEAKING 11
#define PIN_BHF 9

#define IS_SPEAKING digitalRead(PIN_SPEAKING)

#define ON 0
#define OFF 1

#define GATE 2
#define PUSH 3

#define ROTA 6
#define ROTB 7

#define MODE_AUTO 1
#define MODE_TRIG 2
#define MODE_GATE 3

/*
 ▄               ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄   ▄            ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░▌             ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░▌          ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
 ▐░▌           ▐░▌ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░▌          ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ 
  ▐░▌         ▐░▌  ▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌          ▐░▌          ▐░▌          
   ▐░▌       ▐░▌   ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌     ▐░▌     ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░▌          ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄ 
    ▐░▌     ▐░▌    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░▌ ▐░▌          ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
     ▐░▌   ▐░▌     ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀█░█▀▀      ▐░▌     ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░▌          ▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀█░▌
      ▐░▌ ▐░▌      ▐░▌       ▐░▌▐░▌     ▐░▌       ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌          ▐░▌                    ▐░▌
       ▐░▐░▌       ▐░▌       ▐░▌▐░▌      ▐░▌  ▄▄▄▄█░█▄▄▄▄ ▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄█░▌
        ▐░▌        ▐░▌       ▐░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░▌ ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
         ▀          ▀         ▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀         ▀  ▀▀▀▀▀▀▀▀▀▀   ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀ 
                                                                                                                                                                                                      
*/

#define VERSION 0.1

int mainState;
int voiceState;

byte null = 0;
byte mode;
bool triggered;
bool pressed;
bool gated;
bool ungated;
bool longPressed;

volatile bool rotF;              // because use in rot
volatile int interruptCount = 0; // The rotary counter

unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 300;

char SayIt[] = {20, 96, 21, 114, 22, 88, 23, 5, 145, 131, 193, 6, 140, 154, 196, 6, 148, 7, 137, 7, 164, 18, 171, 136, 193};

char *mainFunctions[] = {
    "Phoneme",
    "Synth",
    "Code",
    "MIDI",
    "",
    "",
    ""};

int numFunctions = (sizeof(mainFunctions) / sizeof(mainFunctions[0]));
int function;

enum _EndLineCode
{
        endl
};

template <class T>
inline Print &operator<<(Print &obj, T arg)
{
        obj.print(arg);
        return obj;
}

inline Print &operator<<(Print &obj, _EndLineCode arg)
{
        obj.println();
        return obj;
}

/*
 ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▀▀▀▀▀█░█▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀ 
▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌    ▐░▌          ▐░▌               ▐░▌     ▐░▌          
▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄█░▌     ▐░▌    ▐░█▄▄▄▄▄▄▄▄▄ ▐░▌               ▐░▌     ▐░█▄▄▄▄▄▄▄▄▄ 
▐░▌       ▐░▌▐░░░░░░░░░░▌      ▐░▌    ▐░░░░░░░░░░░▌▐░▌               ▐░▌     ▐░░░░░░░░░░░▌
▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀█░▌     ▐░▌    ▐░█▀▀▀▀▀▀▀▀▀ ▐░▌               ▐░▌      ▀▀▀▀▀▀▀▀▀█░▌
▐░▌       ▐░▌▐░▌       ▐░▌     ▐░▌    ▐░▌          ▐░▌               ▐░▌               ▐░▌
▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▄▄▄▄▄█░▌    ▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄      ▐░▌      ▄▄▄▄▄▄▄▄▄█░▌
▐░░░░░░░░░░░▌▐░░░░░░░░░░▌▐░░░░░░░▌    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░░▌
 ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀      ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀       ▀       ▀▀▀▀▀▀▀▀▀▀▀ 
*/

auto &SpeakJet = Serial5;
SSD1306AsciiWire display;
OneButton button(PUSH, true);

/*
 ▄▄▄▄▄▄▄▄▄▄▄  ▄         ▄  ▄▄        ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄        ▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░▌      ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░▌      ▐░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░▌░▌     ▐░▌▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀█░█▀▀▀▀  ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░▌░▌     ▐░▌▐░█▀▀▀▀▀▀▀▀▀ 
▐░▌          ▐░▌       ▐░▌▐░▌▐░▌    ▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌▐░▌    ▐░▌▐░▌          
▐░█▄▄▄▄▄▄▄▄▄ ▐░▌       ▐░▌▐░▌ ▐░▌   ▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌ ▐░▌   ▐░▌▐░█▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░▌  ▐░▌  ▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌  ▐░▌  ▐░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀▀▀ ▐░▌       ▐░▌▐░▌   ▐░▌ ▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌   ▐░▌ ▐░▌ ▀▀▀▀▀▀▀▀▀█░▌
▐░▌          ▐░▌       ▐░▌▐░▌    ▐░▌▐░▌▐░▌               ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌    ▐░▌▐░▌          ▐░▌
▐░▌          ▐░█▄▄▄▄▄▄▄█░▌▐░▌     ▐░▐░▌▐░█▄▄▄▄▄▄▄▄▄      ▐░▌      ▄▄▄▄█░█▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌▐░▌     ▐░▐░▌ ▄▄▄▄▄▄▄▄▄█░▌
▐░▌          ▐░░░░░░░░░░░▌▐░▌      ▐░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌      ▐░░▌▐░░░░░░░░░░░▌
 ▀            ▀▀▀▀▀▀▀▀▀▀▀  ▀        ▀▀  ▀▀▀▀▀▀▀▀▀▀▀       ▀       ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀        ▀▀  ▀▀▀▀▀▀▀▀▀▀▀ 
                                                                                                                                                                                                                        
*/

void longPress()
{

        Serial.println("longPress...");
        longPressed = true;
        display.clear();
        delay(1000);
        mainState = 0;
        rotF = 1;
} // longPress

void displayFunctionList(int p)
{

        for (int i = 0; i < 4; i++)
        { // nbr of lines to display

                display.setCursor(13, i * 8); //
                String str2 = mainFunctions[i + p];
                display.clearToEOL();
                display.println(str2);
        }
}
void initDisplay()
{
        Wire.begin();
        Wire.setClock(1500000L);              // speed the display to the max
        display.begin(&Adafruit128x64, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
        display.clear();
        display.set1X();
        display.setFont(fixed_bold10x15);
        display.setRow(0);
        Wire.setClock(1500000L);
}
void Rotary()
{
        if (digitalRead(ROTA))
        {
                if (digitalRead(ROTB))
                {
                        interruptCount--;
                }
                else
                {
                        interruptCount++;
                }
                rotF = 1;
        }
}
// MIDI
void phonSynth()
{

        //     Serial << "voice " << voiceState << endl;

        //int phoneme = random(128, 199); //This range seemed to sound best but feel free to tweak.
        int phoneme = map(analogRead(6), 1023, 10, 128, 148);
        phoneme = constrain(phoneme, 128, 199);
        //Serial.println(phoneme);
        int pitch = map(analogRead(1), 1023, 10, 0, 255); //Mapped to range of available pitch values.
        pitch = constrain(pitch, 0, 255);
        int bend = map(analogRead(2), 0, 1023, 15, 0); //Mapped to range of available bend values.
        bend = constrain(bend, 0, 15);
        int speechSpeed = map(analogRead(3), 0, 1023, 0, 127); //Mapped to range of available speedvalues.
        speechSpeed = constrain(speechSpeed, 0, 127);
        int repeat = map(analogRead(4), 1003, 0, 0, 10);
        repeat = constrain(repeat, 0, 10);
        int volume = map(analogRead(5), 0, 1023, 127, 0);
        volume = constrain(volume, 0, 127);
        if (voiceState)
        {
                Serial << voiceState << " ";
        }
        switch (voiceState)
        {
        case 0: // idle
                if (gated)
                //  if (gated && !IS_SPEAKING)
                {
                        voiceState = 1;
                        break;
                }
                if (ungated)
                {
                        voiceState = 2;
                }

                break;
        case 1: // start speaking
                if (mode == 2)
                {
                        Serial5.print("\\0"); // Entrer en Mode Controle SCP
                        Serial5.print("R");   // Clear Buffer
                        Serial5.print("x");   // Exit SCP
                }

                SpeakJet.write(0x15);        //Send speed command
                SpeakJet.write(speechSpeed); //Send speed value mapped to hex
                SpeakJet.write(0x16);        //Send pitch command
                SpeakJet.write(pitch);       //Send pitch value mapped to hex
                SpeakJet.write(0x17);        //Send bend command
                SpeakJet.write(bend);        //Send bend value mapped to hex
                SpeakJet.write(20);          // volume
                SpeakJet.write(volume);
                SpeakJet.write(26); // repeat
                SpeakJet.write(repeat);
                SpeakJet.write(phoneme); //Send phoneme value mapped to hex
                Serial << phonL[phoneme - 128].label << endl;
                //Serial5.print("x")
                voiceState = 0;
                gated = false;

                break;

        case 2: // end of speak

                //   Serial5.print("\\0"); // Entrer en Mode Controle SCP
                //  Serial5.print("R");   // Clear Buffer
                //   Serial5.print("x");
                if (mode == 2)
                {
                        Serial5.print("\\0"); // Entrer en Mode Controle SCP

                        Serial5.print("R"); // Clear Buffer
                                            //     Serial5.print("S"); // stop

                        Serial5.print("x"); // Exit SCP

                        //   SpeakJet.write(255);
                }

                Serial << "low" << endl;
                ungated = false;
                voiceState = 0;

                break;
        default:
                break;
        }
}
void getUser() // voir sinovox
{
        /*
┌───┐
│ 2 │
│ 3 │
│ 1 │
└───┘
*/
        mode = digitalRead(SW0) + digitalRead(SW1) * 2;
}

void initPins()
{
        pinMode(ROTA, INPUT_PULLUP);
        pinMode(ROTB, INPUT_PULLUP);

        pinMode(SW0, INPUT_PULLUP);
        pinMode(SW1, INPUT_PULLUP);
        pinMode(PUSH, INPUT_PULLUP);
        pinMode(GATE, INPUT_PULLUP);
        pinMode(PIN_SPEAKING, INPUT_PULLUP);
        pinMode(PIN_BHF, INPUT_PULLUP);
        pinMode(RED_LED, OUTPUT);
        pinMode(GREEN_LED, OUTPUT);

        digitalWrite(12, HIGH);
        digitalWrite(13, HIGH);
}
void Buffer()
{
        if (!digitalRead(PIN_BHF))
        {

                digitalWrite(GREEN_LED, OFF);
                digitalWrite(BUSY, OFF);
        }
        else
        {
                digitalWrite(GREEN_LED, ON);
                digitalWrite(BUSY, ON);
        }
}

void Speaking()
{
        if (!digitalRead(PIN_SPEAKING))
        {

                //    digitalWrite(BUSY, OFF);
                digitalWrite(RED_LED, OFF);
        }
        else
        {

                //  digitalWrite(BUSY, ON);
                digitalWrite(RED_LED, ON);
        }
}

void justPressed()
{
        //voir gerty2 pour le debounce
        pressed = true;
}

void justGated()
{
        if (!digitalRead(GATE))
        {
                gated = true;
        }
        else
        {
                ungated = true;
        }
}
/*
 ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄         ▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌
▐░█▀▀▀▀▀▀▀▀▀ ▐░█▀▀▀▀▀▀▀▀▀  ▀▀▀▀█░█▀▀▀▀ ▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀█░▌
▐░▌          ▐░▌               ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌
▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄▄▄      ▐░▌     ▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄█░▌
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░▌       ▐░▌▐░░░░░░░░░░░▌
 ▀▀▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀      ▐░▌     ▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀▀▀ 
          ▐░▌▐░▌               ▐░▌     ▐░▌       ▐░▌▐░▌          
 ▄▄▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄      ▐░▌     ▐░█▄▄▄▄▄▄▄█░▌▐░▌          
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌     ▐░▌     ▐░░░░░░░░░░░▌▐░▌          
 ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀       ▀       ▀▀▀▀▀▀▀▀▀▀▀  ▀                                                                           
*/

void setup()
{
        initPins();

        analogReadResolution(10);

        attachInterrupt(PIN_SPEAKING, Speaking, CHANGE);
        attachInterrupt(PIN_BHF, Buffer, CHANGE); //TODO: why full in synth mode ?
        attachInterrupt(PUSH, justPressed, FALLING);
        attachInterrupt(GATE, justGated, CHANGE);
        attachInterrupt(ROTA, Rotary, CHANGE);

        //  button.attachDuringLongPress(longPress);

        SpeakJet.begin(9600); // set up SpeakJet library at 9600 bps

        Serial.begin(115200);

        SpeakJet.println(SayIt); // send it to the SpeakJet
        initDisplay();
        display.setCursor(0, 12);
        display.println("SpeakJet");
        delay(1000);
        display.clear();
        display.println(">");
        pressed = false;
        triggered = false;
        mainState = 0;      // start with idle mode
        rotF = 1;           // force first display
        interruptCount = 0; // setup first display
}
/*
 ▄            ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄ 
▐░▌          ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌
▐░▌          ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌
▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌       ▐░▌
▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄█░▌
▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌▐░░░░░░░░░░░▌
▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌▐░█▀▀▀▀▀▀▀▀▀ 
▐░▌          ▐░▌       ▐░▌▐░▌       ▐░▌▐░▌          
▐░█▄▄▄▄▄▄▄▄▄ ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌▐░▌          
▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌          
 ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀           
*/

void loop()
{
        getUser();
        button.tick();
        //    Serial << "voice " << mainState << endl;
        if (button.getPressedTicks() > 500 && mainState)
        {
                Serial5.print("\\0"); // Entrer en Mode Controle SCP
                Serial5.print("W");   // READY
                Serial5.print("x");
                display.clear();
                delay(400);
                display.println(">");
                interruptCount = 0;
                mainState = 0;
                rotF = 1;
                pressed = false;
        }
        switch (mainState)
        {
        case 0:
                if (rotF) // rotary is rotated
                {
                        interruptCount = constrain(interruptCount, 0, numFunctions - 4);
                        function = interruptCount;
                        displayFunctionList(function);
                        rotF = 0;
                }
                if (pressed) // function is selected
                {
                        mainState = function + 1;
                        display.clear();
                        String str2 = mainFunctions[function];
                        display.print(str2);
                        pressed = false;
                        voiceState = 0;
                }
                break;
        case 1: // voice synth

                phonSynth();
                break;
        case 2: // sound synth
                break;
        case 3: // code
                break;
        case 4: // MIDI
                break;
        }
}
