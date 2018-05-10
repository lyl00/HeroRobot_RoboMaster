#ifndef __CYLINDER_H
#define __CYLINDER_H	 
#include "stm32f4xx.h"

#define Cylinder_PUSH()    GPIO_SetBits(GPIOD,GPIO_Pin_15)
#define Cylinder_BACK()    GPIO_ResetBits(GPIOD,GPIO_Pin_15)
#define Cylinder_TOGGLE() (GPIOD->ODR) ^= GPIO_Pin_15

void Cylinder_Configuration(void);

#endif

















