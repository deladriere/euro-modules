#include <SPI.h>
#include <SD.h> // 868
#include <string.h>
#include "S1V30120_defines.h"
#include "text_to_speech_img.h"

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#include "SAMD_AnalogCorrection.h"
#include "avdweb_AnalogReadFast.h"

/*

   ██   ██  █████  ██████  ██████  ██     ██  █████  ██████  ███████
   ██   ██ ██   ██ ██   ██ ██   ██ ██     ██ ██   ██ ██   ██ ██
   ███████ ███████ ██████  ██   ██ ██  █  ██ ███████ ██████  █████
   ██   ██ ██   ██ ██   ██ ██   ██ ██ ███ ██ ██   ██ ██   ██ ██
   ██   ██ ██   ██ ██   ██ ██████   ███ ███  ██   ██ ██   ██ ███████

 */


#define S1V30120_RST    4
#define S1V30120_RDY    11
#define S1V30120_CS     5
#define S1V30120_MUTE   9

#define BUSY 38
#define SW0 0
#define SW1 1
#define GATE 2
#define PUSH 3
#define ROTA 6
#define ROTB 7
#define GREEN_LED  12
#define RED_LED 13

#define P_VOICE A1
#define P_RATE A2
#define P_AVPITCH A3
#define P_PITCHR A4
#define P_BREATH A5



/*

   ██    ██  █████  ██████  ██  █████  ██████  ██      ███████ ███████
   ██    ██ ██   ██ ██   ██ ██ ██   ██ ██   ██ ██      ██      ██
   ██    ██ ███████ ██████  ██ ███████ ██████  ██      █████   ███████
    ██  ██  ██   ██ ██   ██ ██ ██   ██ ██   ██ ██      ██           ██
     ████   ██   ██ ██   ██ ██ ██   ██ ██████  ███████ ███████ ███████

 */



#define DEBUG_TRACE
#define VERSION "Ver. 0.1"


#define MODE_AUTO 1
#define MODE_TRIG 2
#define MODE_GATE 3

#define READY 1

char inputChar;
char* myFiles[20];  // max 20 files
char song[60][120] {}; // max 60 caracter per lines // 100 lines max
int fileCounter=0; // file index
int fileNumber=0; // file number
int pointer=0;
int linePointer=0;
int ligne=0;

char serialtext[60];

uint8_t mode=0;

int moy;

bool pressed;
bool triggered;

int filePointer=0;
char fileName[13];

char* filetype[4] = {".txt", ".sng", ".tts"};

char buf[16]; // for conversion to String

String mytext = "Success! Look at me, I can speak. I'm the best!";

// Variables
// Most received messages are 6 bytes
char rcvd_msg[20] = {0};

// Educated guess
// “Over the whole document, make the average sentence length
// 15-20 words, 25-33 syllables and 75-100 characters.”
// https://strainindex.wordpress.com/2008/07/28/the-average-sentence-length/
static volatile char send_msg[400] = {0}; // was 200
static volatile unsigned short msg_len;
static volatile unsigned short txt_len;

unsigned short tmp;
long idx;
bool success;


#define ON 0
#define OFF 1


bool rd=0; // rotary display

volatile int interruptCount=0; // The rotary counter
volatile bool rotF; // because use in rot

bool WTF; // to avoid led interrupt trigger by rotary !

char* mainFunctions[12]={

        "Numbers",
        "SD TTS",
        "Singing",
        "Clock","","",""
};


#define numFunctions 4
int function;
int fin =0; /// pour pression longue

int allo;
int prevAllo;


// Used to download image data. This is changed by the
// This is why is declares as static volatile.
// Note: unsigned short is max 32767, while our image data is 31208 in length
// one must change this to unsigned long if future image data becomes larger
static volatile unsigned short TTS_DATA_IDX;

int potVoice=0;
String voice;

/*

   ██████  ██████       ██ ███████  ██████ ████████ ███████
   ██    ██ ██   ██      ██ ██      ██         ██    ██
   ██    ██ ██████       ██ █████   ██         ██    ███████
   ██    ██ ██   ██ ██   ██ ██      ██         ██         ██
   ██████  ██████   █████  ███████  ██████    ██    ███████

 */
SSD1306AsciiWire display;
File root;


/*
   ███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████
   ██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██
   █████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████
   ██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
   ██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████
 */
