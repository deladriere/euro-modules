// Talkie library by Adafruit : https://github.com/adafruit/Talkie version february 10, 2017
// ResponsiveAnalogRead library https://github.com/dxinteractive/ResponsiveAnalogRead version Mar 16, 2017
/*
   repeat mode in library (cfr Talko)
   define mode in library like in Talko



 */

#include "talkie.h"
#include "5220_alphon.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Kfonts.h>
#include <Wire.h>
#include <Encoder.h>
#include "SAMD_AnalogCorrection.h"
#include <SD.h>
#include <SPI.h>

const unsigned char Eye [] PROGMEM = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x18, 0x38, 0x39, 0x9c, 0x49, 0x92,
        0x49, 0x92, 0x38, 0x1c, 0x1c, 0x78, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/*

   ██   ██  █████  ██████  ██████  ██     ██  █████  ██████  ███████
   ██   ██ ██   ██ ██   ██ ██   ██ ██     ██ ██   ██ ██   ██ ██
   ███████ ███████ ██████  ██   ██ ██  █  ██ ███████ ██████  █████
   ██   ██ ██   ██ ██   ██ ██   ██ ██ ███ ██ ██   ██ ██   ██ ██
   ██   ██ ██   ██ ██   ██ ██████   ███ ███  ██   ██ ██   ██ ███████

 */

#define SW0 0
#define SW1 1
#define GATE 2
#define PUSH 3
#define ROTA 6
#define ROTB 7
#define BLUE_LED  12
#define RED_LED 13

/*

   ██    ██  █████  ██████  ██  █████  ██████  ██      ███████ ███████
   ██    ██ ██   ██ ██   ██ ██ ██   ██ ██   ██ ██      ██      ██
   ██    ██ ███████ ██████  ██ ███████ ██████  ██      █████   ███████
   ██  ██  ██   ██ ██   ██ ██ ██   ██ ██   ██ ██      ██           ██
   ████   ██   ██ ██   ██ ██ ██   ██ ██████  ███████ ███████ ███████

 */

#define VERSION "Ver. 1.0"

char inputChar;
char* myFiles[20];  // max 20 files
int fileCounter=0; // file index
int fileNumber=0; // file number
int filePointer=0;
char fileName[13];


//char song[60][120] {}; // max 80 caracter per lines // 60 lines max
String lineLabel[20]; //max 20 lignes
byte stream[20][500]; //max 20 lignes de 500 bytes
int pointer=0;
bool part2=0;
byte msb=0;
byte lsb=0;
int linePointer=0;
int ligne=0;
int serialPointer=0;
char serialtext[40];

byte convert=0;


#define GATE_HIGH !digitalRead(GATE)

//const uint8_t *alphons[72]  = {sp_alphon1,sp_alphon2,sp_alphon3,sp_alphon4,sp_alphon5,sp_alphon6,sp_alphon7,sp_alphon8,sp_alphon9,sp_alphon10,sp_alphon11,sp_alphon12,sp_alphon13,sp_alphon14,sp_alphon15,sp_alphon16,sp_alphon17,sp_alphon18,sp_alphon19,sp_alphon20,sp_alphon21,sp_alphon22,sp_alphon23,sp_alphon24,sp_alphon25,sp_alphon26,sp_alphon27,sp_alphon28,sp_alphon29,sp_alphon30,sp_alphon31,sp_alphon32,sp_alphon33,sp_alphon34,sp_alphon35,sp_alphon36,sp_alphon37,sp_alphon38,sp_alphon39,sp_alphon40,sp_alphon41,sp_alphon42,sp_alphon43,sp_alphon44,sp_alphon45,sp_alphon46,sp_alphon47,sp_alphon48,sp_alphon49,sp_alphon50,sp_alphon51,sp_alphon52,sp_alphon53,sp_alphon54,sp_alphon55,sp_alphon56,sp_alphon57,sp_alphon58,sp_alphon59,sp_alphon60,sp_alphon61,sp_alphon62,sp_alphon63,sp_alphon64,sp_alphon65,sp_alphon66,sp_alphon67,sp_alphon68,sp_alphon69,sp_alphon70,sp_alphon71,sp_alphon7};

const uint8_t *alphons[125]={sp_alphon1,sp_alphon2,sp_alphon3,sp_alphon4,sp_alphon5,sp_alphon6,sp_alphon7,sp_alphon8,sp_alphon9,sp_alphon10,sp_alphon11,sp_alphon12,sp_alphon13,sp_alphon14,sp_alphon15,sp_alphon16,sp_alphon17,sp_alphon18,sp_alphon19,sp_alphon20,sp_alphon21,sp_alphon22,sp_alphon23,sp_alphon24,sp_alphon25,sp_alphon26,sp_alphon27,sp_alphon28,sp_alphon29,sp_alphon30,sp_alphon31,sp_alphon32,sp_alphon33,sp_alphon34,sp_alphon35,sp_alphon36,sp_alphon37,sp_alphon38,sp_alphon39,sp_alphon40,sp_alphon41,sp_alphon42,sp_alphon43,sp_alphon44,sp_alphon45,sp_alphon46,sp_alphon47,sp_alphon48,sp_alphon49,sp_alphon50,sp_alphon51,sp_alphon52,sp_alphon53,sp_alphon54,sp_alphon55,sp_alphon56,sp_alphon57,sp_alphon58,sp_alphon59,sp_alphon60,sp_alphon61,sp_alphon62,sp_alphon63,sp_alphon64,sp_alphon65,sp_alphon66,sp_alphon67,sp_alphon68,sp_alphon69,sp_alphon70,sp_alphon71,sp_alphon72,sp_alphon73,sp_alphon74,sp_alphon75,sp_alphon76,sp_alphon77,sp_alphon78,sp_alphon79,sp_alphon80,sp_alphon81,sp_alphon82,sp_alphon83,sp_alphon84,sp_alphon85,sp_alphon86,sp_alphon87,sp_alphon88,sp_alphon89,sp_alphon90,sp_alphon91,sp_alphon92,sp_alphon93,sp_alphon94,sp_alphon95,sp_alphon96,sp_alphon97,sp_alphon98,sp_alphon99,sp_alphon100,sp_alphon101,sp_alphon102,sp_alphon103,sp_alphon104,sp_alphon105,sp_alphon106,sp_alphon107,sp_alphon108,sp_alphon109,sp_alphon110,sp_alphon111,sp_alphon112,sp_alphon113,sp_alphon114,sp_alphon115,sp_alphon116,sp_alphon117,sp_alphon118,sp_alphon119,sp_alphon120,sp_alphon121,sp_alphon122,sp_alphon123,sp_alphon124,sp_alphon125};
const char *alloL[]  ={"AE1","AE1N","AH1","AH1N","AW1","AW1N","E1","E1N","EH1","EH1N","ER1N","I1","I1N","OO1","OW1N","U1","U1N","UH1","UH1M","UH1N","Y1","Y1N","ER1","OW1","Y2","AE2","AH2","AI2","AR2","AU2","AW2","E2","EER2","EH2","EHR2","EI2","ER2","I2","OI2","OO2","OOR2","OR2","OW2","U2","UH2","UU2","AE3","AH3","AI3","AR3","AU3","AW3","E3","EELL","EER3","EH3","EHR3","EI3","ER3","I3","ILL","ING2","OI3","OO3","OOR3","OR3","OW3","U3","UH3","ULL","UHL","UU3","L","L-","LL","M","MM","N","NN","NG1","NG2","R","W","WH","Y","B","BB","D","DD","G1","G2","GG","J","JJ","THV","THV-","V","VV","Z","ZZ","ZH","ZH-","K2","KH","KH-","KH1","KH2","P","PH","PH-","T","TH","TH-","CH","F","FF","HI","HO","HUH","S","SS","SH","SH-","THF","THF-","Pause1","Pause2"};

