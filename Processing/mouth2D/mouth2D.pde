import processing.serial.*; 

String sentence = "-e-l-l-o- -m-a-n- - - -w-a-t- -c-a-n- -i- -d-o- -f-o-r- -y-u- - - -";
String[] phonems;
String info; 
PShape mouth,th, cd, ai, ll, qw, oo, ee, sh, rr, fv, uu, bm, neutral;
int count = 0;
PFont font;

// Serial Stuff
String inString;  // Input string from serial port
Serial myPort;    // The serial port
int lf = 10;      // ASCII linefeed 
int lastS;         // last serial event
String letter;


void setup() {
 // size(920, 540, P2D);
  fullScreen(P2D,2);
  textSize(32);
  textAlign(CENTER);
  
  mouth = loadShape("mouth2D.svg");
  mouth.scale(0.2); 
  mouth.translate(width/2, height/2);
  
  th = mouth.getChild("th");
  cd = mouth.getChild("cdgknstxyz");
  ai = mouth.getChild("aei");
  ll = mouth.getChild("l");
  qw = mouth.getChild("qw");
  oo = mouth.getChild("o");
  ee = mouth.getChild("ee");
  sh = mouth.getChild("shchj");
  rr = mouth.getChild("r");
  fv = mouth.getChild("fv");
  uu = mouth.getChild("u");
  bm = mouth.getChild("bmp");
  neutral = mouth.getChild("neutral");
  
  //th.setVisible(true);
  
  phonems = splitTokens(sentence, "-");
  info = join(phonems, "");
  
  
    myPort = new Serial(this, Serial.list()[1], 115200);
    myPort.bufferUntil(lf); 
   inString="";
   letter="PA0";
  
}

void draw_mouth(String letter)
{
  PShape gfx;
  gfx = th;
  
  switch(letter)
  {
    case " ": case "PA0": 
      gfx = neutral; break;
      
    case "IE": case "I": case "A": case "AI": case "EH": case "EH1": case "AE": case "AE1": case "AH": case "AH1": case "AW": case ":A": case "E2":
    case "a": case "e": case "i":
      gfx = ai; break ;
      
    case "B": case "P": case "M":
    case "b": case "m": case "p":
      gfx = bm; break ;
      
    case "Y": case "YI": case "AY": case "D": case "KV": case "T": case "Z": case "S": case "N": case "NG":
    case "c": case "d": case "g": case "k": case "n": case "s": case "t": case "x": case "y": case "z": 
      gfx = cd; break ;
      
    case "E": case "E1":
    case "ee":
    
      gfx = ee; break ;
      
    case "LF": case "HV": case "HVC": case "HF": case "HFC": case "HN": case "V": case "F":
    case "f": case "v":
      gfx = fv; break ;
      
    case "L": case "L1": case "LB":
    case "l":
      gfx = ll; break ;
      
    case "O": case "OU": case "OO": case ":OH":
    case "o":
      gfx = oo; break ;
    
    case "W":
    case "q": case "w":
      gfx = qw; break ;
      
    case "ER": case "R": case "R1": case "R2":
    case "r":
      gfx = rr; break ;
      
    case "J": case "SCH":
    case "sh": case "ch": case "j":
      gfx = sh; break ;
      
    case "K": case "THV": case "TH":
    case "th": 
      gfx = th; break ;
      
    case "IU": case "IU1": case "U": case "U1": case "UH": case "UH1": case "UH2": case "UH3": case ":U": case ":UH":
    case "u":
      gfx = uu; break ;
    
  }
  //print(letter);
  shape(gfx,0,0);
}

void draw() 
{
  background(0);

 if (letter!= "PA0")
  {
 // text(letter,width/2,100); 
  }

  
 // String letter = phonems[count++%phonems.length];
 
 draw_mouth(letter);
  //text(letter,width/2,height/2-100);
 // if (letter == " ")  delay(600); else delay(200);
}

void serialEvent(Serial p) { 
  inString = p.readString();  
  letter= inString.substring(0,inString.length()-2);
  lastS= millis();
 // print(letter);
} 
