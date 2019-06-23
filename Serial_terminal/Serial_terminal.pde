import processing.serial.*;
import controlP5.*;

Serial myPort;  // Create object from Serial class
char var; 

ControlP5 cp5;

Textlabel myTextlabelA;

String textValue = "";
int x=-60;
boolean flag=false;

void setup()
{
  clear();
   //size(1400, 400 );
  fullScreen(); 
    surface.setTitle("Serial Terminal");
  // I know that the first port in the serial list on my mac
  // is Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  println(Serial.list());
  String portName = Serial.list()[4]; //change the 0 to a 1 or 2 etc. to match your port
  
  myPort = new Serial(this, portName, 115200);

  
  //PFont pfont = createFont("data/glass_tty_vt220.ttf",100,false); //Create a font
  PFont pfont = createFont("Menlo.ttc",100,false); //Create a font
  
  
  textFont(pfont);
  fill(0,255,0);
   
}

void draw()
{
  
  
  
  if ( myPort.available() > 0) 
  {  // If data is available,
   
  var = myPort.readChar();
  
  print(var);
    if (flag && var!=10)
   {
      background(0);
      flag=false;
   }
  
  if (var!=13)
  {
    
    
    if (var==32)
    {
      x=x+30;
    }
    else
    {
      x=x+80; 
    }
   
  text(var,x,height/2);
  }
  else
  {
    x=-60;
    flag=true;
   
  }
  
  } 

}
