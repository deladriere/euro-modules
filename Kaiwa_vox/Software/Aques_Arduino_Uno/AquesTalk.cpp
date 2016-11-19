/*
  AquesTalk.cpp - AquesTalk pico LSI library for Arduino

  本ライブラリは TwoWire Class(I2C)を使用しているため、
  スケッチ上でライブラリ「Wire」を別途追加してください。

  Copyright (c) 2012 AQUEST Corp.  All right reserved.

  Ver.1.1	2015/02/16	ArduinoIDEで"prog_char" が未定義になったので修正

  バグ等ありましたら、infoaq@a-quest.com まで連絡いただけると幸いです

  This file is free software; you can redistribute it and/or modify
  it under the terms of either the GNU General Public License version 2
  or the GNU Lesser General Public License version 2.1, both as
  published by the Free Software Foundation.

  バグ等ありましたら、infoaq@a-quest.com まで連絡いただけると幸いです
*/

extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}
#include "AquesTalk.h"
#include <Wire.h>
//#include "../Wire/Wire.h"

const char PSTR_CR[] PROGMEM = "\r";


AquesTalk::AquesTalk(int addr)
{
	m_addr = addr;
	Wire.begin();	// 2度呼び出しても大丈夫？
}

AquesTalk::~AquesTalk()
{
}


// LSIの作動チェック
//	LSIから応答が得られるかチェックする。
//	falseが返る場合は、配線や電源等を確認する
// return
//		true:作動中 false:応答なし
bool AquesTalk::IsActive()
{
	if(getOne()==0)	return false;	// NOACK
	return true;
}



// 音声合成開始	引数に音声記号列を指定
// 最後に"\r"を送信
// Readyチェックしてから、コマンドを送信している
void AquesTalk::Synthe(const char *msg)
{
while(IsBusy()) ;
	Write(msg);
	WriteP(PSTR_CR);
}

void AquesTalk::SyntheP(const char *msg)
{
	while(IsBusy()) ;
	WriteP(msg);
	WriteP(PSTR_CR);
}
// max 128byte
void AquesTalk::SyntheS(String &strMsg)
{
    char msg[128];
    strMsg.toCharArray(msg, 128);
	Synthe(msg);
}


// LSI にコマンド送信
void AquesTalk::Write(const char *msg)
{
	// Wireの制約で、一度に送れるのは32byteまで
	// AquesTalk picoへは一度に128byteまで送れるので、
	// Wire.beginTransmission()～Wire.endTransmission()を複数回に分けて呼び出す
	const char *p = msg;
	for(;*p!=0;){
		Wire.beginTransmission(m_addr);
		// Wireの制約で、一度に送れるのは32byteまで
		for(int i=0;i<32;i++){
			Wire.write(*p++);
			if(*p==0) break;
		}
		Wire.endTransmission(); // 実際はこのタイミングで送信される
	}
}

void AquesTalk::WriteP(const char *msg)
{
	const char *p = msg;
	for(;pgm_read_byte(p)!=0;){
		Wire.beginTransmission(m_addr);
		// Wireの制約で、一度に送れるのは32byteまで
		for(int i=0;i<32;i++){
			uint8_t c = pgm_read_byte(p++);
			Wire.write(c);
			if(pgm_read_byte(p)==0) break;
		}
		Wire.endTransmission(); // 実際はこのタイミングで送信される
	}
}

// LSIが発声中などBUSY状態にあるかチェック
// 	BUSY中はコマンドを送信できない(Breakを除く)
// return
//		true:BUSY false:コマンド送信可能
//	【注意】応答なしの場合もfalseが返る
bool AquesTalk::IsBusy()
{
	uint8_t c = getOne();
	if(c=='*' || c==0xFF) {
		delay(10); // Busy応答は10msec以上待つ必要がある 連続して呼ばれた場合のため
		return true;
	}
	return false;
}

// LSIの応答を返す
// return:
//	>100 :	エラーコード
// 	0:正常終了
// 	2:応答なし
// 	3:上記以外
uint8_t AquesTalk::GetResp()
{
	char	str[6];
	getResponse(str);
	return resp2bin(str);
}

// 発声を中断する
void AquesTalk::Break()
{
	WriteP(PSTR("$"));
}

// 発話速度を変更
//	speed: 50-300  default:100 50:最遅　300:最速
void AquesTalk::SetSpeed(uint16_t speed, uint8_t *pRet)
{
	if(speed<50)	speed=50;
	else if(speed>300)	speed=300;
	SetRom(0x002, (uint8_t)(speed&0xFF), pRet);
	if(pRet!=0 && *pRet!=0)	return;
	SetRom(0x003, (uint8_t)(speed/256), pRet);
	if(pRet!=0 && *pRet!=0)	return;
}

// 発話速度を取得
uint16_t AquesTalk::GetSpeed(uint8_t *pRet)
{
	uint16_t speed = GetRom(0x002, pRet);
	if(pRet!=0 && *pRet!=0)	return 0;
	speed += GetRom(0x003, pRet)*256;
	if(pRet!=0 && *pRet!=0)	return 0;
	return speed;
}

