#include "RainUtils.h"
//#include "MsTimer2.h"
#include "IRRecvCounter.h"
#include "Common.h"
#include "Seeed_BME280.h"
#include "MyTimer.h"
#include "Command.h"

#define DEF_MAIN_DEBUG                  1   //for debug , print more log msg
#define DEF_ENABLE_BME280               1   // BME280 function 
#define DEF_ENABLE_TIMERLED		          1   // TIMERLED  for D6 LED function
#define DEF_ENABLE_IR                   1   // for ir conter
#define DEF_ENABLE_ANALOG_TYPE          1   // for A0, A1, A2, ADC sensor, UV, WIND, SOIL
#define DEF_ENABLE_BRIDGE               1   // for Bridge function, in/out data from/to server



#define DEF_VERSION                     ("V0.000.06T7")
volatile bool g_bLEDStatus = LOW;
bool g_bIsBME280InitOK = false;
bool g_bIsRegBridgeOK = false;
bool g_bIsRegDevice = false;

CRainUtils g_Rainfall(DEF_D4_RAIN);
BME280 g_BME280;

CMyTimer g_BME280Timer;
CMyTimer g_ADCTimer;
CMyTimer g_ChkRegTimer;
CMyTimer g_LEDTimer;

CIRRecvCounter g_IRRecvD8 (DEF_D8_IR,   DEF_TYPE_DIGITAL);
CIRRecvCounter g_IRRecvD9 (DEF_D9_IR,   DEF_TYPE_DIGITAL);
CIRRecvCounter g_IRRecvD10(DEF_D10_IR,  DEF_TYPE_DIGITAL);
CIRRecvCounter g_IRRecvD11(DEF_D11_IR,  DEF_TYPE_DIGITAL);
CIRRecvCounter g_IRRecvD12(DEF_D12_IR,  DEF_TYPE_DIGITAL);
CIRRecvCounter g_IRRecvD13(DEF_D13_IR,  DEF_TYPE_DIGITAL);
CIRRecvCounter g_IRRecvA3 (DEF_A3_IR,   DEF_TYPE_ANALOG);
CIRRecvCounter g_IRRecvA4 (DEF_A4_IR,   DEF_TYPE_ANALOG);
CIRRecvCounter g_IRRecvA5 (DEF_A5_IR,   DEF_TYPE_ANALOG);
  /*  CIRRecvCounter IRArray[] =
    {
        g_IRRecvD8  ,
        g_IRRecvD9  ,
        g_IRRecvD10 ,
        g_IRRecvD11 ,
        g_IRRecvD12 ,
        g_IRRecvD13 ,
        g_IRRecvA3  ,
        g_IRRecvA4  ,
        g_IRRecvA5  ,
    };*/
void LoopIRCount()
{
  /*
    for(int i = 0; i < (int)ARRAY_SIZE(IRArray); i++)
    {
        IRArray[i].IRRecvCount(0);
    }*/
         g_IRRecvD8.IRRecvCount(0);  
         g_IRRecvD9.IRRecvCount(0);  
        g_IRRecvD10.IRRecvCount(0); 
        g_IRRecvD11.IRRecvCount(0); 
        g_IRRecvD12.IRRecvCount(0); 
        g_IRRecvD13.IRRecvCount(0); 
         g_IRRecvA3.IRRecvCount(0);  
         g_IRRecvA4.IRRecvCount(0);  
         g_IRRecvA5.IRRecvCount(0);  
}

void initTimer()
{
#if DEF_ENABLE_TIMERLED
/*
    MsTimer2::set(5 * 1000,  OnTimerLED);
    MsTimer2::start();
*/
    g_LEDTimer.Set(25 * 1000, OnTimerLED);
    g_LEDTimer.Start();
#endif

#if DEF_ENABLE_BME280
    g_BME280Timer.Set(10 * 1000, OnTimeBME280);
    g_BME280Timer.Start();
#endif


#if DEF_ENABLE_ANALOG_TYPE
    g_ADCTimer.Set(15 * 1000, OnTimeAnalogTypeData);
    g_ADCTimer.Start();
#endif

#if DEF_ENABLE_BRIDGE
  g_ChkRegTimer.Set(20 * 1000, OnTimeCheckRegStatus);
  g_ChkRegTimer.Start();
#endif

}


void setup()
{
    Serial.begin(9600);

#if DEF_MAIN_DEBUG
    Delay(30 * 1000);
#endif


    pinMode(DEF_D6_LED, OUTPUT);

#if DEF_ENABLE_BME280
    if(!g_BME280.init())
	{
        __printf("[ERROR] BME280 Device init error!\n");
		g_bIsBME280InitOK = false;
	}
    else
	{
        __printf("BME 280 Init OK\n");
		g_bIsBME280InitOK = true;
	}
#endif
#if DEF_ENABLE_BRIDGE
  Bridge.begin();  
  __printf("Finish init Bridge ...\n");
#endif

    pinMode(DEF_D6_LED, OUTPUT);
  
	 __printf("%s\n", DEF_VERSION);

    initTimer();
	__printf("finish setup!!!\n");
  
}


void loop()
{
#if DEF_ENABLE_BRIDGE
  g_ChkRegTimer.LoopTimer();
#endif

  
#if DEF_ENABLE_IR
    LoopIRCount();//IR
#endif

#if DEF_ENABLE_BRIDGE
  pollingCmd();
#endif

    g_Rainfall.DetectRainfall();
 
  

#if DEF_ENABLE_BME280
    g_BME280Timer.LoopTimer();
#endif

  
#if DEF_ENABLE_ANALOG_TYPE
    g_ADCTimer.LoopTimer();
#endif
#if DEF_ENABLE_TIMERLED
  g_LEDTimer.LoopTimer();
#endif
}


