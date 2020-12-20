void STROBE()
{
  //  delayMicroseconds(2);
  GPIO.digitalWrite(WN, GPIO.LOW);
  GPIO.digitalWrite(WN, GPIO.HIGH);
}
void TREQ()

{
  while (GPIO.digitalRead(REQN)==GPIO.HIGH);
}

void STOP()

{
  GPIO.digitalWrite(AO, GPIO.HIGH);

  // slow stop
  MCP23008_writeGPIO(0x1B);

  // holding note
  //  MCP23008_writeGPIO(0x1F);

  STROBE();
}


void INIT()
{
  STOP();
  TREQ();
  GPIO.digitalWrite(AO, GPIO.LOW);
  delay(2); // 2needed
  // pitch
  MCP23008_writeGPIO(0x34);
  delay(2); // delay(2) needed
  TREQ();   // ***
  STROBE();
  TREQ();
}

void FIN()
{


  MCP23008_writeGPIO(0x0);
  STROBE();
  TREQ();


  MCP23008_writeGPIO(0x0);
  STROBE();
  TREQ();


  MCP23008_writeGPIO(0x0);
  STROBE();
  TREQ();

  MCP23008_writeGPIO(0x0);
  STROBE();
  TREQ();

  STOP();
}

void Say(int [] a)
{

        INIT();

        int c;
        int l;

      

        l = a[0]; // Number of byte to read

     

        for (int i = 1; i <=l; i++)
        {
                c = a[i];

                MCP23008_writeGPIO(c);
                STROBE();
                TREQ();
                       }

        FIN();
}
