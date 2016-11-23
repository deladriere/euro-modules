
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "SAMD_AnalogCorrection.h"

/*
 █████  ██████  ██████  ██████  ███████ ███████ ███████ ███████ ███████
██   ██ ██   ██ ██   ██ ██   ██ ██      ██      ██      ██      ██
███████ ██   ██ ██   ██ ██████  █████   ███████ ███████ █████   ███████
██   ██ ██   ██ ██   ██ ██   ██ ██           ██      ██ ██           ██
██   ██ ██████  ██████  ██   ██ ███████ ███████ ███████ ███████ ███████

OLED 0x3C
AQUESCLICK 0X2E

*/



 /*
    ██   ██  █████  ██████  ██████  ██     ██  █████  ██████  ███████
    ██   ██ ██   ██ ██   ██ ██   ██ ██     ██ ██   ██ ██   ██ ██
    ███████ ███████ ██████  ██   ██ ██  █  ██ ███████ ██████  █████
    ██   ██ ██   ██ ██   ██ ██   ██ ██ ███ ██ ██   ██ ██   ██ ██
    ██   ██ ██   ██ ██   ██ ██████   ███ ███  ██   ██ ██   ██ ███████

  */

// define busy = ? voltage = 4.8 v ok ?

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
     ██████  ██████       ██ ███████  ██████ ████████ ███████
     ██    ██ ██   ██      ██ ██      ██         ██    ██
     ██    ██ ██████       ██ █████   ██         ██    ███████
     ██    ██ ██   ██ ██   ██ ██      ██         ██         ██
     ██████  ██████   █████  ███████  ██████    ██    ███████
   */

  File root;
  Adafruit_SSD1306 display(0); // modif library for 64 pxls


  /*
  ███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████
  ██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██
  █████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████
  ██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
  ██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████
  */



void toggleOUTPUTS()
{
static boolean toggle;
toggle =! toggle;



digitalWrite(BLUE_LED,toggle);
digitalWrite(RED_LED,toggle);
digitalWrite(RST_Microbus,toggle);
digitalWrite(CS_Microbus,toggle);
digitalWrite(CS_SD,toggle);


}

void AnalogMap()
{
  Serial.print(" A1:");
  Serial.print(analogRead(A1));
  Serial.print(" MAP:");
  Serial.println(map(analogRead(A5),1023,0,0,26));
}
void AnalogINPUTS()
{
Serial.print("A1: ");
Serial.print(analogRead(A1));
Serial.print(" A2: ");
Serial.print(analogRead(A2));
Serial.print(" A3: ");
Serial.print(analogRead(A3));
Serial.print(" A4: ");
Serial.print(analogRead(A4));
Serial.print(" A5:");
Serial.print(analogRead(A5));
Serial.print(" A6: ");
Serial.print(analogRead(A6));
Serial.println();

}


  /*
     ███████ ███████ ████████ ██    ██ ██████
     ██      ██         ██    ██    ██ ██   ██
     ███████ █████      ██    ██    ██ ██████
          ██ ██         ██    ██    ██ ██
     ███████ ███████    ██     ██████  ██
   */


void setup() {

  Serial.begin(9600);
  //while (!Serial);
analogReadResolution(10);
  analogReadCorrection(20, 2090);


  pinMode(SW0, INPUT_PULLUP);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(GATE, INPUT_PULLUP);
  pinMode(PUSH, INPUT_PULLUP);
  pinMode(RST_Microbus, OUTPUT);
  pinMode(CS_Microbus, OUTPUT);
  pinMode(ROTA, INPUT_PULLUP);
  pinMode(ROTB, INPUT_PULLUP);
  pinMode(CS_SD, OUTPUT);
  pinMode(INT_Microbus,INPUT_PULLUP);
  pinMode(BLUE_LED,OUTPUT);
  pinMode(RED_LED,OUTPUT);

  // turn transistorized outputs low ;
  digitalWrite(BLUE_LED,HIGH);
  digitalWrite(RED_LED,HIGH);



}

/*
██       ██████   ██████  ██████
██      ██    ██ ██    ██ ██   ██
██      ██    ██ ██    ██ ██████
██      ██    ██ ██    ██ ██
███████  ██████   ██████  ██
*/



void loop() {

toggleOUTPUTS();
//AnalogINPUTS();
AnalogMap();

delay(100);

}