bool GetBme280TempHumPressure(float &fTemp, float &fHum, float &fPressure)
{
	if(!g_bIsBME280InitOK)
	{
		__printf("[ERROR] BME280 Device init error!");
		g_bIsBME280InitOK = g_BME280.init();
		return false;
	}
	else
	{
		g_bIsBME280InitOK = true;
		
	}
    fTemp = g_BME280.getTemperature();

    fHum = g_BME280.getHumidity();

    fPressure = g_BME280.getPressure();

    if(fPressure == 0) return false;

     __printf("\n---------------------------BME280 Result----------------------------\n");
     __printf(" 1. Temp     = %f C\n", fTemp);
     __printf(" 2. Pressure = %f Pa\n", fPressure);
     __printf(" 3. Altitude = %f m\n", g_BME280.calcAltitude(fPressure));
     __printf(" 4. Humidity = %f % \n", fHum);
     __printf("------------------------------------------------------------------\n");
	 
	 uploadBME280Data( fTemp,  fHum,  fPressure);
    return true;
}


void OnTimerLED()
{
    digitalWrite(DEF_D6_LED, !g_bLEDStatus);
    g_bLEDStatus = !g_bLEDStatus;
    __printf("Set D6 Led To : %d\n", g_bLEDStatus);

    uploadIRData();
    uploadRainData();
}

void OnTimeAnalogTypeData()
{
    float fSoil = GetAnalogTypeSensorVal(DEF_A0_SOIL);  //Soil
    float fWind = GetAnalogTypeSensorVal(DEF_A1_WIND);  //Wind
    float fUV   = GetAnalogTypeSensorVal(DEF_A2_UV);     //UV

 
   
      __printf("---------------------------ADC Result----------------------------\n");
      __printf("  1. fSoil = %f\n", fSoil);
      __printf("  2. fWind = %f\n", fWind);
      __printf("  3. fUV   = %f\n", fUV);
     __printf("------------------------------------------------------------------\n");
	 uploadADCData(fSoil, fWind, fUV);
}

void OnTimeBME280()
{
   float fTemp, fHum, fPressure;
   bool bRet =  GetBme280TempHumPressure(fTemp,  fHum, fPressure);
   if(!bRet) 
      __printf("[ERROR] Get BME280 Temp Hum Pressure Error\n");
}

void OnTimeCheckRegStatus()
{
   __printf("Check Device Register Status\n");
  if(!g_bIsRegDevice && !CheckRegStatus())
  {
    g_bIsRegDevice = false;
    __printf("[ERROR] Register Bridge NG\n");  
  }
  else
   {
     g_bIsRegDevice = true;
    __printf("Resister Device OK\n");
   }
  
}
/*
-------------------------------------------Upload Data Function eara------------------------------------------------------------------

*/
void uploadIRData()
{
  int nIR1Data = 0, nIR2Data = 0, nIR3Data = 0;
  
  nIR1Data = g_IRRecvD11.getCount() +  g_IRRecvD12.getCount() +  g_IRRecvD13.getCount();
  nIR2Data = g_IRRecvD8.getCount() +  g_IRRecvD9.getCount() +  g_IRRecvD10.getCount();
  nIR3Data = g_IRRecvA3.getCount() +  g_IRRecvA3.getCount() +  g_IRRecvA5.getCount();

#if DEF_ENABLE_BRIDGE
  if(g_bIsRegDevice)
  {
    __printf(" Upload IR Data:[IR1, IR2, IR3] = %d, %d, %d\n", nIR1Data, nIR2Data, nIR3Data);
    SendDataToBridge(DEF_NO1_IR_SUM, nIR1Data);
    SendDataToBridge(DEF_NO2_IR_SUM, nIR2Data);
    SendDataToBridge(DEF_NO3_IR_SUM, nIR3Data);
  }

#endif
}

void uploadRainData()
{
#if DEF_ENABLE_BRIDGE
  if(g_bIsRegDevice)
  {
    __printf(" Upload Rainfall : [Rain] = %f\n", g_Rainfall.GetRainfall());
    SendDataToBridge(DEF_D4_RAIN, g_Rainfall.GetRainfall());
  }
#endif
}
void uploadBME280Data(float fTemp, float fHum, float fPressure)
{
#if DEF_ENABLE_BRIDGE
  if(g_bIsRegDevice)
  {
    __printf(" Upload BME280 Data:[Temp, Hum, Pressure] = %f, %f, %f\n", fTemp, fHum, fPressure);
    SendDataToBridge(DEF_D3_I2C_TEMP, fTemp);
    SendDataToBridge(DEF_D3_I2C_HUM, fHum);
    SendDataToBridge(DEF_D3_I2C_PRESS, fPressure);
  }

#endif

}

void uploadADCData(float fSoil, float fWind, float fUV)
{
	#if DEF_ENABLE_BRIDGE
	
    if(g_bIsRegDevice)
    {
     	__printf("Upload ADC Data:[Soil, Wind, UV] =  %f, %f, %f\n", fSoil, fWind, fUV);
        SendDataToBridge(DEF_A0_SOIL, fSoil); 
        SendDataToBridge(DEF_A1_WIND, fWind);
        SendDataToBridge(DEF_A2_UV, fUV);
    }
  
#endif
}
