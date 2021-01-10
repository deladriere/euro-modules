#include <arduino.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "SDU.h"
#include "avdweb_AnalogReadFast.h"
#include <SD.h> // 868

/*
██╗  ██╗ █████╗ ██████╗ ██████╗ ██╗    ██╗ █████╗ ██████╗ ███████╗
██║  ██║██╔══██╗██╔══██╗██╔══██╗██║    ██║██╔══██╗██╔══██╗██╔════╝
███████║███████║██████╔╝██║  ██║██║ █╗ ██║███████║██████╔╝█████╗  
██╔══██║██╔══██║██╔══██╗██║  ██║██║███╗██║██╔══██║██╔══██╗██╔══╝  
██║  ██║██║  ██║██║  ██║██████╔╝╚███╔███╔╝██║  ██║██║  ██║███████╗
╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝  ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝
*/
// chip
#define BSY 11
#define RST 4

// Emy

#define SW0 0
#define SW1 1
#define GATE 2
#define PUSH 3
#define ROTA 6
#define ROTB 7
#define GREEN_LED 12
#define RED_LED 13
#define AUX 38

// pots

/*
    ██████╗ ██████╗      ██╗███████╗ ██████╗████████╗███████╗
   ██╔═══██╗██╔══██╗     ██║██╔════╝██╔════╝╚══██╔══╝██╔════╝
   ██║   ██║██████╔╝     ██║█████╗  ██║        ██║   ███████╗
   ██║   ██║██╔══██╗██   ██║██╔══╝  ██║        ██║   ╚════██║
   ╚██████╔╝██████╔╝╚█████╔╝███████╗╚██████╗   ██║   ███████║
    ╚═════╝ ╚═════╝  ╚════╝ ╚══════╝ ╚═════╝   ╚═╝   ╚══════╝
                                                             
 */

SSD1306AsciiWire display;
auto &ss = Serial5;
File root;
/*
██╗   ██╗ █████╗ ██████╗ ██╗ █████╗ ██████╗ ██╗     ███████╗███████╗
██║   ██║██╔══██╗██╔══██╗██║██╔══██╗██╔══██╗██║     ██╔════╝██╔════╝
██║   ██║███████║██████╔╝██║███████║██████╔╝██║     █████╗  ███████╗
╚██╗ ██╔╝██╔══██║██╔══██╗██║██╔══██║██╔══██╗██║     ██╔══╝  ╚════██║
 ╚████╔╝ ██║  ██║██║  ██║██║██║  ██║██████╔╝███████╗███████╗███████║
  ╚═══╝  ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝╚══════╝
                                                                    
*/

#define ON 0
#define OFF 1

char buf[256];
int speed = 0;
int volume = 0;
int voice = 0;
int pitch = 0; // tone  word is reserved
int sound = 0;
int interval = 0;
int chime = 0;
int language = 1;

byte counter = 0;
byte mode = 0;
byte lastmode = 9;

int lastSpeed = 99;
int lastVolume = 99;
int lastVoice = 99;
int lastPitch = 99;
int lastSound = -1;
int lastInterval = -1;
int lastChime = -1;
int lastLanguage = -1;
long lastchanged;
bool displayCleared;

// handling files

int filePointer = 0;
int fileCounter = 0; // file index
int fileNumber = 0;  // file number
int lineTable[200];  // max line
char song2[4000];
int pointer = 0;
int linePointer = 0;
int ligne = 0;
char *myFiles[20]; //
char inputChar;    // char reader

char *filetype[4] = {".txt", ".spk", ".tts", ".bin"};

long debouceRot = 0;

int speaker[7] = {3, 51, 52, 53, 54, 55};

char *mainFunctions[] = {

    "Numbers",
    "USB TTS",
    "SD READER",
    "Keyboard",
    "Code",
};

char *SWlabel[] = {
    "Loop",
    "Random",
    "Normal",
};

char getit;

int numFunctions = (sizeof(mainFunctions) / sizeof(mainFunctions[0]));
int function;
int fin = 0; /// pour pression longue

