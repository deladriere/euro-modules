
/**
 * Simple Write. 
 * 
 * Check if the mouse is over a rectangle and writes the status to the serial port. 
 * This example works with the Wiring / Arduino program that follows below.
 */


import processing.serial.*;
import processing.io.*;

Serial myPort;  // Create object from Serial class
int val;        // Data received from the serial port

int BUSY =17;





void setup() 
{
  size(200, 200);
  GPIO.pinMode(BUSY, GPIO.INPUT_PULLUP);
  print(Serial.list());
  String portName = Serial.list()[3];
  myPort = new Serial(this, portName, 115200);
  Say("[d][h2]");
  Say("Welcometoothemacheene");
}

void draw() {
}

void Say(String msg)
{
  while (GPIO.digitalRead(BUSY)==GPIO.HIGH);
  myPort.write(0xFD);
  myPort.write((byte)0x00);
  myPort.write(2 + msg.length());
  myPort.write(0x01);
  myPort.write((byte)0x0);
  myPort.write(msg);
}

void keyPressed() {

  GPIO.releasePin(BUSY);
  exit();
}