//<start>[MK]::LogEnhancement v0.0.1 toggle 'Sprintln()' and 'Serial.print()'
//       on/off with preprocessor variable 'DEBUG_TRACE' (https://forum.arduino.cc/index.php?topic=46900.0)

     #ifdef DEBUG_TRACE
     #define Sprintln(MSG) Serial.println(MSG)
     #define Sprint(MSG)   Serial.print(MSG)
     #else
     #define Sprintln(MSG)
     #define Sprint(MSG)
     #endif
//<end>[MK]::LogEnhancement

int potReadFast(int pot,int readings)
{
        moy=0;
        for (int i = 0; i < readings; i++) {
                moy=moy + analogReadFast(pot);
                //  moy=moy +analogReadFast(pot,2);
        }
        return moy/readings;

}

void readLine()
{

        ligne=map(potReadFast(A6,10),0,4095,linePointer-1,0);
        if (ligne!=prevAllo)
        {
                if (READY) {
                        Wire.setClock(2500000L);
                }
                //display.setRow(2);
                //display.clearToEOL();
                //display.println(ligne+1);
                display.setRow(4);
                display.set2X();
                display.clearToEOL();
                //  display.println(lineLabel[ligne]);
                //Sprintln(lineLabel[ligne]); // for show


                // erase
                memset( &serialtext, 0, 60 );
                for (int p=0; p<60; p++)

                {
                        if (song[ligne][p] !=10)
                        {
                                //  Serial.write(song[ligne][p]);
                                display.print(song[ligne][p]);
                                serialtext[p]=song[ligne][p];


                        }
                        else
                        {
                                //Serial.write(10);
                                //  Sprintln("");
                                display.println("");
                                break;
                        }
                }


                display.set1X();
                Wire.setClock(200000L);
                prevAllo=ligne;
        }
}

void showArray()
{

        for (int l=0; l<linePointer; l++)
        {
                for (int p=0; p<40; p++)
                {
                        if (song[l][p] !=10)
                        {
                                Serial.write(song[l][p]);
                        }
                        else
                        {
                                //Serial.write(10);
                                Sprintln("");
                                break;
                        }
                }

        }
}

void displayFilesList(int p)
{

        for (int i = 0; i<4; i++) {

                display.setCursor(13,i*8);   //
                String str2=myFiles[i+p];
                str2.remove(str2.length()-4);
                display.clearToEOL();
                display.println(str2);
        }

}


bool isTxtFile(char* filename, int type) {
        bool result;
        Sprint(type);
        Sprint(" ");
        Sprintln(filetype[type]);
        if(strstr(strlwr(filename),filetype[type]) && !strstr(strlwr(filename),"_") )   // filtering out just ".txt" file not containing "_"
        { result=true; }
        else
        { result= false; }
        return result;
}


void GetFilesList(File dir,int type) {
        fileNumber=0;
        fileCounter=0;
        for (int8_t i = 0; i < 20; i++) {
                myFiles[i]="";  // to emty the list
        }

        while(true) {


                File entry =  dir.openNextFile();  // was dir
                if (!entry) {
                        // no more files
                        break;
                }

                if(isTxtFile(entry.name(),type))   //check if it's a .txt file
                {

                        String str=entry.name();
                        //  str.remove(str.length()-4);


                        // Length (with one extra character for the null terminator)
                        int str_len = str.length() + 1;

                        // Prepare the character array (the buffer)
                        char char_array[str_len];

                        // Copy it over
                        str.toCharArray(char_array, str_len);
                        //Sprintln(char_array);

                        myFiles[fileCounter]=strdup(char_array);   //WTF is strdup ?
                        fileCounter++;

                }
                entry.close();


        }
        fileNumber=fileCounter;

}


void displayFunctionList(int p)
{

        for (int i = 0; i<4; i++) {   // nbr of lines to display

                display.setCursor(13,i*8);   //
                String str2=mainFunctions[i+p];
                display.clearToEOL();
                display.println(str2);
        }


}

void rot()
{



        if(digitalRead(ROTA))
        {
                if(digitalRead(ROTB))
                {
                        interruptCount--;
                }
                else
                {
                        interruptCount++;
                }
                rotF=1;
        }




}   // Rot function


