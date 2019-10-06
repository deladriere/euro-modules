#define MH_bit 0
#define ML_bit 0

/*
to set the prescaler:

1 MH_bit 0 ML_bit 0
2 MH_bit 0 ML_bit 1
4 MH_bit 1 ML_bit 0
8 MH_bit 1 ML_bit 1
*/



void DS1077_write(byte address, byte val1, byte val2) {
     Wire.beginTransmission(0x58); //start transmission to device
     Wire.write(address);        // send register address
     Wire.write(val1);        // send value to write
     Wire.write(val2);        // send value to write
     Wire.endTransmission(); //end transmission
}

void DS1077(int divider) {
  DS1077_write(0x01, (divider-2)>>2, ((divider-2)<<6)&0xC0); //div
}

void DS1077_init(int divider)
{
  DS1077_write(0x02, 0b00011000|MH_bit, 0b00000000|(ML_bit<<7)); // init DS1077 prescaler
  delay(20);
  DS1077(divider);
}
