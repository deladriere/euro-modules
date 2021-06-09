#include <arduino.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "SDU.h"
#include "avdweb_AnalogReadFast.h"
#include <SD.h> // 868

#include "SAMD_AnalogCorrection.h"
#include "avdweb_AnalogReadFast.h"
#include <FlashAsEEPROM.h> //1192

/*
███╗   ███╗██╗██████╗ ██╗
████╗ ████║██║██╔══██╗██║
██╔████╔██║██║██║  ██║██║
██║╚██╔╝██║██║██║  ██║██║
██║ ╚═╝ ██║██║██████╔╝██║
╚═╝     ╚═╝╚═╝╚═════╝ ╚═╝
*/

//#define SERIAL_MIDI

#ifdef SERIAL_MIDI
#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#else
#include <USB-MIDI.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();
#endif

#ifdef SERIAL_MIDI
#define VERSION "SERIAL MODE"
#else
#define VERSION "USB MODE"
#endif

byte DivClock = 0; // To ouptut the MIDI clock

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
#define BUSY 38
#define SW0 0
#define SW1 1
#define GATE 2
#define PUSH 3
#define ROTA 6
#define ROTB 7
#define GREEN_LED 12
#define RED_LED 13
#define AUX 38
/*

 ██████╗ █████╗ ██╗     ██╗██████╗ ██████╗  █████╗ ████████╗███████╗
██╔════╝██╔══██╗██║     ██║██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██╔════╝
██║     ███████║██║     ██║██████╔╝██████╔╝███████║   ██║   █████╗  
██║     ██╔══██║██║     ██║██╔══██╗██╔══██╗██╔══██║   ██║   ██╔══╝  
╚██████╗██║  ██║███████╗██║██████╔╝██║  ██║██║  ██║   ██║   ███████╗
 ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝
 */

#define ADC_GND_PIN A4
#define ADC_3V3_PIN A6

#define ADC_READS_SHIFT 8
#define ADC_READS_COUNT (1 << ADC_READS_SHIFT)

#define ADC_MIN_GAIN 0x0400
#define ADC_UNITY_GAIN 0x0800
#define ADC_MAX_GAIN (0x1000 - 1)
#define ADC_RESOLUTION_BITS 12
#define ADC_RANGE (1 << ADC_RESOLUTION_BITS)
#define ADC_TOP_VALUE (ADC_RANGE - 1)

#define MAX_TOP_VALUE_READS 10

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

FlashStorage(offset_store, int);
FlashStorage(gain_store, int);

/*
██╗   ██╗ █████╗ ██████╗ ██╗ █████╗ ██████╗ ██╗     ███████╗███████╗
██║   ██║██╔══██╗██╔══██╗██║██╔══██╗██╔══██╗██║     ██╔════╝██╔════╝
██║   ██║███████║██████╔╝██║███████║██████╔╝██║     █████╗  ███████╗
╚██╗ ██╔╝██╔══██║██╔══██╗██║██╔══██║██╔══██╗██║     ██╔══╝  ╚════██║
 ╚████╔╝ ██║  ██║██║  ██║██║██║  ██║██████╔╝███████╗███████╗███████║
  ╚═══╝  ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝╚══════╝
                                                                    
*/
#define CALIB
//#define DEBUG_TRACE

#define VERSION 0.22
#define ON 0
#define OFF 1

char buf[256];
int speed = 0;
int volume = 0;
int voice = 0;
int pitch = 0; // tone  word is reserved
int midipitch = 0;
boolean trackuserpitch = 0;
int sound = 0;
int interval = 0;
int chime = 0;
int language = 1;
int spell = 0;

byte counter = 0;
byte mode = 0;
byte lastmode = 9;

#define LPRESS 200000L

int lastSpeed = 99;
int lastVolume = 99;
int lastVoice = 99;
int lastPitch = 99;
int lastSound = -1;
int lastSpell = -1;
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
char *myFiles[25]; //
char inputChar;    // char reader

char *filetype[4] = {".txt", ".spk", ".tts", ".bin"};

long debouceRot = 0;
long deboucePush = 0;

int speaker[7] = {3, 51, 52, 53, 54, 55};

char *mainFunctions[] = {

    "Numbers",
    "USB TTS",
    "SD READER",
    "Code",
    "MIDI",
};

char *SWlabel[] = {
    "Loop",
    "Random",
    "Normal",
};

char *SW2abel[] = {
    "Manual",
    "Auto",
    "Gated",
};

