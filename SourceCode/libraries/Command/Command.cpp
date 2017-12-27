#include "Command.h"


_stCommand g_cmdTable[]={
	//bridge name = method_feature_name; 
	//example : incomming_D6_LED ; outcomming_A0_Soil
	//method       feature_name     pin num             prev_status
	{DEF_METHOD_IN ,  "D6_LED",      DEF_D6_LED,          999},
	{DEF_METHOD_OUT,  "A0_Soil", 	 DEF_A0_SOIL,         0  },
	{DEF_METHOD_OUT,  "A1_Wind", 	 DEF_A1_WIND, 		  0  },
	{DEF_METHOD_OUT,  "A2_UV",	  	 DEF_A2_UV,   		  0  },
	{DEF_METHOD_OUT,  "D3_Hum",      DEF_D3_I2C_HUM,  	  0  },
	{DEF_METHOD_OUT,  "D3_Temp",     DEF_D3_I2C_TEMP,     0  },
	{DEF_METHOD_OUT,  "D3_Pres",     DEF_D3_I2C_PRESS,    0  },
	{DEF_METHOD_OUT,  "D4_Rain",     DEF_D4_RAIN,         0  },
	/*
	{DEF_METHOD_OUT,  "D8_IR",       DEF_D8_IR,           0  },
	{DEF_METHOD_OUT,  "D9_IR",       DEF_D9_IR,           0  },
	{DEF_METHOD_OUT,  "D10_IR",      DEF_D10_IR,          0  },
	{DEF_METHOD_OUT,  "D11_IR",      DEF_D11_IR,          0  },
	{DEF_METHOD_OUT,  "D12_IR",      DEF_D12_IR,          0  },
	{DEF_METHOD_OUT,  "D13_IR",      DEF_D13_IR,          0  },
	{DEF_METHOD_OUT,  "A3_IR",       DEF_A3_IR,           0  },
	{DEF_METHOD_OUT,  "A4_IR",       DEF_A4_IR,           0  },
	{DEF_METHOD_OUT,  "A5_IR",       DEF_A5_IR,           0  },
	
	*/
	{DEF_METHOD_OUT,  "D11_IR1",     DEF_NO1_IR_SUM,      0  },
	{DEF_METHOD_OUT,  "D8_IR2",      DEF_NO2_IR_SUM,      0  },
	{DEF_METHOD_OUT,  "A3_IR3",      DEF_NO3_IR_SUM,      0  },
};


int pollingCmd()
{
	
	char strKey[DEF_BRIDGE_NAME_MAX_LEN];
	char strName[DEF_PIN_NAME_MAX_LEN];
	char strIncomming[2]={'\0'};
	char strFlag[] = {"_"};
	char strValue[10];
	
	for(int i = 0; i < (int) ARRAY_SIZE(g_cmdTable); i++)
	{
		//check cmd type
		if(strcmp(g_cmdTable[i].strType, DEF_METHOD_IN) != 0)
			continue;
		
		int nLen = sizeof( g_cmdTable[i].strType) + sizeof(g_cmdTable[i].strPinName);
		if(nLen > DEF_BRIDGE_NAME_MAX_LEN)
			return -1;
		strcpy(strKey, g_cmdTable[i].strType);
		strcpy(strName, g_cmdTable[i].strPinName);
		
		strncat(strKey, strFlag, sizeof(strFlag));
		strncat(strKey, strName, sizeof(strName));
		
		Bridge.get(strKey, strIncomming, 2);
		
		if(atoi(strIncomming) != g_cmdTable[i].nPrevStatus)
		{
			g_cmdTable[i].nPrevStatus = atoi(strIncomming);
			Bridge.get(strName, strValue, 5);
			
			__printf(" [Incomming Data] [Key, Value] = [%s, %s]\n", strKey, strValue);
			
			strValue[4]='\0';
			int nVal = atoi(strValue);
			nVal = (nVal != 0 ? HIGH : LOW);
			digitalWrite(g_cmdTable[i].nPinNum, nVal);
		}
		
	}
	
	return 0;
	
}

int SendDataToBridge(int nPinNum, float fVal)
{
	int nIndx = -1;
	
	for(int i = 0; i < (int)ARRAY_SIZE(g_cmdTable); i++)
	{
		
		if(strcmp(g_cmdTable[i].strType, DEF_METHOD_OUT) != 0)
			continue;
		
		if(g_cmdTable[i].nPinNum == nPinNum)
		{
			nIndx = i;
			break;
		}
		
	}
	
	if(nIndx == -1)
	{
		__printf("[ERROR] outcomming Cmd not found!!!");
		return -1;
	}
	
	char outcome[4];
	char strValue[10];
	char strKey[DEF_BRIDGE_NAME_MAX_LEN];
	char strName[DEF_PIN_NAME_MAX_LEN];
 
	char strFlag[] = {"_"};
	strcpy(strKey,  g_cmdTable[nIndx].strType);
	strcpy(strName, g_cmdTable[nIndx].strPinName);
		
	strncat(strKey, strFlag, sizeof(strFlag));
	
	
	strncat(strKey, strName, sizeof(strName) > DEF_PIN_NAME_MAX_LEN ? DEF_PIN_NAME_MAX_LEN : sizeof(strName));
	

	//itoa(fVal, strValue, 10); 
	int nBufLen = 0;
	ExFloatToString(fVal, strValue, nBufLen);
    Bridge.put(strName, strValue);

	
	__printf(" [Outcomming Data] [Key, Value] = [%s, %s]\n", strName, strValue);
    itoa((g_cmdTable[nIndx].nPrevStatus = g_cmdTable[nIndx].nPrevStatus^1), outcome, 10);
    Bridge.put(strKey, outcome);    
	
	__printf(" [Outcomming Flag] [Key, Value] = [%s, %s]\n", strKey, outcome);
	
	return 0;
}

bool CheckRegStatus()
{
	char pin13[2];
	pinMode(13, OUTPUT);
	Bridge.get("Reg_done", pin13, 2);
    digitalWrite(13, atoi(pin13)); 
	
	pinMode(13, INPUT);
	return atoi(pin13) ? true : false;
}