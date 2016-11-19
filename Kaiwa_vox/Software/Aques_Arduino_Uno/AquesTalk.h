/*
  AquesTalk.h - AquesTalk pico LSI library for Arduino
  
  本ライブラリは TwoWire Class(I2C)を使用しているため、
  スケッチ上でライブラリ「Wire」を別途追加してください。
  
  Copyright (c) 2012 AQUEST Corp.  All right reserved.

  Ver.1.1	2015/02/16	ArduinoIDEで"prog_char" が未定義になったので修正
  
  This file is free software; you can redistribute it and/or modify
  it under the terms of either the GNU General Public License version 2
  or the GNU Lesser General Public License version 2.1, both as
  published by the Free Software Foundation.
 */

#ifndef AquesTalk_h
#define AquesTalk_h

#include <Arduino.h>

#define AQTK_I2C_ADDR 0x2E // AquesTalk pico LSI I2Cアドレス（出荷時設定）

#define	AQTK_READY	0
#define	AQTK_NOACK	2
#define	AQTK_OTHER	3

class AquesTalk
{
public:
	AquesTalk(int addr=AQTK_I2C_ADDR);               // create instance. addr:I2C address
	~AquesTalk();

	bool	IsActive();
	void	Synthe(const char *msg);
	void	SyntheP(const char *msg);
	void	SyntheS(String &strMsg);
	void	Write(const char *msg);
	void 	WriteP(const char *msg);
	bool	IsBusy();
	uint8_t	GetResp();
	void	Break();
	void	SetSpeed(uint16_t speed, uint8_t *pRet=0);
	uint16_t GetSpeed(uint8_t *pRet=0);
	void	GetVersion(char *str);
	void	SetRom(uint16_t adr, uint8_t val, uint8_t *pRet=0);
	uint8_t GetRom(uint16_t adr, uint8_t *pRet=0);

	// ATP3011R4 only
	void	SetPitch(uint8_t pitch, uint8_t *pRet=0);
	void	SetAccent(uint8_t accent, uint8_t *pRet=0);
	uint8_t GetPitch(uint8_t *pRet=0);
	uint8_t GetAccent(uint8_t *pRet=0);

private:
	int 	m_addr;             // i2c address

	uint8_t	getOne();
	void	getResponse(char *str);
	uint8_t	resp2bin(const char *str);
	void	byte2hex(uint8_t val8, char *str);
	uint8_t	hex2byte(const char *str);
};

#endif
