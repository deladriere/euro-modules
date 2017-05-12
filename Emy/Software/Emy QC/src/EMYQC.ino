
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // 128x64  version still needs to modify Adafruit_SSD1306.h for 64 lines
#include <Wire.h>
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

// DIGITAL INPUTS

#define SW0 0
#define SW1 1
#define GATE 2
#define PUSH 3
#define ROTA 6
#define ROTB 7


// DIGITALS OUTPUTS

#define BUSY 38
#define BLUE_LED  12
#define RED_LED 13


// MIKROBUS SOCKET




//      +-----------------+
//      |[ ]AN      PWM[ ]|
//      |[ ]RST     INT[ ]|
//      |[ ]CS       RX[ ]|
//      |[ ]SCK      TX[ ]|
//      |[ ]MISO    SCL[ ]|
//      |[ ]MOSI    SDA[ ]|
//  3V3 |[ ]3V3      5V[ ]| 5V
//      |[ ]GND     GND[ ]| GND
//      +________________/


  #define AN 9
  #define RST 4
  #define CS 5

  #define INT 11
  #define RX 31
  #define TX 30



  #define CS_SD 10






/*
   ██████  ██████       ██ ███████  ██████ ████████ ███████
   ██    ██ ██   ██      ██ ██      ██         ██    ██
   ██    ██ ██████       ██ █████   ██         ██    ███████
   ██    ██ ██   ██ ██   ██ ██      ██         ██         ██
   ██████  ██████   █████  ███████  ██████    ██    ███████
 */

File root;
Adafruit_SSD1306 display(0);   // modif library for 64 pxls


/*
   ███████ ██    ██ ███    ██  ██████ ████████ ██  ██████  ███    ██ ███████
   ██      ██    ██ ████   ██ ██         ██    ██ ██    ██ ████   ██ ██
   █████   ██    ██ ██ ██  ██ ██         ██    ██ ██    ██ ██ ██  ██ ███████
   ██      ██    ██ ██  ██ ██ ██         ██    ██ ██    ██ ██  ██ ██      ██
   ██       ██████  ██   ████  ██████    ██    ██  ██████  ██   ████ ███████
 */

void setBLUE_ON() {
        digitalWrite(BLUE_LED,digitalRead(GATE));
}



void toggleOUTPUTS()
{
        static boolean toggle;
        toggle =!toggle;



        digitalWrite(BLUE_LED,toggle);
        delay(100);
        digitalWrite(RED_LED,toggle);
        delay(100);
        digitalWrite(RST,toggle);
        digitalWrite(AN,toggle);
        digitalWrite(CS,toggle);
        digitalWrite(BUSY,toggle);



}


/*
   ███████ ███████ ████████ ██    ██ ██████
   ██      ██         ██    ██    ██ ██   ██
   ███████ █████      ██    ██    ██ ██████
        ██ ██         ██    ██    ██ ██
   ███████ ███████    ██     ██████  ██
 */


void setup() {
        analogReadResolution(12);
        analogReadCorrection(14, 2831);
        Serial.begin(19200);
        display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
        display.clearDisplay();



        pinMode(RST, OUTPUT);
        pinMode(CS, OUTPUT);
        pinMode(CS, OUTPUT);
        pinMode(BLUE_LED,OUTPUT);
        pinMode(RED_LED,OUTPUT);
        pinMode(BUSY,OUTPUT);

        pinMode(SW0, INPUT_PULLUP);
        pinMode(SW1, INPUT_PULLUP);
        pinMode(GATE, INPUT_PULLUP);
        pinMode(PUSH, INPUT_PULLUP);

        pinMode(ROTA, INPUT_PULLUP);
        pinMode(ROTB, INPUT_PULLUP);

        pinMode(INT,INPUT_PULLUP);


        // turn transistorized outputs low ;
        digitalWrite(BLUE_LED,HIGH);
        digitalWrite(RED_LED,HIGH);

        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.println("Emy QC");
        display.display();

}

/*
   ██       ██████   ██████  ██████
   ██      ██    ██ ██    ██ ██   ██
   ██      ██    ██ ██    ██ ██████
   ██      ██    ██ ██    ██ ██
   ███████  ██████   ██████  ██
 */



void loop() {

        do
        {
                toggleOUTPUTS();

        }
        while(digitalRead(PUSH));
        delay(300);
        // turn transistorized outputs low ;
        digitalWrite(BLUE_LED,HIGH);
        digitalWrite(RED_LED,HIGH);

        do
        {
                Serial.print(analogRead(A1));
                Serial.print(" ");
                Serial.print(analogRead(A2));
                Serial.print(" ");
                Serial.print(analogRead(A3));
                Serial.print(" ");
                Serial.print(analogRead(A4));
                Serial.print(" ");
                Serial.print(analogRead(A5));
                Serial.print(" ");
                Serial.println(analogRead(A6));

                display.clearDisplay();
                display.setCursor(0,0);
                display.println(analogRead(A1));
                display.setCursor(0,25);
                display.println(analogRead(A2));
                display.setCursor(0,50);
                display.println(analogRead(A3));
                display.setCursor(60,0);
                display.println(analogRead(A4));
                display.setCursor(60,25);
                display.println(analogRead(A5));
                display.setCursor(60,50);
                display.println(analogRead(A6));

                display.display();

                delay(100);

        }
        while(digitalRead(PUSH));
        delay(300);
        attachInterrupt(digitalPinToInterrupt(GATE), setBLUE_ON, CHANGE);
        do
        {


        }
        while(digitalRead(PUSH));
        delay(300);
        detachInterrupt(GATE);

        digitalWrite(BLUE_LED,HIGH);
        digitalWrite(RED_LED,HIGH);
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.println("Imputs Test");
        display.display();

        do
        {
                digitalWrite(BLUE_LED,digitalRead(SW0));
                digitalWrite(RED_LED,digitalRead(SW1));

        }
        while(digitalRead(PUSH));
        delay(300);
        digitalWrite(BLUE_LED,HIGH);
        digitalWrite(BLUE_LED,HIGH);



}


/*
   do
   {


   }
   while(digitalRead(PUSH));
   delay(300);

 */
