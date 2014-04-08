#include"CRecordWaveParam.h"
CRecordWaveParam::CRecordWaveParam()
{
	this ->nRate = 16000;	// 16kHz
	this ->nBit = 16;		// 16bit
	this ->nDuration = 10; // 10 Ãë
	this ->nDCvalue = 0;
}
void CRecordWaveParam::Get(int &rate, int &bit, int &duration, int &dcvalue)
{
		rate = this ->nRate;
		bit = this ->nBit;
		duration = this ->nDuration;
		dcvalue = this ->nDCvalue;
}
void CRecordWaveParam::Set(int rate, int bit, int duration, int dcvalue)
{
	this ->nRate = rate;
	this ->nBit = bit;
	this ->nDuration = duration;
	this ->nDCvalue = dcvalue;
}
int CRecordWaveParam::GetnBit()
{
	return this ->nBit;
}
int CRecordWaveParam::GetnDCvalue()
{
	return this ->nDCvalue;
}
int CRecordWaveParam::GetnDuration()
{
	return this ->nDuration;
}
int CRecordWaveParam::GetnRate()
{
	return this ->nRate;
}
CRecordWaveParam::~CRecordWaveParam()
{
}