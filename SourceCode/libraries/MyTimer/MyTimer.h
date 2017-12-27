#ifndef __MY_TIMER_H__
#define __MY_TIMER_H__

#include "arduino.h"
/*
	How To Used:
	1. Create Instance 
	2. Call Set Function to set Cycle time and callback function
	3. add LoopTimer to Loop()

*/


class CMyTimer
{

private:
    unsigned long m_ulStartTime;
    unsigned long m_ulCycleTime;
    void (*m_pFun)();

public:
    CMyTimer();
    void Set(unsigned long ulTime,  void (*f)());
    void Start();

    void LoopTimer();


};


#endif