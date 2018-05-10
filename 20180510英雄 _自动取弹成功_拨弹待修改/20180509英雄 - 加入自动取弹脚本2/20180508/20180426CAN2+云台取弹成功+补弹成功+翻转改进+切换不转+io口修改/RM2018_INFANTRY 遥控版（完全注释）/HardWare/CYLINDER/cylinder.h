#ifndef __CYLINDER_H
#define __CYLINDER_H	 
#include "stm32f4xx.h"

#define Cylinder_PUSH()    GPIO_SetBits(GPIOI,GPIO_Pin_0)
#define Cylinder_BACK()    GPIO_ResetBits(GPIOI,GPIO_Pin_0)
#define Cylinder_TOGGLE() (GPIOI->ODR) ^= GPIO_Pin_0

void Cylinder_Configuration(void);

#endif

