void printDirectory(File dir, int numTabs) {
        while (true) {

                File entry =  dir.openNextFile();
                if (!entry) {
                        // no more files
                        break;
                }
                for (uint8_t i = 0; i < numTabs; i++) {
                        Serial.print('\t');
                }
                Serial.print(entry.name());
                if (entry.isDirectory()) {
                        Serial.println("/");
                        printDirectory(entry, numTabs + 1);
                } else {
                        // files have sizes, directories do not
                        Serial.print("\t\t");
                        Serial.println(entry.size(), DEC);
                }
                entry.close();
        }
}

void initSD()
{


        if (!SD.begin(10)) {
                display.clear();
                display.println("Error");
                display.println("SD card");
                display.println("failed to ");
                display.println("initialize");
                while(1);
        }
        root = SD.open("/");

}


void splashScreen()
{
        display.clear();
        display.setFont(fixed_bold10x15);
        // display.set2X();
        display.setRow(0);

        display.println("Polaxis");

        delay(500);


        display.setRow(2);
        display.println("VAX");
        display.setRow(4);
        display.println(VERSION);
        display.setRow(6);
        display.println(analogRead(A6));

        //delay(1500);
        //

}




// This function resets the S1V30120 chip and loads the firmware code
void S1V30120_reset(void)
{
        digitalWrite(S1V30120_CS,HIGH); // S1V30120 not selected
        digitalWrite(S1V30120_RST,LOW);
        // send one dummy byte, this will leave the clock line high
        SPI.beginTransaction(SPISettings(750000, MSBFIRST, SPI_MODE3));
        SPI.transfer(0x00);
        SPI.endTransaction();
        delay(5);
        digitalWrite(S1V30120_RST,HIGH);
        delay(150);
}

unsigned short S1V30120_get_version(void)
{
        // Querry version
        unsigned short S1V30120_version = 0;
        unsigned short tmp_disp;
        // Sending ISC_VERSION_REQ = [0x00, 0x04, 0x00, 0x05];
        char msg_ver[] = {0x04, 0x00, 0x05, 0x00};
        S1V30120_send_message(msg_ver, 0x04);

        //wait for ready signal
        while(digitalRead(S1V30120_RDY) == 0);

        // receive 20 bytes
        digitalWrite(S1V30120_CS,LOW);
        SPI.beginTransaction(SPISettings(750000, MSBFIRST, SPI_MODE3));
        // wait for message start
        while(SPI.transfer(0x00) != 0xAA);
        for (int i = 0; i < 20; i++)
        {
                rcvd_msg[i] = SPI.transfer(0x00);
        }
        // Send 16 bytes padding
        S1V30120_send_padding(16);
        SPI.endTransaction();
        digitalWrite(S1V30120_CS,HIGH);
        S1V30120_version = rcvd_msg[4] << 8 | rcvd_msg[5];
        Serial.print("HW version ");
        Serial.print(rcvd_msg[4],HEX);
        Serial.print(".");
        Serial.println(rcvd_msg[5],HEX);
        Serial.print("Firmware version ");
        Serial.print(rcvd_msg[6],HEX);
        Serial.print(".");
        Serial.print(rcvd_msg[7],HEX);
        Serial.print(".");
        Serial.println(rcvd_msg[16],HEX);
        Serial.print("Firmware features ");
        Serial.println(((rcvd_msg[11] << 24) | (rcvd_msg[10] << 16) | (rcvd_msg[9] << 8) | rcvd_msg[8]),HEX);
        Serial.print("Firmware extended features ");
        Serial.println(((rcvd_msg[15] << 24) | (rcvd_msg[14] << 16) | (rcvd_msg[13] << 8) | rcvd_msg[12]),HEX);
        return S1V30120_version;
}

