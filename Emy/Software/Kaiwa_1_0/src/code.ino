#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Kfonts.h>
#include "AquesTalk.h"


// https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf

/*
   ██   ██  █████  ██████  ██████  ██     ██  █████  ██████  ███████
   ██   ██ ██   ██ ██   ██ ██   ██ ██     ██ ██   ██ ██   ██ ██
   ███████ ███████ ██████  ██   ██ ██  █  ██ ███████ ██████  █████
   ██   ██ ██   ██ ██   ██ ██   ██ ██ ███ ██ ██   ██ ██   ██ ██
   ██   ██ ██   ██ ██   ██ ██████   ███ ███  ██   ██ ██   ██ ███████
 */




//                        +-----------------+
//                        |[ ]NC      OUT[ ]| Audio
//                        |[ ]RST   !PLAY[ ]|
//                        |[ ]NC       NC[ ]|
//                        |[ ]NC       NC[ ]|
//                        |[ ]NC      SCL[ ]|
//                        |[ ]NC      SDA[ ]|
//                    3V3 |[ ]3V3      5V[ ]| 5V
//                        |[ ]GND     GND[ ]| GND
//                        +________________/




 #define SW0 0
 #define SW1 1
 #define GATE 2
 #define PUSH 3
 #define RST_Microbus 4
 #define CS_Microbus 5
 #define ROTA 6
 #define ROTB 7
// A6 is also D8 on Adafruit Feather
 #define AN_Microbus 9 // A7 can be D9 on Adafruit Feather
 #define CS_SD 10
 #define INT_Microbus 11
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


char* mainFunctions[12]={

        "SD tts",
        "Functio1",
        "USB tts",
        "USB phon","","",""
};

#define numFunctions 4
int function;

char song[60][120] {}; // max 80 caracter per lines // 100 lines max
int pointer=0;
int linePointer=0;
int ligne=0;
int serialPointer=0;
char serialtext[40];

bool WTF;

char inputChar;
char* myFiles[10];  // max 10 files
int fileCounter=0; // file index
int fileNumber=0; // file number
int filePointer=0;
char fileName[13];

String terminal;

static boolean rotating=false;

volatile int interruptCount=0; // The rotary counter



/*
   ██████  ██████       ██ ███████  ██████ ████████ ███████
   ██    ██ ██   ██      ██ ██      ██         ██    ██
   ██    ██ ██████       ██ █████   ██         ██    ███████
   ██    ██ ██   ██ ██   ██ ██      ██         ██         ██
   ██████  ██████   █████  ███████  ██████    ██    ███████
 */

File root;
Adafruit_SSD1306 display(0); // modif library for 64



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
        analogReadResolution(10); //10 or 12 ?

        attachInterrupt(ROTA, rot, CHANGE);


        // Open Serial communications and wait for port to open:
        Serial.begin(115200);
        //Serial.setTimeout(100); // why ?
        //while (!Serial) {
        //          ; // wait for Serial port to connect. Needed for native USB port only
        //  }

        Serial.print("Initializing SD card...");

        if (!SD.begin(10)) {
                Serial.println("initialization failed!");
                return;
        }
        Serial.println("initialization done.");
        display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)


        splashScreen();

