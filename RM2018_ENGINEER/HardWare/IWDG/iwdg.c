/************************************************************
 *File		: iwdg.c
 *Author	: YangTao @ ybb331082@126.com 
 *Version	: V1.0
 *Update	: 2018.5.1
 *Description: 	
 *Input   : None
 *Output  : None
 ************************************************************/

#include "main.h"

void IWDG_Configuration(void)
{
	// LSI = 32kHz
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//打开写功能
	IWDG_SetPrescaler(IWDG_Prescaler_32);			//32分频率，最大超时4096ms
	IWDG_SetReload(1000);							//超时时间1s
	IWDG_Enable();									//使能看门狗，写功能失效
}

void IWDG_FeedDog(void)
{
	IWDG_ReloadCounter();
}