bool S1V30120_download(void)
{
        // TTS_INIT_DATA is of unsigned char type (one byte)
        unsigned short len = sizeof (TTS_INIT_DATA);
        unsigned short fullchunks;
        unsigned short remaining;
        bool chunk_result;

        //long data_index = 0;

        Serial.print("TTS_INIT_DATA length is ");
        Serial.println(len);
        // We are loading chunks of data
        // Each chunk, including header must be of maximum 2048 bytes
        // as the header is 4 bytes, this leaves 2044 bytes to load each time
        // Computing number of chunks
        fullchunks = len / 2044;
        remaining = len - fullchunks * 2044;
        Serial.print("Full chunks to load: ");
        Serial.println(fullchunks);
        Serial.print("Remaining bytes: ");
        Serial.println(remaining);
        // Load a chunk of data
        for (int num_chunks = 0; num_chunks < fullchunks; num_chunks++)
        {
                chunk_result = S1V30120_load_chunk (2044);
                if (chunk_result)
                {
                        Serial.println("Success");
                }
                else
                {
                        Serial.print("Failed at chunk ");
                        Serial.println(num_chunks);
                        return 0;
                }
        }
        // Now load the last chunk of data
        chunk_result = S1V30120_load_chunk (remaining);
        if (chunk_result)
        {
                Serial.println("Success");
        }
        else
        {
                Serial.print("Failed at last chunk ");
                return 0;
        }
// All was OK, returning 1
        return 1;
}

bool S1V30120_load_chunk(unsigned short chunk_len)
{
        // Load a chunk of data
        char len_msb = ((chunk_len + 4) & 0xFF00) >> 8;
        char len_lsb = (chunk_len + 4) & 0xFF;
        digitalWrite(S1V30120_CS,LOW);
        SPI.beginTransaction(SPISettings(750000, MSBFIRST, SPI_MODE3));
        SPI.transfer(0xAA); // Start Message Command
        SPI.transfer(len_lsb); // Message length is 2048 bytes = 0x0800
        SPI.transfer(len_msb); // LSB first
        SPI.transfer(0x00); // Send SC_BOOT_LOAD_REQ (0x1000)
        SPI.transfer(0x10);
        for (int chunk_idx = 0; chunk_idx < chunk_len; chunk_idx++)
        {
                SPI.transfer(TTS_INIT_DATA[TTS_DATA_IDX]);
                TTS_DATA_IDX++;
        }
        SPI.endTransaction();
        digitalWrite(S1V30120_CS,HIGH);
        return S1V30120_parse_response(ISC_BOOT_LOAD_RESP, 0x0001, 16);
}

bool S1V30120_boot_run(void)
{
        char boot_run_msg[] = {0x04, 0x00, 0x02, 0x10};
        S1V30120_send_message(boot_run_msg, 0x04);
        return S1V30120_parse_response(ISC_BOOT_RUN_RESP, 0x0001, 8);
}

void show_response(bool response)
{
        if(response)
                Serial.println("OK!");
        else
        {
                Serial.println("Failed. System halted!");
                while(1);
        }
}