int allo;
int prevAllo;

bool busy=0;
int moy;


volatile float interruptCount=0; // The rotary counter
char* mainFunctions[12]={

        "Phoneme",
        "Numbers",
        "SD LPC",
        "Set Time","","",""
};

#define numFunctions 4
int function;

bool WTF; // to avoid led interrupt trigger by rotary !

int fin =0; /// pour pression longue

/*

   ██████  ██████       ██ ███████  ██████ ████████ ███████
   ██    ██ ██   ██      ██ ██      ██         ██    ██
   ██    ██ ██████       ██ █████   ██         ██    ███████
   ██    ██ ██   ██ ██   ██ ██      ██         ██         ██
   ██████  ██████   █████  ███████  ██████    ██    ███████

 */
Adafruit_SSD1306 display(0); // modif library for 64
Talkie voice;
Encoder myEnc(6, 7);
File root;



/*

   ███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████
   ██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██
   █████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████
   ██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
   ██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████

 */

void readLine()
{

        display.clearDisplay();
        display.setFont(&Orbitron_Light_22);
        display.setTextSize(1);
        display.setCursor(0,16);
        ligne=map(analogRead(A6),0,4095,linePointer-1,0);
        display.setTextSize(1);
        display.print(ligne+1);
        display.setFont(&Orbitron_Light_24);
        display.setCursor(0,40);
        display.setTextSize(1);
        display.print(lineLabel[ligne]);
        display.display();
        display.setFont();



}

void showArray()
{

        for (int l=0; l<linePointer; l++)
        {


                Serial.print(lineLabel[l]);
                for (int i=0; i<500;i++)
                {
                    Serial.print(stream[l][i]);
                }
                Serial.println("");

        }
}

void displayFilesList(int p)
{

        //  detachInterrupt(ROTA);
        display.clearDisplay();
        display.setCursor(0,12);
        display.print(">");
        for (int i = 0; i<4; i++) {

                display.setCursor(12,i*16+12);  //
                String str2=myFiles[i+p];
                str2.remove(str2.length()-4);

                display.print(str2);


        }
        display.display();
        //attachInterrupt(ROTA, rot, CHANGE);
}

bool isTxtFile(char* filename) {
        bool result;
        if(strstr(strlwr(filename),".lpc") && !strstr(strlwr(filename),"_") )  // filtering out just ".txt" file not containing "_"
        { result=true; }
        else
        { result= false; }
        return result;
}



void GetFilesList(File dir) {
        fileNumber=0;

        while(true) {


                File entry =  dir.openNextFile(); // was dir
                if (!entry) {
                        // no more files
                        break;
                }

                if(isTxtFile(entry.name()))  //check if it's a .txt file
                {

                        String str=entry.name();
                        //  str.remove(str.length()-4);


                        // Length (with one extra character for the null terminator)
                        int str_len = str.length() + 1;

                        // Prepare the character array (the buffer)
                        char char_array[str_len];

                        // Copy it over
                        str.toCharArray(char_array, str_len);
                        //Serial.println(char_array);

                        myFiles[fileCounter]=strdup(char_array);  //WTF is strdup ?
                        fileCounter++;

                }
                entry.close();

        }
        fileNumber=fileCounter;
}

int potRead(int pot)
{
        moy=0;
        for (int i = 0; i < 10; i++) {
                moy=moy +analogRead(pot);
                //  moy=moy +analogReadFast(pot,2);
        }
        return moy/10;

}

void displayFunctionList(int p)
{

        display.clearDisplay();

        display.setCursor(0,12);
        display.print(">");
        for (int i = 0; i<4; i++) {  // nbr of lines to display

                display.setCursor(12,i*16+12); //
                String str2=mainFunctions[i+p];
                display.print(str2);


        }
        display.display();

}


void rot()
{


        // delayMicroseconds(1000);
        if(digitalRead(ROTA))

                if(digitalRead(ROTB)) { interruptCount=interruptCount-0.5; }
                else
                {

                        interruptCount=interruptCount+0.5;
                }

}



void readSound()

{
        allo=map(potRead(6),4095,0,0,9);
        if (allo!=prevAllo)
        {
                prevAllo=allo;
                display.clearDisplay();
                display.setFont(&Orbitron_Light_22);
                display.setTextSize(1);
                display.setCursor(0,16);
                display.print(allo+1);
                display.setFont(&Orbitron_Light_24);
                display.setCursor(0,40);
                display.setTextSize(1);
                display.print(alloL[allo]);
                display.display();
        }
}

void setBLUE_ON()
{
        if (WTF) { // sorry but I cannot find how to stop this interrupt from happening when turning the rotary

                if (digitalRead(GATE))
                {
                        digitalWrite(BLUE_LED,HIGH);
                }
                else
                {
                        digitalWrite(BLUE_LED,LOW);
                }
        }
}


void splashScreen()
{
        display.clearDisplay();
        display.setFont(&Orbitron_Bold_14);
        display.setCursor(0,0);
        display.setTextColor(WHITE);
        // display.setTextSize(2);
        display.println("Polaxis");
        display.display();
        delay(500);

        //  display.setTextSize(3);
        display.setCursor(0,22);
        display.println("Emy");
//display.setTextSize(2);
        display.setCursor(0,50);
        display.println(VERSION);
        display.display();
        delay(1500);
}