//Wire.begin();
        SetSpeed(80);
        delay(10);
        SetPitch(100);
        SetAccent(255);
        delay(10);
        Synthe("konichiwa.");




        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.display();


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
        interruptCount=0;

        do {

                interruptCount=constrain(interruptCount,0,numFunctions-1);
                function=interruptCount;
                displayFunctionList(function);



        } while(digitalRead(PUSH));


        switch (function) {
        case 0:

        {
                interruptCount=0;

                delay(400);



                do {
                        // choose the file
                        interruptCount=constrain(interruptCount,0,fileNumber-1);
                        filePointer=interruptCount;
                        Serial.println(filePointer);
                        displayFilesList(filePointer);
                        //  delay(100);
                }
                while(digitalRead(PUSH)); // ok when have a file


                Serial.print("opening ");
                display.clearDisplay();
                display.setCursor(15,0);
                display.print("Reading");
                display.setCursor(15,20); //
                display.print(myFiles[filePointer]);
                Serial.println(myFiles[filePointer]);
                display.display();


                String str=myFiles[filePointer];
                str.remove(str.length()-4);


                Serial.println(str);

                File myfile = SD.open(myFiles[filePointer]);
                pointer=0;
                linePointer=0;
                if (myfile)
                {
                        while (myfile.available())
                        {
                                inputChar = myfile.read();
                                //  Serial.print("[");
                                //  Serial.print(line);
                                //  Serial.print("] ");
                                if (inputChar != 10) // define breaking char here (\n isnt working for some reason, i will follow this up later)
                                {
                                        song[linePointer][pointer]= inputChar;
                                        //  Serial.write(inputChar);
                                        pointer++;
                                        //  Serial.print(pointer);

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

                        attachInterrupt(GATE, setBLUE_ON, CHANGE);
                        WTF=HIGH;

                        do { /// show A1



                                // readline into array to pass to Synthe


                                do {
                                        if(!digitalRead(PUSH)) break;
                                        readLine();

                                }
                                while(digitalRead(GATE)==0);

                                do {
                                        if(!digitalRead(PUSH)) break;
                                        readLine();

                                }
                                while(digitalRead(GATE)==1);
                                Serial.println("SD speaking");
                                Serial.print(serialtext);
                                Synthe(serialtext);

osaki

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
        case 1:
                Serial.println("function 1");



                break;
        case 2:
                Serial.println("function 2");
                attachInterrupt(GATE, setRED_ON, CHANGE);
                WTF=HIGH;

                delay(400);
                display.clearDisplay();
                //  display.setCursor(0,0);
                //  display.print(" ");
                display.setCursor(15,0);
                display.print(mainFunctions[function]);
                // display : connect serial terminal
                display.display();
                display.setCursor(0,18);
                WTF=HIGH;
                do {

                        /*

                              display.clearDisplay();

                              //  display.setCursor(0,0);
                              //    display.print(" ");
                              display.setCursor(15,0);
                              display.print(mainFunctions[function]);
                              display.setCursor(0,18);

                              display.print(terminal);
                              display.display();



                              while(Serial.available()) {
                                      terminal= Serial.readString();        // read the incoming data as string


                              }
                         */

                        do {
                                if(!digitalRead(PUSH)) break;
                                readSerial();
                        }


                        while(digitalRead(GATE)==0);
                        if(!digitalRead(SW1)) Break(); // stop talking when gate goes low


                        //End of HIGH gate
                        // waiting at low level

                        do {
                                if(!digitalRead(PUSH)) break;
                                readSerial();
                        }

                        while(digitalRead(GATE)==1);
                        SetSpeed(map(analogRead(1),1023,0,50,300));
                        delayMicroseconds(10000);
                        SetPitch(map(analogRead(2),1023,0,254,0));
                        delayMicroseconds(10000);
                        SetAccent(map(analogRead(3),1023,0,0,255));

                        Serial.println("Start speaking");
                        Serial.print(serialtext);
                        //Serial.print(terminal.length());
                        //  char kaiwa[terminal.length()];
                        //  terminal.toCharArray(kaiwa,terminal.length()+1);
                        Synthe(serialtext);




                } while(digitalRead(PUSH));
                detachInterrupt(GATE);
                digitalWrite(RED_LED,HIGH);
                WTF=LOW;
                // push trap
                display.clearDisplay();
                display.display();

                while(!digitalRead(PUSH));
                //delay(200);

                break;
        case 3:
                Serial.println("function 3");
                break;
        }



}

/*
   ███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████
   ██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██
   █████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████
   ██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
   ██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████
 */

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
                        //  sayLine();
                }
        }
}


void setRED_ON()
{
        if (WTF) { // sorry but I cannot find how to stop this interrupt from happening when turning the rotary
                if (digitalRead(GATE))
                {
                        digitalWrite(RED_LED,HIGH);

                }
                else
                {
                        digitalWrite(RED_LED,LOW);

                }
        }
}



void displayFunctionList(int p)
{

        //  detachInterrupt(ROTA);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(0,0);
        display.print(">");
        for (int i = 0; i<3; i++) {

                display.setCursor(15,i*20);  //
                String str2=mainFunctions[i+p];
                display.print(str2);


        }
        display.display();
        //attachInterrupt(ROTA, rot, CHANGE);
}

void readSerial()

{

        if (Serial.available())
        {
                int inByte = Serial.read();
                switch  (inByte)

                {
                case 13:
                {
                        serialPointer=0;
                        memset( &serialtext, 0, 40 );
                        display.clearDisplay();
                        display.setCursor(15,0);
                        display.print(mainFunctions[function]);
                        //fh

                        display.display();
                        display.setCursor(0,18);
                        break;
                }

                case 8:

                        break;

                case 10:
                        break;


                default:
                {
                        serialtext[serialPointer]=char(inByte);
                        serialPointer++;
                        display.print(char(inByte));
                        display.display();
                        break;
                }

                }
        }

}

void readLine()
{

        display.clearDisplay();
        display.setFont(&Orbitron_Light_22);
        display.setTextSize(1);
        display.setCursor(0,16);
        ligne=map(analogRead(A6),0,1023,linePointer-1,0);
        display.setTextSize(1);
        display.print(ligne+1);
        display.setFont(&Orbitron_Light_24);
        display.setCursor(0,40);
        display.setTextSize(1);

        //display.setTextSize(3);

        // erase
        memset( &serialtext, 0, 40 );
        for (int p=0; p<40; p++)

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
                        //  Serial.println("");
                        break;
                }
        }

        display.display();
        display.setFont();



}