// バージョンを取得
// str[6](return)	"VF1a" など
void AquesTalk::GetVersion(char *str)
{
	if(str==0) return;
	SyntheP(PSTR("#V"));
	getResponse(str);
	if(str[4]=='>') str[4]=0;
}

// EEPROMに1byte書き込み
//	adr:	EEPROMアドレス(0-0x3FF)
//	val:	設定値
void AquesTalk::SetRom(uint16_t adr, uint8_t val, uint8_t *pRet)
{
	char str[8];
	byte2hex((uint8_t)(adr>>8), str+1);
	str[0] = '#';
	str[1] = 'W';
	byte2hex((uint8_t)(adr&0xFF), str+3);
	byte2hex(val, str+5);
	Synthe(str);	// "#Wnnnmm"
	if(pRet) {
		getResponse(str);	// ">" "Ennn>"
		*pRet = resp2bin(str);
	}
}

// EEPROMから1byte読み出し
//	adr:	EEPROMアドレス(0-0x3FF)
//	return:	値(0-255)
//	【注意】エラー時にも255が返る
//
uint8_t AquesTalk::GetRom(uint16_t adr, uint8_t *pRet)
{
	char str[6];
	byte2hex((uint8_t)(adr>>8), str+1);
	str[0] = '#';
	str[1] = 'R';
	byte2hex((uint8_t)(adr&0xFF), str+3);
	Synthe(str);	// "#Rnnn"
	getResponse(str);	// "nn>" "Ennn>"
	if(str[2]!='>') {
		if(pRet)	*pRet = resp2bin(str);
		return 0xFF;	// error
	}
	if(pRet)	*pRet=AQTK_READY;
	return hex2byte(str);
}

///////////////////////////////////////////
// ATP3011R4 Only
///////////////////////////////////////////

// アクセントの強さを変更
//	accent: 0-255  default:64 0:なし 255:強い
void AquesTalk::SetAccent(uint8_t accent, uint8_t *pRet)
{
	SetRom(0x03C, accent, pRet);
}
// アクセントの強さを取得
uint8_t AquesTalk::GetAccent(uint8_t *pRet)
{
	return GetRom(0x03C, pRet);
}
// 声の高さを変更
//	pitch: 0-255  default:64 0:高い 254:低い
void AquesTalk::SetPitch(uint8_t pitch, uint8_t *pRet)
{
	SetRom(0x03D, pitch, pRet);
}
// 声の高さを取得
uint8_t AquesTalk::GetPitch(uint8_t *pRet)
{
	return GetRom(0x03D, pRet);
}

/*----------------------------------------------
	PRIVATE FUNCTION
-----------------------------------------------*/

// 1文字取得
// return
//	0			:	NOACK または応答が無い。I2Cの配線をチェックすべし
//  '*'/0xFF	:	busy
//  '>'			:	ready
//	その他ASCII	:	エラーなど各種コマンド応答
uint8_t AquesTalk::getOne()
{
	Wire.requestFrom(m_addr, 1);
	if(Wire.available()>0){
		uint8_t c = Wire.read();
		return c;
	}
	else {
		//ERR: NOACK または応答が無い。I2Cの配線をチェックすべし
		return 0;
	}
}

// コマンド応答を取得 (同期型)
// str[6]	">" "E105>" "VF1a>" "CC>" ""
void AquesTalk::getResponse(char *str)
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
				return;	// Ready応答
			}
			else if('0'<=c && c<='z'){
				str[cnt++] = c;
			}
			else {
				delay(10); 	// busy応答 '*' 0xFF
			}
		}
	}
	str[cnt] = 0;
	return;	// OTHER
}

// コマンド応答をバイナリーコードに変換
uint8_t AquesTalk::resp2bin(const char *str)
{
	if(str==0)		return AQTK_OTHER;
	if(str[0]==0)	return AQTK_NOACK;
	if(str[0]=='>')	return AQTK_READY;
//	if(str[0]=='*')	return AQTK_BUSY;	// strには'*'が入ることは無い
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

void AquesTalk::byte2hex(uint8_t val8, char *str)
{
	uint8_t val = val8>>4;
	*str++ = val<10?val+'0':val-10+'A';
	val = val8&0x0f;
	*str++ = val<10?val+'0':val-10+'A';
	*str = 0;
}

uint8_t AquesTalk::hex2byte(const char *str)
{
	uint8_t	cc;
	uint8_t val;

	cc = str[0];
	if('0'<=cc && cc<='9')		val = cc - '0';
	else if('A'<=cc && cc<='F') val = cc - 'A'+10;
	else return 0;

	val = val<<4;

	cc = str[1];
	if('0'<=cc && cc<='9')		val += cc - '0';
	else if('A'<=cc && cc<='F') val += cc - 'A'+10;
	else return 0;
	return val;
}
