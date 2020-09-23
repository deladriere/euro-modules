#include <arduino.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "SDU.h"

/*
██╗  ██╗ █████╗ ██████╗ ██████╗ ██╗    ██╗ █████╗ ██████╗ ███████╗
██║  ██║██╔══██╗██╔══██╗██╔══██╗██║    ██║██╔══██╗██╔══██╗██╔════╝
███████║███████║██████╔╝██║  ██║██║ █╗ ██║███████║██████╔╝█████╗  
██╔══██║██╔══██║██╔══██╗██║  ██║██║███╗██║██╔══██║██╔══██╗██╔══╝  
██║  ██║██║  ██║██║  ██║██████╔╝╚███╔███╔╝██║  ██║██║  ██║███████╗
╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝  ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝
*/
// chip
#define RDY 11

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

char buf[100];
int speed = 0;
int volume = 0;
int voice = 0;
int pitch = 0; // tone is reserved

int lastSpeed = 99;
int lastVolume = 99;
int lastVoice = 99;
int lastPitch = 99;

int speaker[7] = {3, 51, 52, 53, 54, 55};

/*
███████╗██╗   ██╗███╗   ██╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗███████╗
██╔════╝██║   ██║████╗  ██║██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║██╔════╝
█████╗  ██║   ██║██╔██╗ ██║██║        ██║   ██║██║   ██║██╔██╗ ██║███████╗
██╔══╝  ██║   ██║██║╚██╗██║██║        ██║   ██║██║   ██║██║╚██╗██║╚════██║
██║     ╚██████╔╝██║ ╚████║╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║███████║
╚═╝      ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝
*/

void showBusy()
{
        digitalWrite(RED_LED, !digitalRead(RDY));      
}

