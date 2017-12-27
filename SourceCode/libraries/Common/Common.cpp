#include "Common.h"

float GetSoilValByADC(int nPinNum)
{
    int nVal = 0;
    float fVol = 0;
    nVal = analogRead(nPinNum);
    fVol = dataMap(nVal, 0, 1023, 0, 5);
    return fVol / 3.3 * 100;
}

float GetWindValByADC(int nPinNum)
{
    int nVal = 0;
    float fVol = 0;
    nVal = analogRead(nPinNum);
    fVol = dataMap(nVal, 0, 1023, 0, 5);
	float fTemp = (fVol - 0.4) / 1.6 * 32.4;
	if(fTemp < 0) fTemp = 0;
    return fTemp;
}

float GetUvValByADC(int nPinNum)
{
#define DEF_READ_COUNT    (8)
#define DEV_UV_MIN        (0)
#define DEV_UV_MAX		  (11)
    int nVal = 0;
	//long sum = 0;
	float fVol = 0;
	float fSum = 0;
	for(int i = 0; i < DEF_READ_COUNT; i++)
	{
		nVal = analogRead(nPinNum);
		fVol = dataMap(nVal, 0, 1023, 0, 5);
		fSum += fVol;
		delay(2);
    }
	
	
	
	float fAvgVal = fSum / DEF_READ_COUNT;
//	__printf("UV Level avg = %d\n",  meanVal);
	float fValTemp = (fAvgVal * 1000 / 4.3 - 83) / 21;
	
	if(fValTemp < DEV_UV_MIN)
		fValTemp = DEV_UV_MIN;
	
	if(fValTemp > DEV_UV_MAX)
		fValTemp = DEV_UV_MAX;
		
		nVal = (int)(fValTemp + 0.5);
	return (float)nVal;
}


float GetAnalogTypeSensorVal(int nPin)
{

    float fSensorVal = 0.0;
    switch(nPin)
    {
    case DEF_A0_SOIL:
        fSensorVal = GetSoilValByADC(DEF_A0_SOIL);
        /*
            nVal = analogRead(DEF_A0_SOIL);
            fVol = dataMap(nVal, 0, 1023, 0, 5);
            fSensorVal = GetSoilValByVol(fVol);
        	*/
        break;

    case DEF_A1_WIND:
        /*
            nVal = analogRead(DEF_A1_WIND);
            fVol = dataMap(nVal, 0, 1023, 0, 2);

            fSensorVal = GetWindValByVol(fVol);
        	*/
        fSensorVal = GetWindValByADC(DEF_A1_WIND);
        break;

    case DEF_A2_UV:
        /*
            nVal = analogRead(DEF_A2_UV);
            fSensorVal = (nVal * 1000 / 4.3 - 83) / 21;

            fVol = dataMap(nVal, 0, 1023, 0, 5);
            fSensorVal = (307 * fVol) / 200;
            */
        fSensorVal = GetUvValByADC(DEF_A2_UV);
        break;
    default:
        fSensorVal = 0.0;
        break;
    }

    return fSensorVal;

}


float dataMap(int nValue, float fInMin, float fInMax, float fOutMin, float fOutMax)
{

    if(fInMax == fInMin)
        return 0.0;
    float fParam = (fOutMax - fOutMin) / (fInMax - fInMin);
    return fParam * nValue + (fOutMin - fParam *  fInMin);
}


void Delay(unsigned long ulTime)
{
    unsigned long startTime = millis();
    int i = 0;
    __printf("-------------Waiting 30s------------------------\n");
    while((millis() - startTime) < (ulTime))
    {
        __printf("%d.", i++);
        
        delay(1000);
    }
   __printf("\n-------------Start Running, Go,Go------------------------\n");
}


/*
void showVerByD13LED(int nMajor, int nMinor)
{
    pinMode(13, OUTPUT);
    for(int i = 0 ; i < nMajor; i++)
    {
        digitalWrite(13, HIGH);
        delay(3000);
        digitalWrite(13, LOW);
        delay(1000);
    }

    for(int i = 0; i < nMinor; i++)
    {

        digitalWrite(13, HIGH);
        delay(1000);
        digitalWrite(13, LOW);
        delay(1000);
    }
    Serial.println("");
    Serial.print("Version: V");
    Serial.print(nMajor);
    Serial.print(".");
    Serial.print(nMinor);
    Serial.println("");
    pinMode(13, INPUT);

}
*/
void ExFloatToString(float fSrc, char *pDestBuf, int &nBufLen)
{
    if(pDestBuf == NULL)
        return  ;

    sprintf(pDestBuf, "%.2f", fSrc);
    nBufLen = strlen(pDestBuf);


}

void __printf(const char *format, ...)  
{  
    va_list arg;  
    va_start(arg, format);  
    while (*format)  
    {  
        char ret = *format;  
        if (ret == '%')  
        {  
            switch (*++format)  
            {  
            case 'c':
            case 'C':  
                {  
                    char ch = (char)va_arg(arg, int);  
                    Serial.print(ch);  
                    break;  
                }  
            case 's':  
			case 'S':
                {  
                    char *pc = va_arg(arg, char *); 
                    Serial.print(pc); 
					/*
                    while (*pc)  
                    {  
                        pc++;  
                    }  */
                    break;  
                } 
            case 'd':
            case 'D':
                {
                    int nVal = va_arg(arg, int);  
                    Serial.print(nVal, DEC); 
                }
                break;
                case 'x':
                {
                      int nVal = va_arg(arg, int);  
                      Serial.print(nVal, HEX); 
                }
                break;
				case 'f':
				case 'F':
				{
					float fVal = (float)va_arg(arg, double);  
					Serial.print(fVal);
				}
				break; 
            default:  
                break;  
            }  
        }  
        else  
        {  
            Serial.print(*format);
        }  
        format++;  
    }  
    va_end(arg);  
}  

