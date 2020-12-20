byte MH_bit = 0;
byte ML_bit = 0;



void DS1077_init(int divider)
{
  DS1077_write(0x2,0x18,0x0); // init DS1077 prescaler
  delay(20);
  DS1077(divider);
}

void DS1077_write(int address, int val1, int val2)
{
     i2c.beginTransmission(0x58); //start transmission to device
     i2c.write(address);        // send register address
     i2c.write(val1);        // send value to write
     i2c.write(val2);        // send value to write
     i2c.endTransmission(); //end transmission
}

void DS1077(int divider) {
 DS1077_write(0x01, (divider-2)>>2, ((divider-2)<<6)&0xC0); //div
}
