#ifndef __LED_H
#define __LED_H
#include "stm32f4xx.h"

#define LED_G_OFF()       	GPIO_SetBits(GPIOF,GPIO_Pin_14)			
#define LED_G_ON()      	GPIO_ResetBits(GPIOF,GPIO_Pin_14)
#define LED_G_TOGGLE()   	(GPIOF->ODR)^= GPIO_Pin_14//GREEN		低电平触发

#define LED_R_OFF()       	GPIO_SetBits(GPIOE,GPIO_Pin_7)
#define LED_R_ON()      	GPIO_ResetBits(GPIOE,GPIO_Pin_7)
#define LED_R_TOGGLE()   	(GPIOE->ODR)^= GPIO_Pin_7//RED


void Led_Configuration(void);//初始化		 


#endif
