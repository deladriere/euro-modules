#include <Arduino.h>
#include <Wire.h>

#define I2C_ADDR_AQUESTALK 0x2E


#define  AQTK_READY  0
#define AQTK_NOACK  2
#define AQTK_OTHER  3

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
    delayMicroseconds(10); // Busy応答は10msec以上待つ必要がある 連続して呼ばれた場合のため
    return true;
  }
  return false;
}


void Write(const char *msg)
{
  // Wireの制約で、一度に送れるのは32byteまで
  // AquesTalk picoへは一度に128byteまで送れるので、
  // Wire.beginTransmission()～Wire.endTransmission()を複数回に分けて呼び出す
//while(digitalRead(12)==0); //dirty fix !! must work with I2C !!
  const char *p = msg;
  for(;*p!=0;){
    Wire.beginTransmission(I2C_ADDR_AQUESTALK);
    // Wireの制約で、一度に送れるのは32byteまで
    for(int i=0;i<50;i++){ //was 32 seems to be working with 50
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


void byte2hex(uint8_t val8, char *str)
{
  uint8_t val = val8>>4;
  *str++ = val<10?val+'0':val-10+'A';
  val = val8&0x0f;
  *str++ = val<10?val+'0':val-10+'A';
  *str = 0;
}

void Break()
{
    Wire.beginTransmission(I2C_ADDR_AQUESTALK);
  Wire.write("$");
//  Wire.write("\r");
  Wire.endTransmission();
  delay(1);
}

void Synthe(const char *msg)
{
  //while(IsBusy()) ; // working when checking => priority to speech not going to next step but doesn't reset speech - break Needed
 Break();
  Write(msg);
//WriteP(PSTR_CR); // not working why ?
  Wire.write("\r");
  Wire.endTransmission(); // 実際はこのタイミングで送信される
}

void getResponse(char *str)
{
  uint8_t cnt;
  for(cnt=0;cnt<5;){
    uint8_t c = getOne();
    if(c==0){
      str[cnt] = 0;
      return; //ERR: NOACK または応答が無い。
    }
    else {
      if(c=='>') {
        str[cnt++] = c;
        str[cnt]   = 0;
        return; // Ready応答
      }
      else if('0'<=c && c<='z'){
        str[cnt++] = c;
      }
      else {
        delayMicroseconds(10000);  // busy応答 '*' 0xFF
      }
    }
  }
  str[cnt] = 0;
  return; // OTHER
}

uint8_t resp2bin(const char *str)
{
  if(str==0)    return AQTK_OTHER;
  if(str[0]==0) return AQTK_NOACK;
  if(str[0]=='>') return AQTK_READY;
//  if(str[0]=='*') return AQTK_BUSY; // strには'*'が入ることは無い
  if(str[0]=='E') { // "Ennn>" エラーコードをバイナリにして返す
    if( ('0'<=str[1]&&str[1]<='9')
     && ('0'<=str[2]&&str[2]<='9')
     && ('0'<=str[3]&&str[3]<='9') ){
      uint8_t iret;
      iret = str[1]-'0';
      iret = iret*10 + str[2]-'0';
      iret = iret*10 + str[3]-'0';
      return iret;
    }
  }
  return AQTK_OTHER;
}

void SetRom(uint16_t adr, uint8_t val, uint8_t *pRet)
{
  char str[8];
  byte2hex((uint8_t)(adr>>8), str+1);
  str[0] = '#';
  str[1] = 'W';
  byte2hex((uint8_t)(adr&0xFF), str+3);
  byte2hex(val, str+5);
  Synthe(str);  // "#Wnnnmm"
  if(pRet) {
    getResponse(str); // ">" "Ennn>"
    *pRet = resp2bin(str);
  }
}
//  pitch: 0-255  default:64 0:高い 254:低い
void SetPitch(uint8_t pitch, uint8_t *pRet=0)
{
  SetRom(0x03D, pitch, pRet);
}
//accent: 0-255  default:64 0:なし 255:強い
void SetAccent(uint8_t accent, uint8_t *pRet=0)
{
  SetRom(0x03C, accent, pRet);
}

//  speed: 50-300  default:100 50:最遅　300:最速
void SetSpeed(uint16_t speed, uint8_t *pRet=0)
{
  if(speed<50)  speed=50;
  else if(speed>300)  speed=300;
  SetRom(0x002, (uint8_t)(speed&0xFF), pRet);
  if(pRet!=0 && *pRet!=0) return;
  SetRom(0x003, (uint8_t)(speed/256), pRet);
  if(pRet!=0 && *pRet!=0) return;
}
