#ifndef __SERVO_H__
#define __SERVO_H__
#include "stm32f4xx.h"

#define SERVO_SPEED_MIN		550			// 0.55ms
#define SERVO_SPEED_MAX		2450		// 2.45ms

#define SERVO_OPEN_OC3		550
#define SERVO_CLOSE_OC3		1160
#define SERVO_OPEN_OC4		2450
#define SERVO_CLOSE_OC4		1600

void Servo_Configuration(void);
void Servo_Loop(void);

#endif