volatile int interruptCount = 0; // The rotary counter
volatile int endRange;           // because use in rot
volatile bool rotF;              // because use in rot

int endNumbers[4] = {1, 9, 99, 999};
int moy;

bool pressed;
bool triggered;

bool rd = 0; // rotary display

// USB TTS
#define COLS 100
char serialtext[COLS];
int serialPointer = 0;

/*
███████╗██╗   ██╗███╗   ██╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗███████╗
██╔════╝██║   ██║████╗  ██║██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
█████╗  ██║   ██║██╔██╗ ██║██║        ██║   ██║██║   ██║██╔██╗ ██║███████╗
██╔══╝  ██║   ██║██║╚██╗██║██║        ██║   ██║██║   ██║██║╚██╗██║╚════██║
██║     ╚██████╔╝██║ ╚████║╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║███████║
╚═╝      ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝
*/

//<start>[MK]::LogEnhancement v0.0.1 toggle 'Sprintln()' and 'Serial.print()'
//       on/off with preprocessor variable 'DEBUG_TRACE' (https://forum.arduino.cc/index.php?topic=46900.0)
#ifdef DEBUG_TRACE
#define Sprintln(MSG) Serial.println(MSG)
#define Sprint(MSG) Serial.print(MSG)
#else
#define Sprintln(MSG)
#define Sprint(MSG)
#endif
//<end>[MK]::LogEnhancement
void displayFilesList(int p)
{

  for (int i = 0; i < 4; i++)
  {

    display.setCursor(13, i * 8); //
    String str2 = myFiles[i + p];
    str2.remove(str2.length() - 4);
    display.clearToEOL();
    display.println(str2);
  }
}

bool isTxtFile(char *filename, int type)
{
  bool result;
  Sprint(type);
  Sprint(" ");
  Sprintln(filetype[type]);
  if (strstr(strlwr(filename), filetype[type]) && !strstr(strlwr(filename), "_")) // filtering out just ".txt" file not containing "_"
  {
    result = true;
  }
  else
  {
    result = false;
  }
  return result;
}

void GetFilesList(File dir, int type)
{
  fileNumber = 0;
  fileCounter = 0;
  for (int8_t i = 0; i < 20; i++)
  {
    myFiles[i] = ""; // to emty the list
  }

  while (true)
  {

    File entry = dir.openNextFile(); // was dir
    if (!entry)
    {
      // no more files
      break;
    }

    if (isTxtFile(entry.name(), type)) //check if it's a .txt file
    {

      String str = entry.name();
      //  str.remove(str.length()-4);

      // Length (with one extra character for the null terminator)
      int str_len = str.length() + 1;

      // Prepare the character array (the buffer)
      char char_array[str_len];

      // Copy it over
      str.toCharArray(char_array, str_len);
      //Sprintln(char_array);

      myFiles[fileCounter] = strdup(char_array); //WTF is strdup ?
      fileCounter++;
    }
    entry.close();
  }
  fileNumber = fileCounter;
}

void initSD()
{

  if (!SD.begin(10))
  {
    display.clear();
    display.println("Error");
    display.println("SD card");
    display.println("failed to ");
    display.println("initialize");
    while (1)
      ;
  }
  root = SD.open("/");
}

void readSerial()

{

  if (Serial.available())
  {
    int inByte = Serial.read();

    switch (inByte)

    {

    case 27:
    {

      serialPointer = 0;
      memset(&serialtext, 0, 40);
      //Sprintln("Clear");
      display.setCursor(0, 3);
      display.set1X();
      display.clearToEOL();
      Serial.println("");

      break;
    }

    case 8:

      serialPointer--;
      display.setCursor(display.col() - 11, 3); // delete last character 7+2x2 pixel =11
      display.clearToEOL();
      if (serialPointer < 0)
      {
        serialPointer = 0;
      }
      serialtext[serialPointer] = 0;
      Serial.write(inByte);
      Serial.print("\033[K");
      break;

    case 10:
      break;

    case 13:
    {
      triggered = true;
      break;
    }

    default:
    {
      serialtext[serialPointer] = char(inByte);
      serialPointer++;
      display.setRow(3);
      display.set1X();
      display.print(char(inByte));
      Serial.write(inByte);

      break;
    }
    }
  }
}

