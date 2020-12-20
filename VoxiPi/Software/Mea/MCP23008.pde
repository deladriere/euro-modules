byte _ADDR=0x20;
byte regIODIR=0x0;
byte regGPIO=0x9;
// regIODIR      0x00  IO Direction Register
// regGPIO       0x09   General Purpose IO Register

// writeIODIR
//------------------------------------------------------------------------------
// Writes to the MCP23008 IODIR register.
//
// Parameter  Description
//------------------------------------------------------------------------------
// cIODIR    Data direction value (0=output; 1=input)
//
//        BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0
//        IO7 |IO6 |IO5 |IO4 |IO3 |IO2 |IO1 |IO0
  

void MCP23008_WriteIODIR(int cIODIR)
{
  MCP23008_writebyte(_ADDR, regIODIR, cIODIR);
}

// writeGPIO
//------------------------------------------------------------------------------
// Writes to the MCP23008 GPIO register.  
//
// Parameter  Description
//------------------------------------------------------------------------------
// cGPIO    Port Register (0=Logic Low; 1=Logic High)
//
//        BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0
//        GP7 |GP6 |GP5 |GP4 |GP3 |GP2 |GP1 |GP0  

void MCP23008_writeGPIO(int cGPIO)
{
  MCP23008_writebyte(_ADDR, regGPIO, cGPIO);
}

void MCP23008_writebyte(int Address, int Register, int Value)
{
  i2c.beginTransmission(Address);
  i2c.write(Register);
  i2c.write(Value);
  i2c.endTransmission();
}
