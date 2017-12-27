
#include "MyTimer.h"

CMyTimer::CMyTimer()
{
    m_ulStartTime = 0;
    m_ulCycleTime = 0;
    m_pFun = NULL;
}
void CMyTimer::Set(unsigned long ulTime, void(*f)())
{
    
    if(f == NULL)
        return;

    m_ulCycleTime = ulTime;
    m_pFun = f;
}


void CMyTimer::Start()
{
    m_ulStartTime = millis();
}

void CMyTimer::LoopTimer()
{
    unsigned long ulCurTime = millis();
    unsigned long ulDetal = 0;
    if(ulCurTime >= m_ulStartTime)
    {
        ulDetal = ulCurTime - m_ulStartTime;
    }
    else
    {
         ulDetal = ~m_ulStartTime + ulCurTime - 1;
    }

    if((ulDetal >= m_ulCycleTime)&& (m_pFun != NULL))
    {
        m_pFun();
        m_ulStartTime =ulCurTime;
    }
    
}