const uint8_t spZERO[]     PROGMEM = {0x69, 0xFB, 0x59, 0xDD, 0x51, 0xD5, 0xD7, 0xB5, 0x6F, 0x0A, 0x78, 0xC0, 0x52, 0x01, 0x0F, 0x50, 0xAC, 0xF6, 0xA8, 0x16, 0x15, 0xF2, 0x7B, 0xEA, 0x19, 0x47, 0xD0, 0x64, 0xEB, 0xAD, 0x76, 0xB5, 0xEB, 0xD1, 0x96, 0x24, 0x6E, 0x62, 0x6D, 0x5B, 0x1F, 0x0A, 0xA7, 0xB9, 0xC5, 0xAB, 0xFD, 0x1A, 0x62, 0xF0, 0xF0, 0xE2, 0x6C, 0x73, 0x1C, 0x73, 0x52, 0x1D, 0x19, 0x94, 0x6F, 0xCE, 0x7D, 0xED, 0x6B, 0xD9, 0x82, 0xDC, 0x48, 0xC7, 0x2E, 0x71, 0x8B, 0xBB, 0xDF, 0xFF, 0x1F};
const uint8_t spONE[]      PROGMEM = {0x66, 0x4E, 0xA8, 0x7A, 0x8D, 0xED, 0xC4, 0xB5, 0xCD, 0x89, 0xD4, 0xBC, 0xA2, 0xDB, 0xD1, 0x27, 0xBE, 0x33, 0x4C, 0xD9, 0x4F, 0x9B, 0x4D, 0x57, 0x8A, 0x76, 0xBE, 0xF5, 0xA9, 0xAA, 0x2E, 0x4F, 0xD5, 0xCD, 0xB7, 0xD9, 0x43, 0x5B, 0x87, 0x13, 0x4C, 0x0D, 0xA7, 0x75, 0xAB, 0x7B, 0x3E, 0xE3, 0x19, 0x6F, 0x7F, 0xA7, 0xA7, 0xF9, 0xD0, 0x30, 0x5B, 0x1D, 0x9E, 0x9A, 0x34, 0x44, 0xBC, 0xB6, 0x7D, 0xFE, 0x1F};
const uint8_t spTWO[]      PROGMEM = {0x06, 0xB8, 0x59, 0x34, 0x00, 0x27, 0xD6, 0x38, 0x60, 0x58, 0xD3, 0x91, 0x55, 0x2D, 0xAA, 0x65, 0x9D, 0x4F, 0xD1, 0xB8, 0x39, 0x17, 0x67, 0xBF, 0xC5, 0xAE, 0x5A, 0x1D, 0xB5, 0x7A, 0x06, 0xF6, 0xA9, 0x7D, 0x9D, 0xD2, 0x6C, 0x55, 0xA5, 0x26, 0x75, 0xC9, 0x9B, 0xDF, 0xFC, 0x6E, 0x0E, 0x63, 0x3A, 0x34, 0x70, 0xAF, 0x3E, 0xFF, 0x1F};
const uint8_t spTHREE[]    PROGMEM = {0x0C, 0xE8, 0x2E, 0x94, 0x01, 0x4D, 0xBA, 0x4A, 0x40, 0x03, 0x16, 0x68, 0x69, 0x36, 0x1C, 0xE9, 0xBA, 0xB8, 0xE5, 0x39, 0x70, 0x72, 0x84, 0xDB, 0x51, 0xA4, 0xA8, 0x4E, 0xA3, 0xC9, 0x77, 0xB1, 0xCA, 0xD6, 0x52, 0xA8, 0x71, 0xED, 0x2A, 0x7B, 0x4B, 0xA6, 0xE0, 0x37, 0xB7, 0x5A, 0xDD, 0x48, 0x8E, 0x94, 0xF1, 0x64, 0xCE, 0x6D, 0x19, 0x55, 0x91, 0xBC, 0x6E, 0xD7, 0xAD, 0x1E, 0xF5, 0xAA, 0x77, 0x7A, 0xC6, 0x70, 0x22, 0xCD, 0xC7, 0xF9, 0x89, 0xCF, 0xFF, 0x03};
const uint8_t spFOUR[]     PROGMEM = {0x08, 0x68, 0x21, 0x0D, 0x03, 0x04, 0x28, 0xCE, 0x92, 0x03, 0x23, 0x4A, 0xCA, 0xA6, 0x1C, 0xDA, 0xAD, 0xB4, 0x70, 0xED, 0x19, 0x64, 0xB7, 0xD3, 0x91, 0x45, 0x51, 0x35, 0x89, 0xEA, 0x66, 0xDE, 0xEA, 0xE0, 0xAB, 0xD3, 0x29, 0x4F, 0x1F, 0xFA, 0x52, 0xF6, 0x90, 0x52, 0x3B, 0x25, 0x7F, 0xDD, 0xCB, 0x9D, 0x72, 0x72, 0x8C, 0x79, 0xCB, 0x6F, 0xFA, 0xD2, 0x10, 0x9E, 0xB4, 0x2C, 0xE1, 0x4F, 0x25, 0x70, 0x3A, 0xDC, 0xBA, 0x2F, 0x6F, 0xC1, 0x75, 0xCB, 0xF2, 0xFF};
const uint8_t spFIVE[]     PROGMEM = {0x08, 0x68, 0x4E, 0x9D, 0x02, 0x1C, 0x60, 0xC0, 0x8C, 0x69, 0x12, 0xB0, 0xC0, 0x28, 0xAB, 0x8C, 0x9C, 0xC0, 0x2D, 0xBB, 0x38, 0x79, 0x31, 0x15, 0xA3, 0xB6, 0xE4, 0x16, 0xB7, 0xDC, 0xF5, 0x6E, 0x57, 0xDF, 0x54, 0x5B, 0x85, 0xBE, 0xD9, 0xE3, 0x5C, 0xC6, 0xD6, 0x6D, 0xB1, 0xA5, 0xBF, 0x99, 0x5B, 0x3B, 0x5A, 0x30, 0x09, 0xAF, 0x2F, 0xED, 0xEC, 0x31, 0xC4, 0x5C, 0xBE, 0xD6, 0x33, 0xDD, 0xAD, 0x88, 0x87, 0xE2, 0xD2, 0xF2, 0xF4, 0xE0, 0x16, 0x2A, 0xB2, 0xE3, 0x63, 0x1F, 0xF9, 0xF0, 0xE7, 0xFF, 0x01};
const uint8_t spSIX[]      PROGMEM = {0x04, 0xF8, 0xAD, 0x4C, 0x02, 0x16, 0xB0, 0x80, 0x06, 0x56, 0x35, 0x5D, 0xA8, 0x2A, 0x6D, 0xB9, 0xCD, 0x69, 0xBB, 0x2B, 0x55, 0xB5, 0x2D, 0xB7, 0xDB, 0xFD, 0x9C, 0x0D, 0xD8, 0x32, 0x8A, 0x7B, 0xBC, 0x02, 0x00, 0x03, 0x0C, 0xB1, 0x2E, 0x80, 0xDF, 0xD2, 0x35, 0x20, 0x01, 0x0E, 0x60, 0xE0, 0xFF, 0x01};
const uint8_t spSEVEN[]    PROGMEM = {0x0C, 0xF8, 0x5E, 0x4C, 0x01, 0xBF, 0x95, 0x7B, 0xC0, 0x02, 0x16, 0xB0, 0xC0, 0xC8, 0xBA, 0x36, 0x4D, 0xB7, 0x27, 0x37, 0xBB, 0xC5, 0x29, 0xBA, 0x71, 0x6D, 0xB7, 0xB5, 0xAB, 0xA8, 0xCE, 0xBD, 0xD4, 0xDE, 0xA6, 0xB2, 0x5A, 0xB1, 0x34, 0x6A, 0x1D, 0xA7, 0x35, 0x37, 0xE5, 0x5A, 0xAE, 0x6B, 0xEE, 0xD2, 0xB6, 0x26, 0x4C, 0x37, 0xF5, 0x4D, 0xB9, 0x9A, 0x34, 0x39, 0xB7, 0xC6, 0xE1, 0x1E, 0x81, 0xD8, 0xA2, 0xEC, 0xE6, 0xC7, 0x7F, 0xFE, 0xFB, 0x7F};
const uint8_t spEIGHT[]    PROGMEM = {0x65, 0x69, 0x89, 0xC5, 0x73, 0x66, 0xDF, 0xE9, 0x8C, 0x33, 0x0E, 0x41, 0xC6, 0xEA, 0x5B, 0xEF, 0x7A, 0xF5, 0x33, 0x25, 0x50, 0xE5, 0xEA, 0x39, 0xD7, 0xC5, 0x6E, 0x08, 0x14, 0xC1, 0xDD, 0x45, 0x64, 0x03, 0x00, 0x80, 0x00, 0xAE, 0x70, 0x33, 0xC0, 0x73, 0x33, 0x1A, 0x10, 0x40, 0x8F, 0x2B, 0x14, 0xF8, 0x7F};
const uint8_t spNINE[]     PROGMEM = {0xE6, 0xA8, 0x1A, 0x35, 0x5D, 0xD6, 0x9A, 0x35, 0x4B, 0x8C, 0x4E, 0x6B, 0x1A, 0xD6, 0xA6, 0x51, 0xB2, 0xB5, 0xEE, 0x58, 0x9A, 0x13, 0x4F, 0xB5, 0x35, 0x67, 0x68, 0x26, 0x3D, 0x4D, 0x97, 0x9C, 0xBE, 0xC9, 0x75, 0x2F, 0x6D, 0x7B, 0xBB, 0x5B, 0xDF, 0xFA, 0x36, 0xA7, 0xEF, 0xBA, 0x25, 0xDA, 0x16, 0xDF, 0x69, 0xAC, 0x23, 0x05, 0x45, 0xF9, 0xAC, 0xB9, 0x8F, 0xA3, 0x97, 0x20, 0x73, 0x9F, 0x54, 0xCE, 0x1E, 0x45, 0xC2, 0xA2, 0x4E, 0x3E, 0xD3, 0xD5, 0x3D, 0xB1, 0x79, 0x24, 0x0D, 0xD7, 0x48, 0x4C, 0x6E, 0xE1, 0x2C, 0xDE, 0xFF, 0x0F};
const uint8_t spTEN[]      PROGMEM = {0x0E, 0x38, 0x3C, 0x2D, 0x00, 0x5F, 0xB6, 0x19, 0x60, 0xA8, 0x90, 0x93, 0x36, 0x2B, 0xE2, 0x99, 0xB3, 0x4E, 0xD9, 0x7D, 0x89, 0x85, 0x2F, 0xBE, 0xD5, 0xAD, 0x4F, 0x3F, 0x64, 0xAB, 0xA4, 0x3E, 0xBA, 0xD3, 0x59, 0x9A, 0x2E, 0x75, 0xD5, 0x39, 0x6D, 0x6B, 0x0A, 0x2D, 0x3C, 0xEC, 0xE5, 0xDD, 0x1F, 0xFE, 0xB0, 0xE7, 0xFF, 0x03};
const uint8_t spELEVEN[]   PROGMEM = {0xA5, 0xEF, 0xD6, 0x50, 0x3B, 0x67, 0x8F, 0xB9, 0x3B, 0x23, 0x49, 0x7F, 0x33, 0x87, 0x31, 0x0C, 0xE9, 0x22, 0x49, 0x7D, 0x56, 0xDF, 0x69, 0xAA, 0x39, 0x6D, 0x59, 0xDD, 0x82, 0x56, 0x92, 0xDA, 0xE5, 0x74, 0x9D, 0xA7, 0xA6, 0xD3, 0x9A, 0x53, 0x37, 0x99, 0x56, 0xA6, 0x6F, 0x4F, 0x59, 0x9D, 0x7B, 0x89, 0x2F, 0xDD, 0xC5, 0x28, 0xAA, 0x15, 0x4B, 0xA3, 0xD6, 0xAE, 0x8C, 0x8A, 0xAD, 0x54, 0x3B, 0xA7, 0xA9, 0x3B, 0xB3, 0x54, 0x5D, 0x33, 0xE6, 0xA6, 0x5C, 0xCB, 0x75, 0xCD, 0x5E, 0xC6, 0xDA, 0xA4, 0xCA, 0xB9, 0x35, 0xAE, 0x67, 0xB8, 0x46, 0x40, 0xB6, 0x28, 0xBB, 0xF1, 0xF6, 0xB7, 0xB9, 0x47, 0x20, 0xB6, 0x28, 0xBB, 0xFF, 0x0F};
const uint8_t spTWELVE[]   PROGMEM = {0x09, 0x98, 0xDA, 0x22, 0x01, 0x37, 0x78, 0x1A, 0x20, 0x85, 0xD1, 0x50, 0x3A, 0x33, 0x11, 0x81, 0x5D, 0x5B, 0x95, 0xD4, 0x44, 0x04, 0x76, 0x9D, 0xD5, 0xA9, 0x3A, 0xAB, 0xF0, 0xA1, 0x3E, 0xB7, 0xBA, 0xD5, 0xA9, 0x2B, 0xEB, 0xCC, 0xA0, 0x3E, 0xB7, 0xBD, 0xC3, 0x5A, 0x3B, 0xC8, 0x69, 0x67, 0xBD, 0xFB, 0xE8, 0x67, 0xBF, 0xCA, 0x9D, 0xE9, 0x74, 0x08, 0xE7, 0xCE, 0x77, 0x78, 0x06, 0x89, 0x32, 0x57, 0xD6, 0xF1, 0xF1, 0x8F, 0x7D, 0xFE, 0x1F};
const uint8_t spTHIR_[]    PROGMEM = {0x04, 0xA8, 0xBE, 0x5C, 0x00, 0xDD, 0xA5, 0x11, 0xA0, 0xFA, 0x72, 0x02, 0x74, 0x97, 0xC6, 0x01, 0x09, 0x9C, 0xA6, 0xAB, 0x30, 0x0D, 0xCE, 0x7A, 0xEA, 0x6A, 0x4A, 0x39, 0x35, 0xFB, 0xAA, 0x8B, 0x1B, 0xC6, 0x76, 0xF7, 0xAB, 0x2E, 0x79, 0x19, 0xCA, 0xD5, 0xEF, 0xCA, 0x57, 0x08, 0x14, 0xA1, 0xDC, 0x45, 0x64, 0x03, 0x00, 0xC0, 0xFF, 0x03};
const uint8_t spFIF_[]     PROGMEM = {0x08, 0x98, 0x31, 0x93, 0x02, 0x1C, 0xE0, 0x80, 0x07, 0x5A, 0xD6, 0x1C, 0x6B, 0x78, 0x2E, 0xBD, 0xE5, 0x2D, 0x4F, 0xDD, 0xAD, 0xAB, 0xAA, 0x6D, 0xC9, 0x23, 0x02, 0x56, 0x4C, 0x93, 0x00, 0x05, 0x10, 0x90, 0x89, 0x31, 0xFC, 0x3F};
const uint8_t sp_TEEN[]    PROGMEM = {0x09, 0x58, 0x2A, 0x25, 0x00, 0xCB, 0x9F, 0x95, 0x6C, 0x14, 0x21, 0x89, 0xA9, 0x78, 0xB3, 0x5B, 0xEC, 0xBA, 0xB5, 0x23, 0x13, 0x46, 0x97, 0x99, 0x3E, 0xD6, 0xB9, 0x2E, 0x79, 0xC9, 0x5B, 0xD8, 0x47, 0x41, 0x53, 0x1F, 0xC7, 0xE1, 0x9C, 0x85, 0x54, 0x22, 0xEC, 0xFA, 0xDB, 0xDD, 0x23, 0x93, 0x49, 0xB8, 0xE6, 0x78, 0xFF, 0x3F};
const uint8_t spTWENTY[]   PROGMEM = {0x0A, 0xE8, 0x4A, 0xCD, 0x01, 0xDB, 0xB9, 0x33, 0xC0, 0xA6, 0x54, 0x0C, 0xA4, 0x34, 0xD9, 0xF2, 0x0A, 0x6C, 0xBB, 0xB3, 0x53, 0x0E, 0x5D, 0xA6, 0x25, 0x9B, 0x6F, 0x75, 0xCA, 0x61, 0x52, 0xDC, 0x74, 0x49, 0xA9, 0x8A, 0xC4, 0x76, 0x4D, 0xD7, 0xB1, 0x76, 0xC0, 0x55, 0xA6, 0x65, 0xD8, 0x26, 0x99, 0x5C, 0x56, 0xAD, 0xB9, 0x25, 0x23, 0xD5, 0x7C, 0x32, 0x96, 0xE9, 0x9B, 0x20, 0x7D, 0xCB, 0x3C, 0xFA, 0x55, 0xAE, 0x99, 0x1A, 0x30, 0xFC, 0x4B, 0x3C, 0xFF, 0x1F};
const uint8_t spT[]        PROGMEM = {0x01, 0xD8, 0xB6, 0xDD, 0x01, 0x2F, 0xF4, 0x38, 0x60, 0xD5, 0xD1, 0x91, 0x4D, 0x97, 0x84, 0xE6, 0x4B, 0x4E, 0x36, 0xB2, 0x10, 0x67, 0xCD, 0x19, 0xD9, 0x2C, 0x01, 0x94, 0xF1, 0x78, 0x66, 0x33, 0xEB, 0x79, 0xAF, 0x7B, 0x57, 0x87, 0x36, 0xAF, 0x52, 0x08, 0x9E, 0x6B, 0xEA, 0x5A, 0xB7, 0x7A, 0x94, 0x73, 0x45, 0x47, 0xAC, 0x5A, 0x9C, 0xAF, 0xFF, 0x07};
const uint8_t spHUNDRED[]  PROGMEM = {0x04, 0xC8, 0x7E, 0x5C, 0x02, 0x0A, 0xA8, 0x62, 0x43, 0x03, 0xA7, 0xA8, 0x62, 0x43, 0x4B, 0x97, 0xDC, 0xF2, 0x14, 0xC5, 0xA7, 0x9B, 0x7A, 0xD3, 0x95, 0x37, 0xC3, 0x1E, 0x16, 0x4A, 0x66, 0x36, 0xF3, 0x5A, 0x89, 0x6E, 0xD4, 0x30, 0x55, 0xB5, 0x32, 0xB7, 0x31, 0xB5, 0xC1, 0x69, 0x2C, 0xE9, 0xF7, 0xBC, 0x96, 0x12, 0x39, 0xD4, 0xB5, 0xFD, 0xDA, 0x9B, 0x0F, 0xD1, 0x90, 0xEE, 0xF5, 0xE4, 0x17, 0x02, 0x45, 0x28, 0x77, 0x11, 0xD9, 0x40, 0x9E, 0x45, 0xDD, 0x2B, 0x33, 0x71, 0x7A, 0xBA, 0x0B, 0x13, 0x95, 0x2D, 0xF9, 0xF9, 0x7F};
const uint8_t spTHOUSAND[] PROGMEM = {0x0C, 0xE8, 0x2E, 0xD4, 0x02, 0x06, 0x98, 0xD2, 0x55, 0x03, 0x16, 0x68, 0x7D, 0x17, 0xE9, 0x6E, 0xBC, 0x65, 0x8C, 0x45, 0x6D, 0xA6, 0xE9, 0x96, 0xDD, 0xDE, 0xF6, 0xB6, 0xB7, 0x5E, 0x75, 0xD4, 0x93, 0xA5, 0x9C, 0x7B, 0x57, 0xB3, 0x6E, 0x7D, 0x12, 0x19, 0xAD, 0xDC, 0x29, 0x8D, 0x4F, 0x93, 0xB4, 0x87, 0xD2, 0xB6, 0xFC, 0xDD, 0xAC, 0x22, 0x56, 0x02, 0x70, 0x18, 0xCA, 0x18, 0x26, 0xB5, 0x90, 0xD4, 0xDE, 0x6B, 0x29, 0xDA, 0x2D, 0x25, 0x17, 0x8D, 0x79, 0x88, 0xD4, 0x48, 0x79, 0x5D, 0xF7, 0x74, 0x75, 0xA1, 0x94, 0xA9, 0xD1, 0xF2, 0xED, 0x9E, 0xAA, 0x51, 0xA6, 0xD4, 0x9E, 0x7F, 0xED, 0x6F, 0xFE, 0x2B, 0xD1, 0xC7, 0x3D, 0x89, 0xFA, 0xB7, 0x0D, 0x57, 0xD3, 0xB4, 0xF5, 0x37, 0x55, 0x37, 0x2E, 0xE6, 0xB2, 0xD7, 0x57, 0xFF, 0x0F};
const uint8_t spAND[]      PROGMEM = {0xA9, 0x6B, 0x21, 0xB9, 0x22, 0x66, 0x9F, 0xAE, 0xC7, 0xE1, 0x70, 0x7B, 0x72, 0xBB, 0x5B, 0xDF, 0xEA, 0x56, 0xBB, 0x5C, 0x65, 0xCB, 0x66, 0xC5, 0x3D, 0x67, 0xD7, 0xAB, 0x6D, 0x2E, 0x64, 0x30, 0x93, 0xEE, 0xB1, 0xCD, 0x3D, 0x92, 0xB9, 0x9A, 0xDA, 0xB2, 0x8E, 0x40, 0x12, 0x9A, 0x6A, 0xEB, 0x96, 0x8F, 0x78, 0x98, 0xB3, 0x2A, 0xB4, 0xD3, 0x48, 0xAA, 0x2F, 0x7D, 0xA7, 0x7B, 0xFB, 0x0C, 0x73, 0x71, 0x5C, 0xCE, 0x6E, 0x5C, 0x52, 0x6C, 0x73, 0x79, 0x9A, 0x13, 0x4B, 0x89, 0x45, 0xE9, 0x6E, 0x49, 0x42, 0xA9, 0x57, 0xFF, 0x3F};
const uint8_t spMINUS[]    PROGMEM = {0xE6, 0x28, 0xC4, 0xF8, 0x44, 0x9A, 0xFB, 0xCD, 0xAD, 0x8D, 0x2A, 0x4E, 0x4A, 0xBC, 0xB8, 0x8C, 0xB9, 0x8A, 0xA9, 0x48, 0xED, 0x72, 0x87, 0xD3, 0x74, 0x3B, 0x1A, 0xA9, 0x9D, 0x6F, 0xB3, 0xCA, 0x5E, 0x8C, 0xC3, 0x7B, 0xF2, 0xCE, 0x5A, 0x5E, 0x35, 0x66, 0x5A, 0x3A, 0xAE, 0x55, 0xEB, 0x9A, 0x57, 0x75, 0xA9, 0x29, 0x6B, 0xEE, 0xB6, 0xD5, 0x4D, 0x37, 0xEF, 0xB5, 0x5D, 0xC5, 0x95, 0x84, 0xE5, 0xA6, 0xFC, 0x30, 0xE0, 0x97, 0x0C, 0x0D, 0x58, 0x40, 0x03, 0x1C, 0xA0, 0xC0, 0xFF, 0x03};
const uint8_t spMILLI[]    PROGMEM = {0x6E, 0xF0, 0x8A, 0xB3, 0x4B, 0xEB, 0xC6, 0xAE, 0x36, 0xA7, 0x1A, 0x3A, 0x54, 0x53, 0xD6, 0xDC, 0xEC, 0x66, 0x23, 0xDF, 0x58, 0x26, 0x43, 0xB4, 0xCD, 0xEA, 0x74, 0x5D, 0x94, 0x46, 0xF0, 0x96, 0x3B, 0x9D, 0x79, 0x98, 0x26, 0x75, 0xDB, 0xB3, 0xD7, 0xB6, 0xF5, 0x90, 0xA8, 0x91, 0x9F, 0xEA, 0x9E, 0xEE, 0xE9, 0x9B, 0x20, 0x7D, 0xCB, 0xFF, 0x03};
const uint8_t spVOLTS[]    PROGMEM = {0xA0, 0xDA, 0xA2, 0xB2, 0x3A, 0x44, 0x55, 0x9C, 0xFA, 0xB0, 0xBA, 0x46, 0x72, 0xDA, 0xD1, 0xDB, 0xAE, 0x47, 0x59, 0x61, 0xED, 0x28, 0x79, 0xED, 0x45, 0xAF, 0x5A, 0xDF, 0x60, 0xF4, 0x39, 0x69, 0xAB, 0x63, 0xD9, 0x3B, 0xD2, 0xBC, 0x24, 0xA5, 0xF5, 0xB6, 0x0F, 0x80, 0x01, 0x3E, 0x63, 0x65, 0xC0, 0x5F, 0x63, 0x12, 0x90, 0x80, 0x06, 0x24, 0x20, 0x01, 0x0E, 0xFC, 0x3F};