void sayLine()
{

        static unsigned long last_interrupt_time = 0;
        unsigned long interrupt_time = millis();
        // If interrupts come faster than 200ms, assume it's a bounce and ignore
        if (interrupt_time - last_interrupt_time > 1) // wtf (this was for the manual press !!!)
        {


                for (int p=0; p<40; p++)
                {
                        if (song[ligne][p] !=10)
                        {
                                //  Serial.write(song[ligne][p]);
                                Serial.print(song[ligne][p]);
                        }
                        else
                        {
                                //Serial.write(10);
                                Serial.println("");
                                break;
                        }
                }




        }
        last_interrupt_time = interrupt_time;



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
                                Serial.println("");
                                break;
                        }
                }

        }
}

void splashScreen()
{
        display.clearDisplay();
        display.setCursor(0,0);
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.println("Polaxis");
        display.display();
        delay(500);

        display.setTextSize(3);
        display.setCursor(0,22);
        display.println("Kaiwa");
        display.setTextSize(2);
        display.setCursor(0,50);
        display.println(VERSION);
        display.display();
        delay(1500);
}
void GetFilesList(File dir) {
        fileNumber=0;

        while(true) {


                File entry =  dir.openNextFile();// was dir
                if (!entry) {
                        // no more files
                        break;
                }

                if(isTxtFile(entry.name())) //check if it's a .txt file
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

                        myFiles[fileCounter]=strdup(char_array); //WTF is strdup ?
                        fileCounter++;

                }
                entry.close();

        }
        fileNumber=fileCounter;
}


// get rotary
void rot()
{


        delayMicroseconds(1000);
        if(digitalRead(ROTA))

                if(digitalRead(ROTB)) { interruptCount--; }
                else
                {

                        interruptCount++;
                }

}



bool isTxtFile(char* filename) {
        bool result;
        if(strstr(strlwr(filename),".txt") && !strstr(strlwr(filename),"_") ) // filtering out just ".txt" file not containing "_"
        { result=true; }
        else
        { result= false; }
        return result;
}



void displayFilesList(int p)
{

        //  detachInterrupt(ROTA);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(0,0);
        display.print(">");
        for (int i = 0; i<3; i++) {

                display.setCursor(15,i*20); //
                String str2=myFiles[i+p];
                str2.remove(str2.length()-4);

                display.print(str2);


        }
        display.display();
//attachInterrupt(ROTA, rot, CHANGE);
}
