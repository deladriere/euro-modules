import processing.io.*;

int REQN = 17;
int AO = 22;
int WN = 4;


I2C i2c;
byte data;

int heures;

void setup() {
  printArray(I2C.list());
  i2c = new I2C(I2C.list()[0]);
  delay(100);
  DS1077_init(18); // initiate clock, thus pitch
  MCP23008_WriteIODIR(0x0); // set Data bus as output
  GPIO.pinMode(REQN, GPIO.INPUT_PULLUP);
  GPIO.pinMode(AO, GPIO.OUTPUT);
  GPIO.pinMode(WN, GPIO.OUTPUT);

  INIT();
  FIN();

  Say(spATTENTION);
  Say(spSVP);
}




void draw() {

  delay(2000);
  heures=hour();

  dire(heures);
  if (heures== 2 || heures== 3 || heures== 6|| heures== 10) Say(spZ);
  if (heures== 18) Say(spT);
  Say(spHEURE);

  dire(minute());
  Say(spMINUTE);

  dire(second());
  Say(spSECONDE);
}


void dire(int valeur)
{
  if (valeur < 21)
  {

    switch (valeur) {
    case 0:
      Say(spZERO);
      break;
    case 1:
      Say(spUNE);
      break;
    case 2:
      Say(spDEUX);
      break;
    case 3:
      Say(spTROIS);
      break;
    case 4:
      Say(spQUATRE);
      break;
    case 5:
      Say(spCINQ);
      break;
    case 6:
      Say(spSIX);
      break;
    case 7:
      Say(spSEPT);
      break;
    case 8:
      Say(spHUIT);
      break;
    case 9:
      Say(spNEUF);
      break;
    case 10:
      Say(spDIX);
      break;
    case 11:
      Say(spONZE); 
      Say(spZ);
      break;
    case 12:
      Say(spDOUZE);
      break;
    case 13:
      Say(spTREIZE);
      break;
    case 14:
      Say(spQUATORZ);
      break;
    case 15:
      Say(spQUINZE);
      break;
    case 16:
      Say(spSEIZE);
      break;
    case 17:
      Say(spDIXSEPT);
      break;
    case 18:
      Say(spDIXHUIT);
      break;
    case 19:
      Say(spDIXNEUF);
      break;
    case 20:
      Say(spVINGT);
      Say(spT);
      break;
    }
  } else if (valeur < 30)
  {
    Say(spVINGT);
    if (valeur == 21)
    {
      Say(spTET); 
      Say(spUNE);
    }

    if (valeur == 22)
    {
      Say(spT); 
      Say(spDEUX);
    }

    if (valeur >22)
    {

      dire(valeur - 20);
    }
  } else if (valeur < 40)
  {
    Say(spTRENTE);
    if (valeur == 31)
    {
      Say(spTET); 
      Say(spUNE);
    }
    if (valeur >31)
    {

      dire(valeur - 30);
    }
  } else if (valeur < 50)
  {
    Say(spQUARANTE);
    if (valeur == 41)
    {
      Say(spTET); 
      Say(spUNE);
    }
    if (valeur >41)
    {

      dire(valeur - 40);
    }
  } else if (valeur < 60)
  {
    Say(spCINQUANTE);
    if (valeur == 51)
    {
      Say(spTET); 
      Say(spUNE);
    }
    if (valeur >51)
    {

      dire(valeur - 50);
    }
  }
}