const uint8_t spYOU_ARE_FIRED[]    PROGMEM =

{0xe8,0xa8,0x52,0xcc,0x52,0xa2,0x8a,0xbc,0x70,0x96,0x38,0x0f,0xa3,0xb2,0x22,0x49,0xe2,0xa2,0x8c,0x8b,0x6b,0x00,0xcf,0x8e,0xab,0x25,0x6a,0x81,0x45,0x5b,0xae,0x8c,0xa0,0x66,0x35,0x0d,0x79,0x33,0x82,0x92,0xc5,0x2d,0x65,0x6b,0x0b,0x72,0x36,0x97,0xa6,0xb7,0x25,0xca,0x21,0x83,0x5a,0xbd,0x86,0x62,0xe8,0x72,0x73,0x71,0x6a,0xe2,0x6e,0xd2,0xcd,0xcc,0x29,0x89,0xab,0x0a,0x33,0xf7,0xa4,0x24,0xca,0xca,0xbd,0x2a,0x92,0x90,0xa7,0x48,0x0f,0xaf,0x4c,0xab,0xcc,0xe9,0x23,0xdc,0xd4,0x4e,0xf1,0xb2,0xa9,0x36,0xe7,0x27,0xc3,0xcf,0xb6,0x5b,0x5d,0x9f,0x2e,0xbf,0xd8,0x1e,0x73,0x7d,0xba,0xfc,0xa6,0x63,0x22,0x65,0xe9,0xf1,0x9b,0x8b,0x8e,0xd0,0xa7,0xc7,0x6f,0x3e,0xca,0x53,0x9f,0x9e,0xa0,0x87,0x08,0x2b,0x7d,0xb2,0xc2,0x1e,0xc3,0xb4,0x6d,0xc9,0x88,0x5a,0x75,0xe5,0xf6,0x39,0x2d,0xee,0x35,0x85,0xca,0xe6,0xb4,0xa4,0x97,0x32,0x6c,0x59,0x52,0xb2,0x96,0x5d,0xb8,0x6d,0x69,0xc9,0x5b,0x56,0xb1,0xb1,0xd7,0xa1,0xac,0x85,0x2d,0xd2,0x67,0xbb,0xba,0x04,0xf6,0x6c,0x5f,0xa4,0x86,0x64,0x25,0x7a,0x22,0x91,0x18,0x8b,0x67,0xf5,0xf3,0x46,0x6c,0xa9,0x41,0x84,0xce,0x63,0xa1,0xa8,0x15,0x32,0x9d,0x88,0x85,0xe2,0x64,0xc4,0x2a,0xcd,0x32,0x00,0x0,0x0,0x0,0xf0,0xff,0xff};

