#ifndef __RAIN_UTILS_H__
#define __RAIN_UTILS_H__
#include "arduino.h"
#include "Common.h"
typedef unsigned long  	ul32;


#define DEF_RAIN_UTILS_DEBUG    1


class CRainUtils
{

private:
 int m_nPinPrevStatus;
 bool m_bIsLToH;
 ul32 m_ulRainfallCount;  
 int m_nPin;
public:
	CRainUtils(int nPin);
	
	void DetectRainfall(ul32 ulTime = 0);
	
	ul32 GetRainfall();
	
	void ResetRainfall();
};


#endif