void getPot()
{
  speed = map(analogRead(A1), 1023, 0, 0, 10);
  pitch = map(analogRead(A2), 1023, 0, 0, 10);
  voice = map(analogRead(A3), 1010, 5, 0, 5);
  volume = map(analogRead(A4), 1023, 0, 0, 10);
  // display.setFont(Arial14);
  if (speed != lastSpeed)
  {
    display.setRow(2);
    display.clearToEOL();
    display.print("Speed ");
    display.println(speed);
    lastSpeed = speed;
    lastVolume = 99; // to force volume display
  }
  if (pitch != lastPitch)
  {
    display.setRow(4);
    display.clearToEOL();
    display.print("Pitch ");
    display.println(pitch);
    lastPitch = pitch;
  }
  if (voice != lastVoice)
  {
    display.setRow(6);
    display.clearToEOL();
    display.print("Voice ");
    display.println(voice);
    lastVoice = voice;
  }
  if (volume != lastVolume)
  {
    display.setCursor(95, 2);
    display.clearToEOL();
    display.print("V");
    display.println(volume);
    lastVolume = volume;
  }
  voice = speaker[voice];
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

void waitForSpeech(unsigned long timeout = 60000)
{
  unsigned long start = millis();
  bool done = false;
  while (!done && (millis() - start) < timeout)
  {
    getPot();
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
  ss.begin(9600);

  // GPIOs

  pinMode(RDY, INPUT_PULLUP);

  pinMode(SW0, INPUT_PULLUP);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(ROTA, INPUT_PULLUP);
  pinMode(ROTB, INPUT_PULLUP);
  pinMode(PUSH, INPUT_PULLUP);
  pinMode(GATE, INPUT_PULLUP);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(REG_DAC_DATABUF, OUTPUT);

  // turn transistorized outputs low ;
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, HIGH);

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
  attachInterrupt(RDY, showBusy, CHANGE);

  sprintf(buf, "[d]");
  speak(buf);
  waitForSpeech();

  do
  {
    for (int i = 101; i < 126; i++)
    { // message pitchs

      sprintf(buf, "[x1]sound%d", i);
      Serial.println(i);
      speak(buf);
      waitForSpeech();
      Serial.println("");
      delay(500);
    }
    delay(3000);

    for (int i = 201; i < 226; i++)
    { // ring pitchs
      sprintf(buf, "[x1]sound%d", i);
      Serial.println(i);
      speak(buf);
      waitForSpeech();
      Serial.println("");
      delay(500);
    }

    delay(3000);
    for (int i = 301; i < 331; i++)
    { // alarm pitchs
      sprintf(buf, "[x1]sound%d", i);
      Serial.println(i);
      speak(buf);
      waitForSpeech();
      Serial.println("");
      delay(500);
    }

    delay(3000);
  } while (1);
  do
  {

    sprintf(buf, "[i0][x1]sound219");
    speak(buf);
    waitForSpeech();

    //getPot();
    sprintf(buf, "[t%d][s%d][m%d][v%d][h2][i1]jing1chang2xi3shou3[p500]", pitch, speed, voice, volume);
    speak(buf);
    waitForSpeech();
    //delay(500);

    //getPot();
    sprintf(buf, "[t%d][s%d][m%d][v%d][h2][i0]wash your hands often[p1000]", pitch, speed, voice, volume);
    speak(buf);
    waitForSpeech();

    //getPot();
    sprintf(buf, "[t%d][s%d][m%d][v%d][h2][i1]bi4mian3mi4qie4jie1chu4[p500]", pitch, speed, voice, volume);
    speak(buf);
    waitForSpeech();

    //getPot();
    sprintf(buf, "[t%d][s%d][m%d][v%d][h2][i0]Avoid close contact[p1000]", pitch, speed, voice, volume);
    speak(buf);
    waitForSpeech();

    //getPot();
    sprintf(buf, "[t%d][s%d][m%d][v%d][h2][i1]wei2zhebier2en2zhe1zhu4kou3bi2[p500]", pitch, speed, voice, volume);
    speak(buf);
    waitForSpeech();

    //getPot();
    sprintf(buf, "[t%d][s%d][m%d][v%d][h2][i0]Cover your mouth and nose with a mask when around others[p1000]", pitch, speed, voice, volume);
    speak(buf);
    waitForSpeech();

    //getPot();
    sprintf(buf, "[t%d][s%d][m%d][v%d][h2][i1]qing1jie2he2xiao1du2[p500]", pitch, speed, voice, volume);
    speak(buf);
    waitForSpeech();

    //getPot();
    sprintf(buf, "[t%d][s%d][m%d][v%d][h2][i0]Clean and disinfect[p1000]", pitch, speed, voice, volume);
    speak(buf);
    waitForSpeech();

    delay(2000);

  } while (1);
  sprintf(buf, "[h][i0]Welcometoothemacheene");
  speak(buf);
  waitForSpeech();
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
  /*
  for (int i = 101; i < 126; i++)
  { // message pitchs
    sprintf(buf, "[x1]sound%d", i);
    speak(buf);
    waitForSpeech();
  }
  
  for (int i = 201; i < 226; i++) { // ring pitchs
    sprintf(buf, "[x1]sound%d", i);
    speak(buf);
    waitForSpeech();
  }
  for (int i = 301; i < 331; i++) { // alarm pitchs
    sprintf(buf, "[x1]sound%d", i);
    speak(buf);
    waitForSpeech();
  }
  */

  while (true)
  {
    /*sprintf( buf, "[m51][t1][n2][s4]Check %d. Battery is %d percent charged.", count, random(1, 100));
    speak(buf);
    waitForSpeech();
    sprintf( buf, "[m52][t10][n2][s5]A robot may not injure a human being or, through inaction");
   
    speak(buf);
    waitForSpeech();

*/
    getPot();
    sprintf(buf, "[m5%d][t%d][s%d][v%d]We are charging our battery", voice, pitch, speed, volume);
    speak(buf);
    waitForSpeech();

    getPot();
    sprintf(buf, "[m5%d][t%d][s%d][v%d]And now we are full of energy", voice, pitch, speed, volume);
    speak(buf);
    waitForSpeech();

    getPot();
    sprintf(buf, "[m5%d][t%d][s%d][v%d]We are the robots", voice, pitch, speed, volume);
    speak(buf);
    waitForSpeech();

    getPot();
    sprintf(buf, "[m5%d][t%d][s%d][v%d]We are functioning automatic", voice, pitch, speed, volume);
    speak(buf);
    waitForSpeech();

    getPot();
    sprintf(buf, "[m5%d][t%d][s%d][v%d]And we are dancing mehkanic", voice, pitch, speed, volume);
    speak(buf);
    waitForSpeech();

    getPot();
    sprintf(buf, "[m5%d][t%d][s%d][v%d]Ja tvoi sluga", voice, pitch, speed, volume);
    speak(buf);
    waitForSpeech();

    getPot();
    sprintf(buf, "[m5%d][t%d][s%d][v%d]Ja tvoi rabotnik", voice, pitch, speed, volume);
    speak(buf);
    waitForSpeech();

    delay(1000);
  }
}