/* Say any number between -999,999 and 999,999 */
void sayNumber(long n) {
        if (n<0) {
                voice.say(spMINUS);
                sayNumber(-n);
        } else if (n==0) {
                voice.say(spZERO);
        } else {
                if (n>=1000) {
                        int thousands = n / 1000;
                        sayNumber(thousands);
                        voice.say(spTHOUSAND);
                        n %= 1000;
                        if ((n > 0) && (n<100)) voice.say(spAND);
                }
                if (n>=100) {
                        int hundreds = n / 100;
                        sayNumber(hundreds);
                        voice.say(spHUNDRED);
                        n %= 100;
                        if (n > 0) voice.say(spAND);
                }
                if (n>19) {
                        int tens = n / 10;
                        switch (tens) {
                        case 2: voice.say(spTWENTY); break;
                        case 3: voice.say(spTHIR_); voice.say(spT); break;
                        case 4: voice.say(spFOUR); voice.say(spT);  break;
                        case 5: voice.say(spFIF_);  voice.say(spT); break;
                        case 6: voice.say(spSIX);  voice.say(spT); break;
                        case 7: voice.say(spSEVEN);  voice.say(spT); break;
                        case 8: voice.say(spEIGHT);  voice.say(spT); break;
                        case 9: voice.say(spNINE);  voice.say(spT); break;
                        }
                        n %= 10;
                }
                switch(n) {
                case 1: voice.say(spONE); break;
                case 2: voice.say(spTWO); break;
                case 3: voice.say(spTHREE); break;
                case 4: voice.say(spFOUR); break;
                case 5: voice.say(spFIVE); break;
                case 6: voice.say(spSIX); break;
                case 7: voice.say(spSEVEN); break;
                case 8: voice.say(spEIGHT); break;
                case 9: voice.say(spNINE); break;
                case 10: voice.say(spTEN); break;
                case 11: voice.say(spELEVEN); break;
                case 12: voice.say(spTWELVE); break;
                case 13: voice.say(spTHIR_); voice.say(sp_TEEN); break;
                case 14: voice.say(spFOUR); voice.say(sp_TEEN); break;
                case 15: voice.say(spFIF_); voice.say(sp_TEEN); break;
                case 16: voice.say(spSIX); voice.say(sp_TEEN); break;
                case 17: voice.say(spSEVEN); voice.say(sp_TEEN); break;
                case 18: voice.say(spEIGHT); voice.say(sp_TEEN); break;
                case 19: voice.say(spNINE); voice.say(sp_TEEN); break;
                }
        }
}