char *SW3abel[] = {
    "Letter trigger",
    "Space trigger",
    "Line trigger",
    "Random Skip",
};
char *SW4abel[] = {
    "Letter",
    "Word",
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
#define COLS 256
char serialtext[COLS];
int serialPointer = 0;

// waiting for speech
bool done = false;
int pseudoR;

struct menuCode
{
  char *label;
};

menuCode Code[]{
    ">Version",
    ">Calibrate",
    ">Flash",
};

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

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  analogWrite(RED_LED, 255 - (velocity << 1));
  // Sprintln(pitch);
  switch (pitch)
  {
  case 46:
    triggered = 1;

    //display.setCursor(0, 3);
    //display.set1X();
    //display.clearToEOL();
    Serial.println("");
    break;

  default:
    serialtext[serialPointer] = char(pitch);
    serialPointer++;
    //display.setRow(3);
    //display.set1X();
    //display.print(char(pitch));
    Serial.write(pitch);
    break;
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  pinMode(RED_LED, OUTPUT); // to allow switching the led off see : https://forum.arduino.cc/index.php?topic=156413.15
  digitalWrite(RED_LED, OFF);
}

void controlChange(byte channel, byte number, byte value)
{
  switch (number)
  {
  }
}

void Clock()
{
  DivClock++;
  if (DivClock > 5) // divide by 6
  {
    digitalWrite(BUSY, 1);
    DivClock = 0;
  }
  else
  {
    digitalWrite(BUSY, 0);
  }
}

void pitchBend(byte channel, int bend)
{

  midipitch = map(bend, 8191, -8192, 10, 0);
  trackuserpitch = 0;
  Sprintln(midipitch);
}

void Start()
{
  // DivClock = 0;
  Sprintln("Start");
}

void Stop()
{
  Sprint("Stop");
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

void flashFirmare()
{
  display.clear();
  display.println(">");
  initSD();
  root = SD.open("/");
  root.rewindDirectory();
  SD.remove("firmware.bin");
  GetFilesList(root, 3);
  interruptCount = 0;
  rotF = 1;
  rd = 0; // to avoid interruptCount constrain
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
  display.println("Reading:");
  display.setCursor(0, 28); //

  display.println(myFiles[filePointer]);

  //  while (digitalRead(PUSH))
  //          ;

  File myFileIn = SD.open(myFiles[filePointer]);
  File myFileOut = SD.open("firmware.bin", FILE_WRITE);
  int cnt = 0;
  float full = myFileIn.size();
  display.println(full);
  Serial.println(full);
  int lastpercent = 0;
  int percent;
  size_t n;
  uint8_t buf[128];
  display.setCursor(0, 6);
  while ((n = myFileIn.read(buf, sizeof(buf))) > 0)
  {
    cnt++;
    percent = cnt * 128 * 128 / full; // 128=100%
    if (lastpercent != percent)
    {
      display.ssd1306WriteRam(255);
      lastpercent = percent;
    }
    myFileOut.write(buf, n);
  }

  myFileIn.close();
  Serial.println("closing");
  myFileOut.close();
  display.clear();
  NVIC_SystemReset(); // bye bye
}

uint16_t readGndLevel()
{
  uint32_t readAccumulator = 0;

  for (int i = 0; i < ADC_READS_COUNT; ++i)
    readAccumulator += analogRead(ADC_GND_PIN);

  uint16_t readValue = readAccumulator >> ADC_READS_SHIFT;

  //  display.print("CW A1(GND) = ");
  //  display.println(readValue);

  return readValue;
}

uint16_t read3V3Level()
{
  uint32_t readAccumulator = 0;

  for (int i = 0; i < ADC_READS_COUNT; ++i)
    readAccumulator += analogRead(ADC_3V3_PIN);

  uint16_t readValue = readAccumulator >> ADC_READS_SHIFT;

  if (readValue < (ADC_RANGE >> 1))
    readValue += ADC_RANGE;

  //  display.print("CCW A2(3.3V) = ");
  //  display.println(readValue);

  return readValue;
}

void calibration()
{
  display.println("Calibration");
  display.setRow(2);
  display.print(offset_store.read());
  display.print(",");
  display.print(gain_store.read());

  delay(1000);
  display.clear();

  do
  {
    /* code */

    display.setRow(0);
    display.println("Turn pots :");
    display.println("SOUND <=CCW");
    display.println("P4 => CW");
    display.println(",then push.");

  } while (digitalRead(PUSH));
  delay(400);
  digitalWrite(RED_LED, LOW);
  display.clear();
  display.setRow(3);
  display.set2X();
  display.println("Wait");
  display.set1X();

  int offsetCorrectionValue = 0;
  uint16_t gainCorrectionValue = ADC_UNITY_GAIN;

  Sprint("\r\nOffset correction (@gain = ");
  Sprint(gainCorrectionValue);
  Sprintln(" (unity gain))");

  // Set default correction values and enable correction
  analogReadCorrection(offsetCorrectionValue, gainCorrectionValue);

  for (int offset = 0; offset < (int)(ADC_OFFSETCORR_MASK >> 1); ++offset)
  {
    analogReadCorrection(offset, gainCorrectionValue);

    Sprint("   Offset = ");
    Sprint(offset);
    Sprint(", ");

    if (readGndLevel() == 0)
    {
      offsetCorrectionValue = offset;
      break;
    }
  }

  Sprintln("\r\nGain correction");

  uint8_t topValueReadsCount = 0U;

  uint16_t minGain = 0U,
           maxGain = 0U;

  analogReadCorrection(offsetCorrectionValue, gainCorrectionValue);
  Sprint("   Gain = ");
  Sprint(gainCorrectionValue);
  Sprint(", ");
  uint16_t highLevelRead = read3V3Level();

  if (highLevelRead < ADC_TOP_VALUE)
  {
    for (uint16_t gain = ADC_UNITY_GAIN + 1; gain <= ADC_MAX_GAIN; ++gain)
    {
      analogReadCorrection(offsetCorrectionValue, gain);

      Sprint("   Gain = ");
      Sprint(gain);
      Sprint(", ");
      highLevelRead = read3V3Level();

      if (highLevelRead == ADC_TOP_VALUE)
      {
        if (minGain == 0U)
          minGain = gain;

        if (++topValueReadsCount >= MAX_TOP_VALUE_READS)
        {
          maxGain = minGain;
          break;
        }

        maxGain = gain;
      }

      if (highLevelRead > ADC_TOP_VALUE)
        break;
    }
  }
  else if (highLevelRead >= ADC_TOP_VALUE)
  {
    if (highLevelRead == ADC_TOP_VALUE)
      maxGain = ADC_UNITY_GAIN;

    for (uint16_t gain = ADC_UNITY_GAIN - 1; gain >= ADC_MIN_GAIN; --gain)
    {
      analogReadCorrection(offsetCorrectionValue, gain);

      Sprint("   Gain = ");
      Sprint(gain);
      Sprint(", ");
      highLevelRead = read3V3Level();

      if (highLevelRead == ADC_TOP_VALUE)
      {
        if (maxGain == 0U)
          maxGain = gain;

        minGain = gain;
      }

      if (highLevelRead < ADC_TOP_VALUE)
        break;
    }
  }

  gainCorrectionValue = (minGain + maxGain) >> 1;

  analogReadCorrection(offsetCorrectionValue, gainCorrectionValue);

  Sprintln("\r\nReadings after corrections");
  Sprint("   ");
  readGndLevel();
  Sprint("   ");
  read3V3Level();

  Sprintln("\r\n==================");
  Sprintln("\r\nCorrection values:");
  Sprint("   Offset = ");
  Sprintln(offsetCorrectionValue);
  Sprint("   Gain = ");
  Sprintln(gainCorrectionValue);
  Sprintln("\r\nAdd the next line to your sketch:");
  Sprint("   analogReadCorrection(");
  Sprint(offsetCorrectionValue);
  Sprint(", ");
  Sprint(gainCorrectionValue);
  Sprintln(");");
  Sprintln("\r\n==================");

  digitalWrite(RED_LED, HIGH);

  display.clear();
  display.setRow(0);
  display.println("Apply cal.");
  display.print(offsetCorrectionValue);
  display.print(" , ");
  display.println(gainCorrectionValue);
  display.println("Press to \napply");

  while (digitalRead(PUSH))
    ;
  offset_store.write(offsetCorrectionValue);
  gain_store.write(gainCorrectionValue);

  analogReadCorrection(offsetCorrectionValue, gainCorrectionValue);
}

void splashScreen()
{
  display.clear();
  display.setFont(fixed_bold10x15);
  display.set1X();
  display.setRow(0);
  digitalWrite(RED_LED, HIGH);
  display.clear();
  display.setRow(0);
  display.println("Code");
  display.setRow(2);
  display.println("Version");
  display.setRow(4);
  display.println("SINO");
  display.setRow(6);
  display.println(VERSION);
  delay(1000);
}

void Exit()
{
  digitalWrite(RED_LED, ON);

  digitalWrite(GREEN_LED, ON);
  delay(500);
  digitalWrite(GREEN_LED, OFF);
  digitalWrite(RED_LED, OFF);
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
      memset(&serialtext, 0, COLS);
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

void PushDown()
{
  fin = 0;
  if ((millis() - deboucePush) < 200)
  {
    return;
  }
  pressed = true;
  deboucePush = millis();

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
    case 0: // Numbers
      interruptCount = constrain(interruptCount, 0, 3);
      endRange = endNumbers[interruptCount];
      break;
    case 3: // @code
      interruptCount = constrain(interruptCount, 0, 2);
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
  voice = map(potReadFast(A3, 10), 4095, 0, 0, 5);
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
  case 2:
    spell = map(potReadFast(A5, 2), 0, 1500, 1, 2);
    spell = constrain(spell, 1, 2);
    if (spell != lastSpell)
    {
      display.setRow(6);
      display.clearToEOL();
      display.print("Read ");
      display.println(SW4abel[spell - 1]);

      lastSpell = spell;
      lastchanged = millis();
      displayCleared = false;
    }
    if (mode != lastmode)
    {
      display.setRow(6);
      display.clearToEOL();
      display.print("Read ");
      display.println(SW2abel[mode - 1]);

      lastmode = mode;
      lastchanged = millis();
      displayCleared = false;
    }
    sound = map(potReadFast(A6, 10), 4095, 10, 4, 0); //
    if (sound < 1)
    {
      sound = 1;
    } // FIXME for a smoother display ?
    if (sound != lastSound)
    {
      display.setRow(6);
      display.clearToEOL();
      display.println(SW3abel[sound - 1]);
      lastSound = sound;
      lastchanged = millis();
      displayCleared = false;
    }
    break;
  case 4:
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
    trackuserpitch = true;
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

void CheckForSpeechFlag()
{
  if (ss.available())
  {
    if (ss.read() == 0x4F)
    {
      done = true;
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
  pinMode(BUSY, OUTPUT);
  digitalWrite(BUSY, OFF);
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
  display.setRow(0);

  if ((digitalRead(PUSH) == 0) | (gain_store.read() == 0))
  {
#ifdef CALIB
    calibration();
#endif
  }
  else
  { // apply correction
    analogReadCorrection(offset_store.read(), gain_store.read());
  }

  splashScreen();

  // Interrupts
  attachInterrupt(BSY, showBusy, CHANGE);
  attachInterrupt(ROTA, rot, CHANGE);
  attachInterrupt(GATE, gateUp, FALLING);
  attachInterrupt(PUSH, PushDown, FALLING);

  sprintf(buf, "[d][h2][i1]");
  speak(buf);
  waitForSpeech();
  sprintf(buf, "suo3you3xi4tong3dou1zhun3bei4jiu4xu4");
  speak(buf);
  waitForSpeech();
  delay(1000);

  // Initialize MIDI, and listen to all MIDI channels
  // This will also call usb_midi's begin()
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Attach the handleNoteOn function to the MIDI Library. It will
  // be called whenever the Bluefruit receives MIDI Note On messages.
  MIDI.setHandleNoteOn(handleNoteOn);

  // Do the same for MIDI Note Off messages.
  MIDI.setHandleNoteOff(handleNoteOff);

  MIDI.setHandleControlChange(controlChange);

  MIDI.setHandlePitchBend(pitchBend);

  MIDI.setHandleClock(Clock);

  MIDI.setHandleStart(Start);

  MIDI.setHandleStop(Stop);
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
  Exit();
  // delay(400);
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
  case 0: //@ Numbers
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
  case 1: //@ USB TTS
  {
    sprintf(buf, "[d][h2]");
    speak(buf);
    waitForSpeech();
    display.clear();
    display.println("USB TTS");
    // display.println("com: 115.2 kBd 8n1");
    Serial.print("\033\143");     // ANSI   clear screen
    memset(&serialtext, 0, COLS); // or COLS ?
    rd = 0;                       // no need for extra rotary functions
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
    sprintf(buf, "[d][h2][g2]"); // TODO: attention remove g2 ? force number to be read in English
    speak(buf);
    triggered = false;
    pressed = false;
    memset(&serialtext, 0, COLS);
    serialPointer = 0;
    fin = 0;
    do
    {
      myfile.seek(0);
      while (myfile.available() && fin < LPRESS)
      {

        getUser();

        if (!digitalRead(BSY) && (mode == 2 || (mode == 3 && triggered) || (mode == 1 && pressed)))
        {
          // triggered = false;
          // pressed = false; TODO:remove if not needed

          switch (sound)
          {
          case 1: // character mode
            inputChar = myfile.read();
            serialtext[0] = inputChar;
            break;
          case 2: // line and space mode
            do
            {
              inputChar = myfile.read();
              if (inputChar != 10)
              {
                serialtext[serialPointer] = inputChar;
                serialPointer++;
              }
            } while (myfile.available() && inputChar != 32 && inputChar != 10);
            break;
          case 3: // line mode
            do
            {
              inputChar = myfile.read();
              if (inputChar != 10)
              {
                serialtext[serialPointer] = inputChar;
                serialPointer++;
              }
            } while (myfile.available() && inputChar != 10);
            break;
          case 4: // pseudo random words
            pseudoR = random(10);

            do
            {
              inputChar = myfile.read();
              if (inputChar != 10 && pseudoR == 1)
              {
                serialtext[serialPointer] = inputChar;
                serialPointer++;
              }
              else
              {
                pressed = true;   // pretend to be pressed to skip
                triggered = true; // pretend to be triggered
              }

            } while (myfile.available() && inputChar != 32 && inputChar != 10);

            break;

          default:
            break;
          }
          if (sound < 4 || pseudoR == 1) // Skip word in random mode
          {
            sprintf(buf, "[i0][h%d][t%d][s%d][m%d][v%d] %s", spell, pitch, speed, voice, volume, serialtext);

            speak(buf);
            Serial.println(serialtext);
            display.setRow(4);
            display.clearToEOL();
            display.println(serialtext);
            triggered = false;
            pressed = false;
          }
          memset(&serialtext, 0, COLS);
          serialPointer = 0;
        }
        delay(5); // to allow BSY gpio to reflect busy state

        fin = 0;

        do
        {
          fin++;

          if (fin > LPRESS)
            break;
        } while (digitalRead(PUSH) == 0);
      }

    } while (fin < LPRESS);
    myfile.close();
    display.clear();

  } // end of case 2 SD reader
  break;
  case 3: //@Code
  {
    display.clear();
    display.println("Code");
    rd = 1;
    rotF = 1;
    interruptCount = 0;

    do
    {
      fin = 0;

      do
      {

        if (rotF)
        {

          display.setRow(2);
          display.clearToEOL();
          display.println(Code[interruptCount].label);
          rotF = 0;
        }

      } while (digitalRead(PUSH));

      switch (interruptCount)
      {
      case 0:
        splashScreen();

        while (digitalRead(PUSH))
          ;

        fin = 200000L;

        break;

      case 1:
        display.clear();
        calibration();
        fin = 200000L;

        break;

      case 2:
        interruptCount = 0;
        flashFirmare();
        fin = 200000L;
        break;
      }

      do
      {
        fin++;

        if (fin > LPRESS)
          break;
      } while (digitalRead(PUSH) == 0);

    } while (fin < LPRESS); // long presss

  } // end of case
    display.clear();
    break;
  case 4:
  {
    sprintf(buf, "[d][h2][i0]");
    speak(buf);
    waitForSpeech();
    display.clear();
    display.clear();
    display.println("MIDI");

    memset(&serialtext, 0, COLS); //
    rd = 0;                       // no need for extra rotary functions
    triggered = false;
    pressed = false;

    do
    { // main
      fin = 0;
      getUser();
      MIDI.read();

      if (!digitalRead(BSY) && triggered)
      {
        triggered = 0;
        if (!trackuserpitch)

        {
          pitch = midipitch;
        }
        //getUser();
        sprintf(buf, "[i%d][h2][t%d][s%d][m%d][v%d] %s", language, pitch, speed, voice, volume, serialtext);
        speak(buf);
        serialPointer = 0;
        memset(&serialtext, 0, COLS);
      }

      do
      {
        fin++;

        if (fin > LPRESS)
          break;
      } while (digitalRead(PUSH) == 0);
    } while (fin < LPRESS);
    display.clear();
  } // end of case
  break;

  } // end of switch
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

        if (fin > LPRESS)
          break;
      } while (digitalRead(PUSH) == 0);
    } while (fin < LPRESS);
    display.clear();
  } // end of case
  break;
  }*/
