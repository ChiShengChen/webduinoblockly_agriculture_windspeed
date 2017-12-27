
#include "RainUtils.h"


CRainUtils::CRainUtils(int nPin)
{
    pinMode(nPin, INPUT_PULLUP);
    m_ulRainfallCount = 0;
    m_nPinPrevStatus = HIGH;
    m_bIsLToH = false;
    m_nPin = nPin;
}

void CRainUtils::DetectRainfall(ul32 ulTime)
{
    delay(ulTime);
    int nPinCurStatus = digitalRead(m_nPin);

    if(m_nPinPrevStatus != nPinCurStatus)
    {
        m_nPinPrevStatus = nPinCurStatus;
        m_nPinPrevStatus == LOW ? m_bIsLToH = false : m_bIsLToH = true;
    }
    else
    {
        return;
    }

    if(m_bIsLToH)
	{
        m_ulRainfallCount++;
		
	#if DEF_RAIN_UTILS_DEBUG
		__printf(" [Rainfall count] = %d\n", m_ulRainfallCount);
	#endif
	}

}

ul32 CRainUtils::GetRainfall()
{

	/*#if DEF_RAIN_UTILS_DEBUG
		Serial.print(" Rainfall = ");
		Serial.println(m_ulRainfallCount * 0.2);
	#endif*/
    return m_ulRainfallCount * 0.2;

}

void CRainUtils::ResetRainfall()
{
	m_ulRainfallCount = 1;
}