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
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//��д����
	IWDG_SetPrescaler(IWDG_Prescaler_32);			//32��Ƶ�ʣ����ʱ4096ms
	IWDG_SetReload(1000);							//��ʱʱ��1s
	IWDG_Enable();									//ʹ�ܿ��Ź���д����ʧЧ
}

void IWDG_FeedDog(void)
{
	IWDG_ReloadCounter();
}
