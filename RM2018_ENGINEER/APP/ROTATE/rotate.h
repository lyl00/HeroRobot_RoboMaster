#ifndef __ROTATE_H__
#define __ROTATE_H__
#include "stm32f4xx.h"

#include "main.h"

extern float mid_A, mid_B,mid;

void Rotate_Configuration(void);
void Rotate_Loop(void);

//开放给自动控制程序
void Rotate_Bao_Start(void);
void Rotate_Bao_Stop(void);
void Rotate_Bao_Toggle(void);
void Rotate_Bao_DirePos(void);
void Rotate_Bao_DireRev(void);
void Rotate_Bao_DireToggle(void);
void Rotate_Fan(void);

#endif
