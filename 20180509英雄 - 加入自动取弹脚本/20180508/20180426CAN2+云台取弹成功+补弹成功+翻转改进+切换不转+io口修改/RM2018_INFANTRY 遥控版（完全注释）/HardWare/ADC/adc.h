#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f4xx.h"
#include <stdio.h>



/************************************************************
 *File		:	adc.c
 *Author	:	@YangTianhao ,490999282@qq.com
 *Version	: V1.0
 *Update	: 2017.03.09
 *Description: 	ADC
 ************************************************************/
#include "adc.h"
#include "delay.h"

/*-----ADC1_IN9(PB1)-----*/

//≥ı ºªØADC															   
void  Adc_Init(void);
u16 Get_Adc(u8 ch);   
u16 Get_Adc_Average(u8 ch,u8 times);
u16 GetAdcAlarm(u8 ch);
extern u8 adc_alarm_flag ;
	 

#endif

