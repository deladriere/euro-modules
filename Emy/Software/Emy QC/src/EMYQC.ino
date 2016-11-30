
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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
Adafruit_SSD1306 display(0);   // modif library for 64 pxls


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
        toggle =!toggle;



        digitalWrite(BLUE_LED,toggle);
        delay(100);
        digitalWrite(RED_LED,toggle);
        delay(100);
        digitalWrite(RST_Microbus,toggle);
        digitalWrite(CS_Microbus,toggle);
        digitalWrite(CS_SD,toggle);



}


/*
   ███████ ███████ ████████ ██    ██ ██████
   ██      ██         ██    ██    ██ ██   ██
   ███████ █████      ██    ██    ██ ██████
        ██ ██         ██    ██    ██ ██
   ███████ ███████    ██     ██████  ██
 */


void setup() {

        Serial.begin(19200);
        display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
        display.clearDisplay();
        analogReadCorrection(9, 2067);


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

        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.println("Emy QC Test");
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




}
