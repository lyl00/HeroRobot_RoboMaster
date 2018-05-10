#ifndef _CATCHCONTROL_H_
#define _CATCHCONTROL_H_

#include "rc.h"
#include "main.h"
#define Kv1 1//抬升速度系数，越大抬升越快
#define Kv2 600//翻转速度
#define FORWARD 1//翻转电机正转
#define BACKWARD 2//翻转电机反转

// 自动取弹任务次序，用于&运算判断当前该执行哪个操作
#define TASK_A			0x01      //抓
#define TASK_B			0x02      //抬
#define TASK_C			0x04      //收
#define TASK_D			0x08      //翻转
#define TASK_E			0x10      //往回翻转
#define TASK_F			0x20      //伸
#define TASK_G			0x40      //松
#define TASK_H			0x80      //收
#define AutoCatch_FLIFT_POSLIMIT_H   8192*3.f      //再次抬升的高度，先设置成限位值


void CatchControl(void);
void CatchInit(void);
void AutoCatch_tInit(void); //初始化所有时间常数
void AutoCatch_TaskControl(void); //自动取弹循环,管理时间参数
void AutoCatch_TaskJudge(u8 Register); //根据取弹循环中得到的寄存器值进行相应步骤操作



#endif