int potReadFast(int pot, int readings)
{
  moy = 0;
  for (int i = 0; i < readings; i++)
  {
    moy = moy + analogReadFast(pot);
    //  moy=moy +analogReadFast(pot,2);
  }
  return moy / readings;
}

void speak(char *msg)
{
  ss.write(0xFD);
  ss.write((byte)0x00);
  ss.write(2 + strlen(msg));
  ss.write(0x01);
  ss.write((byte)0x0);
  ss.write(msg);
}

void gateUp()
{
  triggered = true;
  // speak(buf);
}

void Reset()
{
  digitalWrite(RST, LOW);
  delay(50);
  digitalWrite(RST, HIGH);
  delay(500);
}

void rot()
{
  if ((millis() - debouceRot) < 5)
  {
    return;
  }
  debouceRot = millis();
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
  if (rd)
  {
    switch (function)
    {
    case 0:
      interruptCount = constrain(interruptCount, 0, 3);
      endRange = endNumbers[interruptCount];
      break;
    }
  }
}

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

void showBusy()
{
  digitalWrite(RED_LED, !digitalRead(BSY));
}

void getUser()
{
  mode = digitalRead(SW0) + digitalRead(SW1) * 2;
  speed = map(potReadFast(A1, 30), 4095, 0, 0, 10);
  pitch = map(potReadFast(A2, 20), 4095, 0, 0, 10);
  voice = map(potReadFast(A3, 10), 4095, 5, 0, 5);
  volume = map(potReadFast(A4, 20), 4095, 0, 0, 10);

  switch (function)
  {
  case 0:
    if (mode != lastmode)
    {
      display.setRow(6);
      display.clearToEOL();
      display.print("Mode ");
      display.println(SWlabel[mode - 1]);

      lastmode = mode;
      lastchanged = millis();
      displayCleared = false;
    }
    language = map(potReadFast(A5, 2), 4095, 0, 1, 3);
    if (language != lastLanguage)
    {

      display.setRow(6);
      display.clearToEOL();
      if (language == 1)
      {
        display.println("Chinese");
      }
      else
      {
        display.println("English");
      }

      lastLanguage = language;
      lastchanged = millis();
      displayCleared = false;
    }
    if (sound != lastSound)
    {
      display.setRow(4);
      display.clearToEOL();
      display.println(sound);
      lastSound = sound;
    }

    break;
  case 1:
    language = map(potReadFast(A5, 2), 0, 4095, 0, 2);
    language = constrain(language, 0, 1);
    if (language != lastLanguage)
    {

      display.setRow(6);
      display.clearToEOL();
      if (language == 1)
      {
        display.println("Pinyin");
      }
      else
      {
        display.println("English");
      }

      lastLanguage = language;
      lastchanged = millis();
      displayCleared = false;
    }
    break;
  }

  // display.setFont(Arial14);
  if (speed != lastSpeed)
  {
    display.setRow(6);
    display.clearToEOL();
    display.print("Speed ");
    display.println(speed);
    lastSpeed = speed;
    lastchanged = millis();
    displayCleared = false;
  }
  if (pitch != lastPitch)
  {
    display.setRow(6);
    display.clearToEOL();
    display.print("Pitch ");
    display.println(pitch);
    lastPitch = pitch;
    lastchanged = millis();
    displayCleared = false;
  }
  if (voice != lastVoice)
  {
    display.setRow(6);
    display.clearToEOL();
    display.print("Voice ");
    display.println(voice);
    lastVoice = voice;
    lastchanged = millis();
    displayCleared = false;
  }
  if (volume != lastVolume)
  {
    display.setRow(6);
    display.clearToEOL();
    display.print("Volume ");
    display.println(volume);
    lastVolume = volume;
    lastchanged = millis();
    displayCleared = false;
  }

  voice = speaker[voice];
  if (millis() - lastchanged > 2000 && !displayCleared) // clear display after 2 seconds
  {
    display.setRow(6);
    display.clearToEOL();
    displayCleared = true;
  }
}

