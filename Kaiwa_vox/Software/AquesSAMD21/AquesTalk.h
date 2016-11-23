#include <Arduino.h>
#include <Wire.h>

#define I2C_ADDR_AQUESTALK 0x2E

uint8_t getOne()
{
  Wire.requestFrom(I2C_ADDR_AQUESTALK, 1);
  if(Wire.available()>0){
    uint8_t c = Wire.read();
    return c;
  }
  else {
    //ERR: NOACK または応答が無い。I2Cの配線をチェックすべし
    return 0; 
  }
}

bool IsBusy()
{
  uint8_t c = getOne();
  if(c=='*' || c==0xFF) {
    delay(10); // Busy応答は10msec以上待つ必要がある 連続して呼ばれた場合のため
    return true;
  }
  return false;
}


void Write(const char *msg)
{
  // Wireの制約で、一度に送れるのは32byteまで
  // AquesTalk picoへは一度に128byteまで送れるので、
  // Wire.beginTransmission()～Wire.endTransmission()を複数回に分けて呼び出す
while(digitalRead(12)==0); //dirty fix !! must work with I2C !! 
  const char *p = msg;
  for(;*p!=0;){
    Wire.beginTransmission(I2C_ADDR_AQUESTALK);
    // Wireの制約で、一度に送れるのは32byteまで
    for(int i=0;i<32;i++){
      Wire.write(*p++);
      if(*p==0) break;
    }
     
  }

}

void WriteP(const char *msg)
{
  const char *p = msg;
  for(;pgm_read_byte(p)!=0;){
    Wire.beginTransmission(I2C_ADDR_AQUESTALK);
    // Wireの制約で、一度に送れるのは32byteまで
    for(int i=0;i<32;i++){
      uint8_t c = pgm_read_byte(p++);
      Wire.write(c);
      if(pgm_read_byte(p)==0) break;
    }
    Wire.endTransmission(); // 実際はこのタイミングで送信される
  }
}

void Synthe(const char *msg)
{
  while(IsBusy()) ; // ok fonctionne mais petit delais entre les Synthe ?
  Write(msg);
  //WriteP(PSTR_CR);
 Wire.write("\r");
  Wire.endTransmission(); // 実際はこのタイミングで送信される
}

