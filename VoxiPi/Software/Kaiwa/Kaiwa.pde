import processing.io.*;
I2C i2c;
int BUSY =17;
int SPEAK =25;
int RST= 4;
boolean req;
boolean clr;
String message="konnichiwa";
int loop;
int s;


StringList inventory;



void setup() {
  size(640, 360);
  noStroke();
  background(0);

  GPIO.pinMode(SPEAK, GPIO.INPUT_PULLUP);
  GPIO.pinMode(BUSY, GPIO.INPUT_PULLUP);
  GPIO.pinMode(RST, GPIO.OUTPUT);
  GPIO.digitalWrite(RST, GPIO.HIGH);
  delay(100);
  GPIO.attachInterrupt(SPEAK, this, "speakRequest", GPIO.FALLING);
  GPIO.attachInterrupt(BUSY, this, "doneTalking", GPIO.RISING);

  printArray(I2C.list());
  i2c = new I2C(I2C.list()[0]);




  setClock(100);
  Reset();

  setPitch(100);
  setAccent(64);

  setSpeed(100);


  inventory = new StringList();
  inventory.append("bokuwaongakuka, dentakukatateni");
  inventory.append("tashitari");
  inventory.append("hiitari");
  inventory.append("sousashite");
  inventory.append("sakkyokusuru");
  inventory.append("konobotanoseba, ongakukanaderu");


  s = inventory.size();
  loop=0;
}

void Reset()
{
  delay(100);
  GPIO.digitalWrite(RST, GPIO.LOW);
  delay(30);
  GPIO.digitalWrite(RST, GPIO.HIGH);
  delay(100);
}

void setSpeed(int speed)
{
 
 if (speed<50)  speed=50;
  else if (speed>300)  speed=300;
  while (GPIO.digitalRead(BUSY)==GPIO.LOW);
  print(hex(speed&0xFF, 2));
   delay(100);
  Say("#W002"+hex(speed&0xFF, 2));
  delay(100);
  while (GPIO.digitalRead(BUSY)==GPIO.LOW);
  Say("#W003"+hex(speed/256, 2));
  delay(100);
}



void setClock(int clock)
{
  Say("#W03E"+hex(clock, 2));
  if (clock==0xFF) clock=0;
  Say("#W03F"+hex(255-clock, 2));
}

// accent: 0-255 default:64 0:none 255:strong
void setAccent(int accent)
{
  Say("#W03C"+hex(accent, 2));
}


//  pitch: 0-254  default:64 0:high 254:low
void setPitch(int pitch)
{
  Say("#W03D"+hex(pitch, 2));
}
void Say(String msg)
{
  i2c.beginTransmission(0x2e);     
  i2c.write(msg);
  i2c.write("\r");
  i2c.endTransmission();
}

void speakRequest(int pin) {
  println("Received speak request");
  req=true;
}

void doneTalking(int pin) {
  println("Done talking"); 
  loop++; 
  clr=true;
}

void draw() {


  if (GPIO.digitalRead(BUSY) == GPIO.HIGH && req == true) {
    // button is pressed

    message=inventory.get(loop%s);
    Say(message);
    
    req=false;
    textSize(32);
    fill(255);
    textAlign(CENTER);
    text(message, width/2, height/2);
  }

  if (clr)
  {
    background(0);
    clr=false;
  }
}

// detect ENTER key and cleanup on keypress 
void keyPressed() {
  if (key==10)
  {
    req=true;
  }
  else 
  {
   GPIO.releasePin(RST);
  exit();
  }
}
