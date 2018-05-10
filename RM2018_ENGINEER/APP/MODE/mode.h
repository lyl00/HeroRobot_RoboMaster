#ifndef __MODE_H__
#define __MODE_H__
#include "stm32f4xx.h"

typedef enum 
{
	MODE_STANBY=0,			//待机/初始模式：不接受任何指令
	MODE_DRAG,				//拖车模式：
	MODE_LIFT,				//抬升模式：
	MODE_BULLET				//取弹模式：
} Mode_WorkingMode;

#define MODE_DEFAULT	MODE_STANBY

extern Mode_WorkingMode GlobalMode;


void Mode_Configuration(void);
void Mode_Loop(void);

#endif