void waitForSpeechGPIO(unsigned long timeout = 60000)
{
  unsigned long start = millis();
  bool done = false;
  while (!done && (millis() - start) < timeout)
  {
    getUser();
    while (digitalRead(BSY))
    {
    }
    done = true;
    break;
  }
}

void waitForSpeech(unsigned long timeout = 60000)
{
  unsigned long start = millis();
  bool done = false;
  while (!done && (millis() - start) < timeout)
  {
    getUser();
    while (ss.available())
    {
      if (ss.read() == 0x4F)
      {
        done = true;
        break;
      }
    }
  }
}

/*
███████╗███████╗████████╗██╗   ██╗██████╗ 
██╔════╝██╔════╝╚══██╔══╝██║   ██║██╔══██╗
███████╗█████╗     ██║   ██║   ██║██████╔╝
╚════██║██╔══╝     ██║   ██║   ██║██╔═══╝ 
███████║███████╗   ██║   ╚██████╔╝██║     
╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝     
*/

void setup()
{
  Serial.begin(115200);
  ss.begin(115200);

  // GPIOs

  pinMode(BSY, INPUT_PULLUP);
  pinMode(RST, OUTPUT);
  Reset();
  pinMode(SW0, INPUT_PULLUP);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(ROTA, INPUT_PULLUP);
  pinMode(ROTB, INPUT_PULLUP);
  pinMode(PUSH, INPUT_PULLUP);
  pinMode(GATE, INPUT_PULLUP);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(REG_DAC_DATABUF, OUTPUT);

  // Set GPIO;
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, HIGH);

  analogReadResolution(12);

  Wire.begin();
  Wire.setClock(1500000L);              // speed the display to the max
  display.begin(&Adafruit128x64, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
  display.clear();
  display.set1X();
  display.setFont(fixed_bold10x15);
  display.println("Sino Vox");
  display.setRow(4);
  display.println("Ver. 0.01");
  display.setRow(6);
  //display.println(VERSION);

  // Interrupts
  attachInterrupt(BSY, showBusy, CHANGE);
  attachInterrupt(ROTA, rot, CHANGE);
  attachInterrupt(GATE, gateUp, FALLING);

  sprintf(buf, "[d][h2]");
  speak(buf);
  waitForSpeech();
  sprintf(buf, "Welcometoothemacheene");
  speak(buf);
  waitForSpeech();
  delay(1000);
}

/*
██╗      ██████╗  ██████╗ ██████╗ 
██║     ██╔═══██╗██╔═══██╗██╔══██╗
██║     ██║   ██║██║   ██║██████╔╝
██║     ██║   ██║██║   ██║██╔═══╝ 
███████╗╚██████╔╝╚██████╔╝██║     
╚══════╝ ╚═════╝  ╚═════╝ ╚═╝     
                                  
*/

