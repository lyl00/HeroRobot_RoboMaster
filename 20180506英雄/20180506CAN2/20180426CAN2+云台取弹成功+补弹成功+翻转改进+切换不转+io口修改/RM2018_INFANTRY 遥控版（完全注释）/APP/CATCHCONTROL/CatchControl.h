#ifndef _CATCHCONTROL_H_
#define _CATCHCONTROL_H_

#include "rc.h"
#include "main.h"
#define Kv1 1//抬升速度系数，越大抬升越快
#define Kv2 600//翻转速度
#define FORWARD 1//翻转电机正转
#define BACKWARD 2//翻转电机反转

void CatchControl(void);
void CatchInit(void);


#endif
