#ifndef _TIMER_H
#define _TIMER_H
#include "stm32f4xx.h"

//////////////////////////////////////////////////////////////////////////////////	 
extern u8 INITREADY_FLAG;

void TIM5_Init(void);
void TIM2_Init(void);
void TIM4_Init(void);
void TIM8_Init(void);
//void TIM12_Init(void);
#endif