/*

   ███████ ███████ ████████ ██    ██ ██████
   ██      ██         ██    ██    ██ ██   ██
   ███████ █████      ██    ██    ██ ██████
        ██ ██         ██    ██    ██ ██
   ███████ ███████    ██     ██████  ██

 */
void setup() {



        pinMode(SW0, INPUT_PULLUP);
        pinMode(SW1, INPUT_PULLUP);
        pinMode(ROTA, INPUT_PULLUP);
        pinMode(ROTB, INPUT_PULLUP);
        pinMode(PUSH, INPUT_PULLUP);
        pinMode(GATE, INPUT_PULLUP);
        pinMode(BLUE_LED,OUTPUT);
        pinMode(RED_LED,OUTPUT);
        // turn transistorized outputs low ;
        digitalWrite(BLUE_LED,HIGH);
        digitalWrite(RED_LED,HIGH);

        analogReadResolution(12);
        analogReadCorrection(14, 2065);
        //  analogSound.setAnalogResolution(4096);

        display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
        display.clearDisplay();

        display.drawBitmap(35,0, Eye, 16, 16, WHITE);
        display.drawBitmap(75,0, Eye, 16, 16, WHITE);
        display.display();
        delay(500);
        //splashScreen();

        Serial.begin(115200);

        Serial.print("Initializing SD card...");

        if (!SD.begin(10)) {
                Serial.println("initialization failed!");
                display.setTextSize(2);
                display.setTextColor(BLACK,WHITE);
                display.println(" Error #1 ");
                display.setTextColor(WHITE);
                display.println("SD card");
                display.println("failed to ");
                display.println("initialize");

                display.display();
                while(1);
        }
        Serial.println("initialization done.");

        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.display();



        //  voice.say(spYOU_ARE_FIRED);

        Wire.setClock(2000000L);  //magnifique ! ? plante à 3mhz
        //  attachInterrupt(GATE, setBLUE_ON, CHANGE);
        attachInterrupt(ROTA, rot, CHANGE);

        root = SD.open("/");


        GetFilesList(root);
        Serial.println("done!");

        Serial.println(fileNumber);


}

