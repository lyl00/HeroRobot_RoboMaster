#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "stm32f4xx.h"

#include "main.h"

extern float mid_A, mid_B,mid;

//void Motor_Configuration(void);
//void Motor_Loop(void);
//void Motor_SendSpeed(float speedA, float speedB);
//void Motor_CalcPosi(void);
//void Motor_Stop(void);
//void Motor_CalcSpeed(float speedA, float speedB);

void Motor2_Configuration(void);
void Motor2_Loop(void);


#endif
