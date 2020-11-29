#include <arduino.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "SDU.h"
#include "avdweb_AnalogReadFast.h"

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

char buf[100];
int speed = 0;
int volume = 0;
int voice = 0;
int pitch = 0; // tone  word is reserved
int sound = 0;

byte mode = 0;
byte lastmode = 9;

int lastSpeed = 99;
int lastVolume = 99;
int lastVoice = 99;
int lastPitch = 99;
int lastSound = -1;
long lastchanged;
bool displayCleared;

int speaker[7] = {3, 51, 52, 53, 54, 55};

char *mainFunctions[] = {

    "Numbers",
    "SD TTS",
    "Clock",
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
volatile bool rotF;              // because use in rot

int moy;

bool pressed;
bool triggered;

/*
███████╗██╗   ██╗███╗   ██╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗███████╗
██╔════╝██║   ██║████╗  ██║██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
█████╗  ██║   ██║██╔██╗ ██║██║        ██║   ██║██║   ██║██╔██╗ ██║███████╗
██╔══╝  ██║   ██║██║╚██╗██║██║        ██║   ██║██║   ██║██║╚██╗██║╚════██║
██║     ╚██████╔╝██║ ╚████║╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║███████║
╚═╝      ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝
*/

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
  voice = map(potReadFast(A3, 20), 4095, 5, 0, 5);
  volume = map(potReadFast(A4, 20), 4095, 0, 0, 10);
  sound = map(potReadFast(A6, 20), 4095, 10, 0, 99);
  if (sound != lastSound)
  {
    display.setRow(4);
    display.clearToEOL();
    display.println(sound);
    lastSound = sound;
  }

  if (mode != lastmode)
  {
    display.setRow(6);
    display.clearToEOL();
    display.print("Mode ");
    display.println(SWlabel[mode - 1]);

    lastmode = mode;
    lastchanged = millis();
    displayCleared=false;
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
    displayCleared=false;
  }
  if (pitch != lastPitch)
  {
    display.setRow(6);
    display.clearToEOL();
    display.print("Pitch ");
    display.println(pitch);
    lastPitch = pitch;
    lastchanged = millis();
    displayCleared=false;
  }
  if (voice != lastVoice)
  {
    display.setRow(6);
    display.clearToEOL();
    display.print("Voice ");
    display.println(voice);
    lastVoice = voice;
    lastchanged = millis();
    displayCleared=false;
  }
  if (volume != lastVolume)
  {
    display.setRow(6);
    display.clearToEOL();
    display.print("Volume ");
    display.println(volume);
    lastVolume = volume;
    lastchanged = millis();
    displayCleared=false;
  }
  voice = speaker[voice];
  if (millis() - lastchanged > 2000 && !displayCleared) // clear display after 2 seconds
  {
    display.setRow(6);
    display.clearToEOL();
    displayCleared=true;
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
    // getUser();
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
    display.println("Numbers");
    lastSound = -1; // to force first display
   
    do
    {
      fin = 0;
      getUser();
     
     if (digitalRead(GATE) == ON && !digitalRead(BSY) && triggered)
      {
        triggered = 0;

        sprintf(buf, "[h1][t%d][s%d][m%d][v%d] %d", pitch, speed, voice, volume, sound);
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

  } // end of case
  break;
  }
}