/*

   ██       ██████   ██████  ██████
   ██      ██    ██ ██    ██ ██   ██
   ██      ██    ██ ██    ██ ██████
   ██      ██    ██ ██    ██ ██
   ███████  ██████   ██████  ██

 */

void loop() {

        // get ready to grab new function

        digitalWrite(BLUE_LED,HIGH);
        detachInterrupt(GATE);
        WTF=LOW;
        delay(300);
        interruptCount=0;
        display.setFont(&Orbitron_Light_16);
        display.setTextSize(1);

        // grab function

        do {

                interruptCount=constrain(interruptCount,0,numFunctions-1);

                function=interruptCount;
                displayFunctionList(function);



        } while(digitalRead(PUSH));
        delay(400);
        // attachInterrupt(GATE, setBLUE_ON, CHANGE);
        WTF=HIGH;

// process functions

        switch (function)
        {
        case 0: // Allophones
        {

                prevAllo=9999; //to force display at startup

                do {
                        fin=0; // reset long presss counter

                        voice.mode=digitalRead(SW0)+digitalRead(SW1)*2;
                        allo=map(potRead(6),4095,10,0,124);


                        if (allo!=prevAllo)
                        {
                                prevAllo=allo;
                                display.clearDisplay();
                                display.setFont(&Orbitron_Light_22);
                                display.setTextSize(1);
                                display.setCursor(0,16);
                                display.print(allo+1);
                                display.setFont(&Orbitron_Light_24);
                                display.setCursor(0,40);
                                display.setTextSize(1);
                                display.print(alloL[allo]);
                                display.display();
                        }


                        //  if (digitalRead(GATE)==ON || digitalRead(PUSH)==ON)

                        if (voice.mode==MODE_SPEECH)

                        {
                                if (digitalRead(GATE)==ON || digitalRead(PUSH)==ON)
                                {

                                        digitalWrite(RED_LED,ON);
                                        voice.say(alphons[allo]);
                                        digitalWrite(RED_LED,OFF);
                                }
                        }
                        else
                        {
                                voice.say(alphons[allo]);
                        }



                        do {
                                fin++;

                                //  Serial.println(fin);
                                if (fin>100000L) break;
                        } while(digitalRead(PUSH)==0);

                } while(fin<100000L); // long presss
                display.clearDisplay();
                display.display();

        }
        break;



        case 1: // Number
        {

                prevAllo=9999; //to force display at startup

                do {
                        fin=0; // reset long presss counter

                        voice.mode=digitalRead(SW0)+digitalRead(SW1)*2;
                        allo=map(potRead(6),4095,10,0,9);


                        if (allo!=prevAllo)
                        {
                                prevAllo=allo;
                                display.clearDisplay();
                                display.setFont(&Orbitron_Light_22);
                                display.setTextSize(1);
                                display.setCursor(0,16);
                                display.print(allo);
                                display.setFont(&Orbitron_Light_24);
                                display.setCursor(0,40);
                                display.setTextSize(1);
                                display.print(allo);
                                display.display();
                        }


                        //  if (digitalRead(GATE)==ON || digitalRead(PUSH)==ON)

                        if (voice.mode==MODE_SPEECH)

                        {
                                if (digitalRead(GATE)==ON || digitalRead(PUSH)==ON)
                                {

                                        digitalWrite(RED_LED,ON);
                                        sayNumber(allo);
                                        digitalWrite(RED_LED,OFF);
                                }
                        }
                        else
                        {
                                sayNumber(allo);
                        }



                        do {
                                fin++;

                                //  Serial.println(fin);
                                if (fin>100000L) break;
                        } while(digitalRead(PUSH)==0);

                } while(fin<100000L); // long presss
                display.clearDisplay();
                display.display();

        }
        break;

        case 2: // SD LPC
        {
                interruptCount=0;
                WTF=LOW;

                //delay(400);



                do {
                        // choose the file
                        interruptCount=constrain(interruptCount,0,fileNumber-1);
                        filePointer=interruptCount;
                        //  Serial.println(filePointer);
                        displayFilesList(filePointer);
                        delay(100);
                }
                while(digitalRead(PUSH)); // ok when have a file
                display.clearDisplay();
                display.setCursor(0,12);
                Serial.print("Reading file:");
                display.print("Reading file:");
                display.setCursor(0,28); //
                display.print(myFiles[filePointer]);
                Serial.println(myFiles[filePointer]);
                display.display();
                String str=myFiles[filePointer];
                str.remove(str.length()-4);


                //Serial.println(str);

                File myfile = SD.open(myFiles[filePointer]);
                pointer=0;
                linePointer=0;
                part2=0;

                for (int i = 0; i < 20; i++) {
                        lineLabel[i]="";
                }


                if (myfile)
                {
                        while (myfile.available())
                        {
                                inputChar = myfile.read();





                                switch (inputChar)
                                {
                                  case 44:
                                  {

                                    inputChar = myfile.read(); // readnext
                                    Serial.print("/");
                                  }
                                  break;
                                case 58:
                                {
                                        part2=1;

                                        inputChar = myfile.read();
                                }
                                break;

                                case 10:
                                {
                                        part2=0;
                                        linePointer++;
                                        pointer=0;
                                        Serial.print("[");
                                        Serial.print(linePointer);
                                        Serial.println("] ");
                                }
                                break;

                                default:
                                {

                                }
                                break;


                                }

                                if (part2==0 &&inputChar!=10)
                                {
                                        //  Serial.print(inputChar);
                                        //  Serial.print(pointer);
                                        lineLabel[linePointer]=lineLabel[linePointer]+inputChar;


                                }

                                if (part2 &&inputChar!=10)
                                {
                                      msb=int(inputChar);
                                      if (msb < 64)
                                      {
                                        msb=msb-48;
                                      }
                                      else
                                      {
                                        msb=msb-87;// assume we have lower case
                                      }
                                    //  Serial.print(msb);

                                        inputChar = myfile.read();

                                        lsb=int(inputChar);
                                        if (lsb < 64)
                                        {
                                          lsb=lsb-48;
                                        }
                                        else
                                        {
                                          lsb=lsb-87;// assume we have lower case
                                        }

                                        Serial.print(msb*16+lsb);




                                      //  Serial.print(inputChar);
                                        Serial.print(".");
                                        Serial.print(pointer);
                                        stream[linePointer][pointer]=msb*16+lsb;

                                      pointer++;
                                }


                                //  delay(10);
                        }

                        myfile.close();
                        Serial.println();
                        showArray(); // to proof keep it

                        attachInterrupt(GATE, setBLUE_ON, CHANGE);
                        WTF=HIGH;

                        do { /// show A1



                                // readline into array to pass to Synthe


                                do {
                                        if(!digitalRead(PUSH)) break;
                                        //    readLine();

                                }
                                while(digitalRead(GATE)==ON);

                                do {
                                        if(!digitalRead(PUSH)) break;
                                        readLine();

                                }
                                while(digitalRead(GATE)==OFF);
                                readLine();



                        }

                        while(digitalRead(PUSH));
                        detachInterrupt(GATE);
                        WTF=LOW;
                        digitalWrite(BLUE_LED,HIGH);
                        delay(200);



                }
                // if the file cannot be opened give error report
                else {
                        Serial.println("error opening the text file");
                        // add error message on OLED too
                }





        }
        break;






        }



}



/*
   ██████  ██████   ██████  ██
   ██   ██ ██   ██ ██    ██ ██
   ██████  ██████  ██    ██ ██
   ██   ██ ██   ██ ██    ██ ██
   ██████  ██   ██  ██████  ███████
 */

/*
      //  if (GATE_HIGH)
      //  {
                switch (voice.mode)
                {
                case MODE_SPEECH:

                        {
                        if (!voice.talking() && busy==0)
                        {
                                if(!busy) voice.say(alphons[allo]);
                                busy=1;
                        }

                        break;
                      }

                case MODE_REPEAT:
                        voice.say(alphons[allo],false);
                        break;


                }



                case 1: // New function
                {

                }
                break;



 */
//}
//  else
//{
//        busy=0; // si mode speech
//        //  if (MODE_REPEAT) voice.say(sp_alphon127);// si mode 3 stop sound
//  }
