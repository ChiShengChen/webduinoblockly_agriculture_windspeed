#ifndef __COMMON_H__

#define __COMMON_H__
#include "arduino.h"
#include "IRRecvCounter.h"
#define ARRAY_SIZE( ARRAY ) (sizeof (ARRAY) / sizeof (ARRAY[0]))  

//===============================GPIO Config============================
#define DEF_A0_SOIL				(A0) //土壤
#define DEF_A1_WIND             (A1) //风速
#define DEF_A2_UV               (A2) //UV
#define DEF_D4_RAIN				(4)  //雨量
#define DEF_D6_LED				(6)  //LED 继电器

#define DEF_D5_RESERVE			(5)  //reserve

#define DEF_D8_IR				(8)
#define DEF_D9_IR				(9)
#define DEF_D10_IR				(10)
#define DEF_D11_IR				(11)
#define DEF_D12_IR				(12)
#define DEF_D13_IR				(13)
#define DEF_A3_IR				(A3)
#define DEF_A4_IR				(A4)
#define DEF_A5_IR				(A5)


#define DEF_D3_I2C_HUM			(0xB0)
#define DEF_D3_I2C_TEMP			(0xB1)
#define DEF_D3_I2C_PRESS		(0xB2)

#define DEF_NO1_IR_SUM			(0xB5) //D8 D9 D10
#define DEF_NO2_IR_SUM			(0xB6)  //D11 D12 D13
#define DEF_NO3_IR_SUM			(0xB7)
//======================================================================
/*
extern CIRRecvCounter g_IRRecvD8 ;
extern CIRRecvCounter g_IRRecvD9 ;
extern CIRRecvCounter g_IRRecvD10;
extern CIRRecvCounter g_IRRecvD11;
extern CIRRecvCounter g_IRRecvD12;
extern CIRRecvCounter g_IRRecvD13;
extern CIRRecvCounter g_IRRecvA3 ;
extern CIRRecvCounter g_IRRecvA4 ;
extern CIRRecvCounter g_IRRecvA5 ;

*/
float GetAnalogTypeSensorVal(int nPin);

void Delay(unsigned long ulTime);

float dataMap(int nValue, float fInMin, float fInMax, float fOutMin, float fOutMax);


void showVerByD13LED(int nMajor, int nMinor);

/*
	switch float to char buf
*/
void ExFloatToString(float fSrc, char *pDestBuf, int &nBufLen);

float GetSoilValByADC(int nPinNum);

float GetWindValByADC(int nPinNum);

float GetUvValByADC(int nPinNum);

void __printf(const char *format, ...);
#endif