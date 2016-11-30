#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>


// https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf

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




char* mainFunctions[12]={

        "SD tts",
        "SD phon",
        "USB tts",
        "USB phon","","",""
};

#define numFunctions 4
int function;

char song[60][120] {}; // max 80 caracter per lines // 100 lines max
int pointer=0;
int linePointer=0;
int ligne=0;



char inputChar;
char* myFiles[10];  // max 10 files
int fileCounter=0; // file index
int fileNumber=0; // file number
int filePointer=0;
char fileName[13];

String terminal;

static boolean rotating=false;

volatile int interruptCount=0; // The rotary counter

#define VERSION "1.0"

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
        pinMode(ROTA, INPUT_PULLUP);
        pinMode(ROTB, INPUT_PULLUP);
        pinMode(PUSH, INPUT_PULLUP);
        pinMode(GATE, INPUT_PULLUP);

        // turn transistorized outputs low ;
      //  digitalWrite(BLUE_LED,LOW);
      //  digitalWrite(RED_LED,LOW);
        analogReadResolution(10); //10 or 12 ?

        attachInterrupt(ROTA, rot, CHANGE);
        attachInterrupt(GATE, BLUE_ON, RISING);
        attachInterrupt(GATE, BLUE_OFF, FALLING);

        // Open Serial communications and wait for port to open:
        Serial.begin(115200);
        Serial.setTimeout(100);
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
BLUE_ON();
        do {
                digitalWrite(BLUE_LED,LOW);
                digitalWrite(RED_LED,HIGH);
                interruptCount=constrain(interruptCount,0,numFunctions-1);
                function=interruptCount;
                displayFunctionList(function);

        } while(digitalRead(PUSH));


        switch (function) {
        case 0:

        {
                interruptCount=0;
                digitalWrite(BLUE_LED,HIGH);
                digitalWrite(RED_LED,LOW);
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
                        attachInterrupt(GATE, sayLine, FALLING);
                        do { /// show A1


                                display.clearDisplay();

                                display.setCursor(0,0);
                                ligne=map(analogRead(A5),0,1023,linePointer-1,0); // en attendant la modif hardware
                                display.setTextSize(2);
                                display.print(ligne);

                                display.setCursor(0,20);
                                display.setTextSize(3);

                                for (int p=0; p<40; p++)
                                {
                                        if (song[ligne][p] !=10)
                                        {
                                                //  Serial.write(song[ligne][p]);
                                                display.print(song[ligne][p]);
                                        }
                                        else
                                        {
                                                //Serial.write(10);
                                                //  Serial.println("");
                                                break;
                                        }
                                }

                                display.display();


                        }

                        while(digitalRead(PUSH));
                        detachInterrupt(GATE);
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
                digitalWrite(BLUE_LED,HIGH);
                digitalWrite(RED_LED,LOW);
                delay(400);



                do {

                        while(Serial.available()) {
                                display.clearDisplay();

                                terminal= Serial.readString();// read the incoming data as string

                                Serial.print(terminal);
                                display.print(terminal);
                                display.display();

                        }


                } while(digitalRead(PUSH));

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

void BLUE_ON()
{
  digitalWrite(BLUE_LED,LOW);
}
void BLUE_OFF()
{
  digitalWrite(BLUE_LED,HIGH);
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
        display.println("Ver. 1.0");
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