void loop()
{
  interruptCount = 0;
  display.clear();
  delay(300);
  display.set1X();
  display.setFont(fixed_bold10x15);
  display.setCursor(0, 12);
  display.println(">");
  rotF = 1;
  //rd = 0;
  do
  {

    if (rotF)
    {
      interruptCount = constrain(interruptCount, 0, numFunctions - 1);
      function = interruptCount;
      displayFunctionList(function);
      rotF = 0;
    }

  } while (digitalRead(PUSH));
  delay(400);
  switch (function)
  {
  case 0:
  {
    //  printf(buf, "[d][h2]");
    //  speak(buf);
    //  waitForSpeech();
    display.clear();
    display.println("Shuzi");
    display.println(">0-");
    lastSound = -1; // to force first display
    rotF = 1;       // to force first display
    endRange = 1;
    rd = 1;

    do
    {
      fin = 0;
      getUser();
      if (rotF)
      {
        display.setCursor(35, 2);
        display.clearToEOL();
        display.println(endRange);
        rotF = 0;
      }

      if (digitalRead(GATE) == ON && !digitalRead(BSY) && triggered)
      {
        triggered = 0;

        switch (mode)
        {
        case 2:
          sound = random(endRange + 1);
          break;
        case 1:
          sound++;
          break;
        case 3:
          if (endRange > 1)
          {
            sound = map(potReadFast(A6, 20), 4095, 10, 0, endRange);
          }
          else
          {
            if (potReadFast(6, 10) < 2048)
            {
              sound = 1;
            }
            else
            {
              sound = 0;
            }
          }
          break;
        }
        sound = sound % (endRange + 1);
        Serial.println(sound);
        sprintf(buf, "[g%d][h2][t%d][s%d][m%d][v%d] %d", language, pitch, speed, voice, volume, sound);
        speak(buf);
      }

      do
      {
        fin++;

        //  Sprintln(fin);
        if (fin > 100000L)
          break;
      } while (digitalRead(PUSH) == 0);

    } while (fin < 100000L); // long presss
    rd = 0;
  } // end of case
  break;
  case 1:
  {
    sprintf(buf, "[d][h2]");
    speak(buf);
    waitForSpeech();
    display.clear();
    display.println("USB TTS");
    // display.println("com: 115.2 kBd 8n1");
    Serial.print("\033\143");   // ANSI   clear screen
    memset(&serialtext, 0, 40); // or COLS ?
    rd = 0;                     // no need for extra rotary functions
    triggered = false;
    pressed = false;
    do
    { // main
      fin = 0;
      getUser();
      readSerial();
      if (!digitalRead(BSY) && triggered)
      {
        triggered = 0;

        getUser();
        sprintf(buf, "[i%d][h2][t%d][s%d][m%d][v%d] %s", language, pitch, speed, voice, volume, serialtext);
        speak(buf);
      }
      do
      {
        fin++;

        if (fin > 100000L)
          break;
      } while (digitalRead(PUSH) == 0);
    } while (fin < 100000L);
    display.clear();
  } // end of case 1
  break;
  case 2: //@ SD READER
  {

    initSD();
    root = SD.open("/");
    root.rewindDirectory();
    GetFilesList(root, 0);
    interruptCount = 0;
    rotF = 1;
    rd = 0;

    do
    {
      // choose the file
      if (rotF)
      {
        interruptCount = constrain(interruptCount, 0, fileNumber - 1);
        filePointer = interruptCount;
        displayFilesList(filePointer);
        rotF = 0;
      }
    } while (digitalRead(PUSH));
    delay(400); // ok when have a file

    display.clear();
    display.setCursor(0, 12);
    Sprint("Reading file:");
    display.println("Reading ...");
    display.setCursor(0, 28); //
    display.println(myFiles[filePointer]);

    String str = myFiles[filePointer];
    str.remove(str.length() - 4);

    Sprintln(str);

    File myfile = SD.open(myFiles[filePointer]);

    pointer = 0;
    linePointer = 0;
    sprintf(buf, "[d][h2]");
    speak(buf);
    do
    {
      while (myfile.available() && digitalRead(PUSH))
      {
        do
        {
          getUser();
        } while (digitalRead(BSY));

        inputChar = myfile.read();
        
        sprintf(buf, "[i0][h2][t%d][s%d][m%d][v%d] %c", pitch, speed, voice, volume, inputChar);
       // Serial.print(" ");
       // Serial.println(buf);
        speak(buf);
        Serial.println(inputChar);
        waitForSpeech();
        delay(10);
      }
      fin = 0;

      do
      {
        fin++;

        if (fin > 100000L)
          break;
      } while (digitalRead(PUSH) == 0);

    } while (fin < 100000L);
    myfile.close();
    display.clear();

  } // end of case 2
  break;
  } // end fo case
} // end of loop
/*
case 1:
  {
    display.clear();
    display.println("USB TTS");
    display.println("com: 115.2 kBd 8n1");

    do
    { // main
      fin = 0;
      do
      {
        fin++;

        if (fin > 100000L)
          break;
      } while (digitalRead(PUSH) == 0);
    } while (fin < 100000L);
    display.clear();
  } // end of case
  break;
  }*/