bool S1V30120_registration(void)
{
        SPI.beginTransaction(SPISettings(750000, MSBFIRST, SPI_MODE3));
        char reg_code[] = {0x0C, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        S1V30120_send_message(reg_code, 0x0C);
        return S1V30120_parse_response(ISC_TEST_RESP, 0x0000, 16);
}


// Message parser
// This function receives as parameter the expected response code and result
// And returns 1 if the expected result is received, 0 otherwise
// As an observation, most messages are 6 bytes in length
// (2 bytes length + 2 bytes response code + 2 bytes response)
bool S1V30120_parse_response(unsigned short expected_message, unsigned short expected_result, unsigned short padding_bytes)
{
        unsigned short rcvd_tmp;

        //wait for ready signal
        while(digitalRead(S1V30120_RDY) == 0);

        // receive 6 bytes
        digitalWrite(S1V30120_CS,LOW);
        SPI.beginTransaction(SPISettings(750000, MSBFIRST, SPI_MODE3));
        // wait for message start
        while(SPI.transfer(0x00) != 0xAA);
        for (int i = 0; i < 6; i++)
        {
                rcvd_msg[i] = SPI.transfer(0x00);
        }
        // padding bytes
        S1V30120_send_padding(padding_bytes);
        SPI.endTransaction();
        digitalWrite(S1V30120_CS,HIGH);
        // Are we successfull? We shall check
        rcvd_tmp = rcvd_msg[3] << 8 | rcvd_msg[2];
        if (rcvd_tmp == expected_message) // Have we received ISC_BOOT_RUN_RESP?
        {
                // We check the response
                rcvd_tmp = rcvd_msg[5] << 8 | rcvd_msg[4];
                if (rcvd_tmp == expected_result) // success, return 1
                        return 1;
                else
                        return 0;
        }
        else // We received something else
                return 0;
}

// Padding function
// Sends a num_padding_bytes over the SPI bus
void S1V30120_send_padding(unsigned short num_padding_bytes)
{
        for (int i = 0; i < num_padding_bytes; i++)
        {
                SPI.transfer(0x00);
        }
}

// Functions that run in normal mode

void S1V30120_send_message(volatile char message[], unsigned char message_length)
{

        // Check to see if there's an incoming response or indication
        while(digitalRead(S1V30120_RDY) == 1);  // blocking

        // OK, we can proceed
        digitalWrite(S1V30120_CS,LOW);
        SPI.beginTransaction(SPISettings(750000, MSBFIRST, SPI_MODE3));
        SPI.transfer(0xAA); // Start Message Command
        for (int i = 0; i < message_length; i++)
        {
                SPI.transfer(message[i]);
        }
        SPI.endTransaction();
}

bool S1V30120_configure_audio(void)
{
        msg_len = 0x0C;
        send_msg[0] = msg_len & 0xFF;    // LSB of msg len
        send_msg[1] = (msg_len & 0xFF00) >> 8; // MSB of msg len
        send_msg[2] = ISC_AUDIO_CONFIG_REQ & 0xFF;
        send_msg[3] = (ISC_AUDIO_CONFIG_REQ & 0xFF00) >> 8;
        send_msg[4] = TTS_AUDIO_CONF_AS;
        send_msg[5] = TTS_AUDIO_CONF_AG;
        send_msg[6] = TTS_AUDIO_CONF_AMP;
        send_msg[7] = TTS_AUDIO_CONF_ASR;
        send_msg[8] = TTS_AUDIO_CONF_AR;
        send_msg[9] = TTS_AUDIO_CONF_ATC;
        send_msg[10] = TTS_AUDIO_CONF_ACS;
        send_msg[11] = TTS_AUDIO_CONF_DC;
        S1V30120_send_message(send_msg, msg_len);
        return S1V30120_parse_response(ISC_AUDIO_CONFIG_RESP, 0x0000, 16);
}

// set gain to 0 db
bool S1V30120_set_volume(void)
{

        int16_t gain = -15;
        uint8_t tmp_gain[ 2 ] = { 0 };
        tmp_gain[ 0 ] = gain & 0x00FF;
        tmp_gain[ 1 ] = ( gain & 0xFF00 ) >> 8;

        char setvol_code[]={0x06, 0x00, 0x0A, 0x00, tmp_gain[ 0 ],tmp_gain[ 1 ]}; // was 0/0 EC FF / F6 FF
        S1V30120_send_message(setvol_code, 0x06);
        return S1V30120_parse_response(ISC_AUDIO_VOLUME_RESP, 0x0000, 16);
}

bool S1V30120_configure_tts(void)
{
        msg_len = 0x0C;
        send_msg[0] = msg_len & 0xFF;    // LSB of msg len
        send_msg[1] = (msg_len & 0xFF00) >> 8; // MSB of msg len
        send_msg[2] = ISC_TTS_CONFIG_REQ & 0xFF;
        send_msg[3] = (ISC_TTS_CONFIG_REQ & 0xFF00) >> 8;
        send_msg[4] = ISC_TTS_SAMPLE_RATE;
        send_msg[5] = ISC_TTS_VOICE;
        send_msg[6] = ISC_TTS_EPSON_PARSE;
        send_msg[7] = ISC_TTS_LANGUAGE;
        send_msg[8] = ISC_TTS_SPEAK_RATE_LSB;
        send_msg[9] = ISC_TTS_SPEAK_RATE_MSB;
        send_msg[10] = ISC_TTS_DATASOURCE;
        send_msg[11] = 0x00;
        S1V30120_send_message(send_msg, msg_len);
        return S1V30120_parse_response(ISC_TTS_CONFIG_RESP, 0x0000, 16);
}

// bool S1V30120_speech(void)
bool S1V30120_speech(String text_to_speech, unsigned char flush_enable)
{

        bool response;
        txt_len = text_to_speech.length();
        msg_len = txt_len + 6;

        send_msg[0] = msg_len & 0xFF;    // LSB of msg len
        send_msg[1] = (msg_len & 0xFF00) >> 8; // MSB of msg len
        send_msg[2] = ISC_TTS_SPEAK_REQ & 0xFF;
        send_msg[3] = (ISC_TTS_SPEAK_REQ & 0xFF00) >> 8;
        send_msg[4] = flush_enable; // flush control
        for (int i = 0; i < txt_len; i++)
        {
                send_msg[i+5] = text_to_speech[i];
                Serial.print(text_to_speech[i]);
        }
        Serial.println();
        send_msg[msg_len-1] = '\0'; // null character

        S1V30120_send_message(send_msg, msg_len);

        response = S1V30120_parse_response(ISC_TTS_SPEAK_RESP, 0x0000, 16);

        if (send_msg[4] == 0)
        {
        //while (!S1V30120_parse_response(ISC_TTS_FINISHED_IND, 0x0000, 16));  // blocking
        }

        return response;
}

/*
   ███████ ███████ ████████ ██    ██ ██████
   ██      ██         ██    ██    ██ ██   ██
   ███████ █████      ██    ██    ██ ██████
     ██ ██         ██    ██    ██ ██
   ███████ ███████    ██     ██████  ██
 */


void setup() {

        Wire.begin();


        //Pin settings
        pinMode(S1V30120_RST, OUTPUT);
        pinMode(S1V30120_RDY, INPUT);
        pinMode(S1V30120_CS, OUTPUT);
        pinMode(S1V30120_MUTE, OUTPUT);

        pinMode(SW0, INPUT_PULLUP);
        pinMode(SW1, INPUT_PULLUP);
        pinMode(ROTA, INPUT_PULLUP);
        pinMode(ROTB, INPUT_PULLUP);
        pinMode(PUSH, INPUT_PULLUP);
        pinMode(GATE, INPUT_PULLUP);
        pinMode(GREEN_LED,OUTPUT);
        pinMode(RED_LED,OUTPUT);
        pinMode(BUSY,OUTPUT);
        pinMode(10,OUTPUT);

        // turn transistorized outputs low ;
        digitalWrite(GREEN_LED,HIGH);
        digitalWrite(RED_LED,HIGH);

        attachInterrupt(ROTA, rot, CHANGE);

        analogReadResolution(12);
        analogReadCorrection(14, 2064);


        // Unmute
        digitalWrite(S1V30120_MUTE,LOW);

        // for debugging
        Serial.begin(9600);
        //while (!Serial);

        display.begin(&Adafruit128x64, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)


        splashScreen();

        //initSD();
        //printDirectory(root, 0);

        SPI.begin();

        S1V30120_reset();
        S1V30120_reset();



        tmp = S1V30120_get_version();
        if (tmp == 0x0402)
        {
                Serial.println("S1V30120 found. Downloading boot image!");
        }
        success = S1V30120_download();
        Serial.print("Boot image download: ");
        show_response(success);
        success = S1V30120_boot_run();
        Serial.print("Boot image run: ");
        show_response(success);
        delay(150); // Wait for the boot image to execute
        Serial.print("Registering: ");
        success = S1V30120_registration();
        show_response(success);
        // Once again print version information
        S1V30120_get_version();
        success = S1V30120_configure_audio();
        Serial.print("Configuring audio: ");
        show_response(success);
        success = S1V30120_set_volume();
        Serial.print("Setting volume: ");
        show_response(success);

        success = S1V30120_configure_tts();
        Serial.print("Configure TTS: ");
        show_response(success);



        //success = S1V30120_speech(mytext,0);
        //Serial.print("Speaking1: ");
        //show_response(success);
        //delay(250);

        //Serial.print("Speaking2: ");
        //success = S1V30120_speech("ok",0);
        //delay(250);

        //show_response(success);

        //S1V30120_speech("hello",0);
        //Serial.print("Speaking3: ");
        //show_response(success);

        S1V30120_speech("[:n9][:ra 20][:dv ap 50 pr 0] Welcome to the machine",0);
        delay(1000);
        S1V30120_speech("[:n3] we are the robots",0);
        delay(1000);
        //S1V30120_speech("[:dv ap 100 pr 0][:rate 75][WIY<500,0>_<100>AA<600,0>R<10>_<100>DH<50>AH<50,0> R OW<200,0> B AA<200,0> T S ] ",0);
        //  S1V30120_speech("[:dv ap 100 pr 0][:rate 600][WIY<500,0>_<100>AA<600,0>R<10>_<100>DH<50>AH<50,0> R OW<200,0> B AA<200,0> T S ] ",0);

        //  S1V30120_speech("[:n3][WIY<500,0>_<100>AA<600,0>R<10>_<100>DH<50>AH<50,0> R OW<200,0> B AA<200,0> T S ] ",0);
        //S1V30120_speech("[:n0][WIY<500,0>_<100>AA<600,0>R<10>_<100>DH<50>AH<50,0> R OW<200,0> B AA<200,0> T S ] ",0);


        //  digitalWrite(S1V30120_RST,LOW);
        //  delay(1000);
        //initSD();
        //root.rewindDirectory();
        //  printDirectory(root, 0);
        //  digitalWrite(10,HIGH);
        //digitalWrite(S1V30120_CS,HIGH);
        //  delay(100);


        // put your main code here, to run repeatedly:




}


/*
   ██       ██████   ██████  ██████
   ██      ██    ██ ██    ██ ██   ██
   ██      ██    ██ ██    ██ ██████
   ██      ██    ██ ██    ██ ██
   ███████  ██████   ██████  ██
 */


void loop() {





//S1V30120_speech("[hxae<300,10>piy<300,10> brr<600,12>th<100>dey<600,10> tuw<600,15> yu<1200,14>_<120>][hxae<300,10>piy<300,10> brr<600,12>th<100>dey<600,10> tuw<600,17> yu<1200,15>_<120>] ",0);
//S1V30120_speech("[hxae<300,10>piy<300,10>brr<600,22>th<100>dey<600,19>dih<600,15>rdeh<600,14>ktao<600,12>k_<120>_<120>][hxae<300,20>piy<300,20> brr<600,19>th<100>dey<600,15> tuw<600,17> yu<1200,15>] ",0);
//S1V30120_speech("[_<50,22>dey<400,22>ziy<400,19>dey<400,15>ziy<400,10>gih<200,12>vmiy<200,14>yurr<200,15>ae<400,12>nsax<200,15>rduw<400,10>] ",0);
        //  S1V30120_speech("[ay<400,17>mhxae<400,22> kray<400,19> ziy<400,15>ao<200,12>lfao<200,14>rdhax<200,15>lah<400,17>vao<200,19>vyu<400,17>ih<200,19>twow<200,20>ntbiy<200,19>ax<200,17>stay<400,22>] ",0);


        //Wire.setClock(2500000L);
        delay(500);
        interruptCount=0;
        display.clear();
        display.set1X();
        display.setFont(fixed_bold10x15);
        display.setCursor(0,12);
        display.println(">");
        rotF=1;


        do {


                if (rotF)
                {
                        interruptCount=constrain(interruptCount,0,numFunctions-1);
                        function=interruptCount;
                        displayFunctionList(function);
                        rotF=0;
                }


        } while(digitalRead(PUSH));
        delay(400);
        // attachInterrupt(GATE, setBLUE_ON, CHANGE);
        WTF=HIGH;

        switch (function)

        {
        case 0:
        {
                display.clear();
                display.println("Random");

                //digitalWrite(10,LOW);
                //SPI.transfer(0XFF);
                //digitalWrite(10,HIGH);
                //SPI.transfer(0xAA);
                SPI.setDataMode(SPI_MODE3);
                //(https://forum.arduino.cc/index.php?topic=276274.0)


                do {
                        fin=0;
                        while(digitalRead(GATE)==OFF && digitalRead(PUSH)==OFF);

                        allo=map(analogRead(A6),0,4095,99,0);
                        potVoice=map(analogRead(A1),0,4095,8,0);
                        allo=random(999);

                        itoa(allo,buf,10);
                        //itoa(potVoice,voice,10);
                        mytext="[:n" + String(potVoice) + "]";
                        mytext=mytext+ "[:ra "+String(map(analogRead(P_RATE),0,4095,600,75))+"]";
                        mytext=mytext+ "[:dv ap "+String(map(analogRead(P_AVPITCH),0,4095,400,10))+" pr "+String(map(analogRead(P_PITCHR),0,4095,100,0))+" br "+String(map(analogRead(P_BREATH),0,4095,100,0))+ "] ";

                        mytext=mytext +buf+ " ";
                        display.setRow(2);
                        display.clearToEOL();
                        display.println(allo);
                        Sprintln(mytext);
                        S1V30120_speech(mytext,0);
                        //S1V30120_speech(buf,0);






                        while(digitalRead(GATE)==ON);





                        do {
                                fin++;

                                //  Sprintln(fin);
                                if (fin>100000L) break;
                        } while(digitalRead(PUSH)==0);

                } while(fin<100000L);       // long presss


        }
        break;

        case 1: // SD TTS
        {
                //display.clear();
                //display.println(mainFunctions[function]);
                SPI.setDataMode(SPI_MODE3);
                initSD();
                root.rewindDirectory();
                GetFilesList(root,0);
                interruptCount=0;
                WTF=LOW;
                rotF=1;

                do {
                        // choose the file
                        if (rotF)
                        {
                                interruptCount=constrain(interruptCount,0,fileNumber-1);
                                filePointer=interruptCount;
                                displayFilesList(filePointer);
                                rotF=0;
                        }
                }
                while(digitalRead(PUSH));
                delay(400); // ok when have a file


                display.clear();
                display.setCursor(0,12);
                Sprint("Reading file:");
                display.println("Reading file:");
                display.setCursor(0,28); //
                display.print(myFiles[filePointer]);
                Sprintln(myFiles[filePointer]);

                String str=myFiles[filePointer];
                str.remove(str.length()-4);


                Sprintln(str);

                File myfile = SD.open(myFiles[filePointer]);
                pointer=0;
                linePointer=0;
                if (myfile)
                {
                        while (myfile.available())
                        {
                                inputChar = myfile.read();

                                if (inputChar != 10) // define breaking char here (\n isnt working for some reason, i will follow this up later)
                                {
                                        song[linePointer][pointer]= inputChar;
                                        //  Serial.write(inputChar);
                                        pointer++;
                                        //  Sprint(pointer);

                                }
                                else
                                {


                                        song[linePointer][pointer]= inputChar;
                                        //  Serial.write(inputChar);
                                        pointer=0;
                                        linePointer++;


                                }

                                //  delay(10);
                        }

                        myfile.close();

                        showArray(); // to proof keep it

                        // initialize selection
                        SPI.setDataMode(SPI_MODE3);
                        S1V30120_speech("ready amigo",0);
                        display.clear();
                        display.println(str);
                        prevAllo=-1;// force first display










                        do {
                                fin=0;
                                mode=digitalRead(SW0)+digitalRead(SW1)*2;
                                potVoice=map(analogRead(A1),0,4095,8,0);
                                display.setRow(2);
                                display.clearToEOL();
                                display.println(potVoice);

                                if ((pressed | digitalRead(PUSH)==ON | digitalRead(GATE)==ON | mode == MODE_AUTO) && READY && !triggered)

                                {
                                        //  prevAllo=-1;
                                        pressed=false; //reset flag
                                        readLine();
                                        mytext="[:n" + String(potVoice) + "]";
                                        mytext=mytext+ "[:ra "+String(map(analogRead(P_RATE),0,4095,600,75))+"]";
                                        mytext=mytext+ "[:dv ap "+String(map(analogRead(P_AVPITCH),0,4095,400,10))+" pr "+String(map(analogRead(P_PITCHR),0,4095,100,0))+" br "+String(map(analogRead(P_BREATH),0,4095,100,0))+ "] ";

                                        //readParam(); // overkill ?
                                        //Sing(serialtext);

                                        mytext=mytext+ " " + serialtext;
                                        Sprintln(mytext);
                                        S1V30120_speech(mytext,0);

                                        if (mode!=MODE_AUTO)
                                        {
                                                triggered=true; // pour eviter de stopper sur un Gate en mode auto
                                        }

                                }

                                if (digitalRead(GATE)==OFF && triggered)
                                {
                                        triggered=false;
                                }

                                if ( digitalRead(GATE)==OFF)
                                {
                                        readLine();
                                }




                                do {
                                        fin++;

                                        //  Sprintln(fin);
                                        if (fin>100000L) break;
                                } while(digitalRead(PUSH)==0);

                        } while(fin<100000L); // long presss
                        display.clear();


                }
                // if the file cannot be opened give error report
                else {
                        Sprintln("error opening the text file");
                        // add error message on OLED too
                }

        }

        break;



        }// end switch





}// end loop



/*
   case 0:
   {
        display.clear();
        display.println("Phon 64");

        do {
                fin=0;


                do {
                        fin++;

                        //  Sprintln(fin);
                        if (fin>100000L) break;
                } while(digitalRead(PUSH)==0);

        } while(fin<100000L);   // long presss


   } // end of case
   break;

 */
