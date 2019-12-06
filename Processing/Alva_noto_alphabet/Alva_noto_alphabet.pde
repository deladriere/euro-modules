// Example by Tom Igoe 
 
import processing.serial.*; 
 
Serial myPort;    // The serial port
PFont myFont;     // The display font
String inString;  // Input string from serial port

int lf = 10;      // ASCII linefeed 
int lastS;         // last serial event
int posS=0; // to offet display by index
 
void setup() { 
    fullScreen();
  // You'll need to make this font with the Create Font Tool 
   PFont pfont;
 pfont = createFont("glass_tty_vt220.ttf",76,false); //Create a font
//  pfont = createFont("EnvyCodeR.ttf",76,false); //Create a font
 
 textFont(pfont);
// fill(0,255,0);
 // textFont(myFont, 150); 
  // List all the available serial ports: 
  printArray(Serial.list()); 
  // I know that the first port in the serial list on my mac 
  // is always my  Keyspan adaptor, so I open Serial.list()[0]. 
  // Open whatever port is the one you're using. 
  myPort = new Serial(this, Serial.list()[1], 115200); 
  myPort.bufferUntil(lf); 
  inString=" ";
  
} 
 
void draw() { 
  if ((millis()-lastS)>2000) // clear string after 2 sec of inactivity
  {
    //clear();  
    inString="";
  }
  
  background(0); 
   fill(35,44,246,255);
  for(int y=0 ; y<16 ;y++)
  {
  for (int i = 0; i < 45; i++) 
  {
  //  text((int(random(1,11)-1)),i*38+10,y*60);
    char letter = char(int(random(50,98)-1)); // 66,92
     text(letter,i*38+5,y*60);
     for(long z=0;z<30000;z++) // slow dow
     {
     }
  }
  }
   
  
 // text("" + inString, 5+76,4*115-40); 
  
  int x = 38*(posS%5+1)*6; 
for (int i = 0; i < inString.length(); i++) {
  // Each character is displayed one at a time with the charAt() function.
  // now use black block alt 219
  fill(0,0,0);
  char c1 = inString.charAt(i);
   
  if (c1!=32 && i <inString.length()-2)
  {
    rect(x, 3.5*115-40, 38, 60);
  }
   fill(255,255,255);
  text(inString.charAt(i),x+5,4*115-40);
  // All characters are spaced 10 pixels apart.
  x += 38; 
}
  
  
  
  
 // if (frameCount % 10 == 0) println(frameRate);
  
} 
 
void serialEvent(Serial p) { 
  inString = p.readString();  
  lastS= millis();
  posS ++;
} 
