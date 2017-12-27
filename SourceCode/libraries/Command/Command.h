#ifndef __COMMAND_H__
#define __COMMAND_H__
#include <arduino.h>
#include "Common.h"
#include <Bridge.h>


#define DEF_TYPE_NAME_MAX_LEN               (3)
#define DEF_PIN_NAME_MAX_LEN	            (7)
#define DEF_BRIDGE_NAME_MAX_LEN				(DEF_TYPE_NAME_MAX_LEN + DEF_PIN_NAME_MAX_LEN + 1)

#define DEF_METHOD_IN					    "in"
#define DEF_METHOD_OUT						"out"
	
typedef struct
{
	char strType[DEF_TYPE_NAME_MAX_LEN];  		//incomming / outcommint , such as set/get
	char strPinName[DEF_PIN_NAME_MAX_LEN]; 	//FEATURE_NAME: example:A1
							//bridge name: strType[20]_strPinName

	int nPinNum;			//pin number : 0xA1, 0x03
	int nPrevStatus; 		// last times status
	
	
}_stCommand;


/*
	用于查询是否有新的cmd进来. incomming类型;
	当状态和上次状态不一样时, 说明有更新资料
*/
int pollingCmd();

/*
	用于上传资料到AR9331.
	nPinNum : 代表上传的是哪个GPIO的资料;
	fVal: 代表是资料的值.
	
*/
int SendDataToBridge(int nPinNum, float fVal);

/*
	用于确认是否已经在IoT上注册成功.
	Reg_done: 1 注册成功, 否则失败

*/
bool CheckRegStatus();



#endif