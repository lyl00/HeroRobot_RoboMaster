#ifndef __PID_H__
#define __PID_H__

#include "stm32f4xx.h"
//#include "sys.h"
//#define u32 unsigned long
//#define u8 unsigned char

//PIDCR寄存器（调用SetPIDCR来更改其值）
#define PEN 0		  //比例使能，0-关闭，1-打开，控制比例运算状态
#define IEN 1		  //积分使能，0-关闭，1-打开，控制积分运算状态
#define DEN 2		  //微分使能，0-关闭，1-打开，控制微分运算状态
#define PIDEN 3		  //PID使能，0-关闭，1-打开，控制PID运算的总开关
#define IHEN 4		  //积分保持使能，0-关闭，1-打开，当积分运算由打开变为关闭，若该位置位，则会保持原来积分的值，否则将积分值清0
#define INTERVAL 5	  //传入PID_Calc函数的第二个参数为系统时间还是固定时间，0-系统时间，传入系统时间，PID运算函数会自动计算时间间隔，单位为us；1-固定时间间隔，此时PID运算函数以该值作为PID运算时间间隔，单位为us
#define TOR 6		  //超时复位，0-关闭，1-打开。当此位置位时，每次计算会检查计算时间间隔是否大于某个设定值（InternalTimeMax），若大于，说明计算间隔超时，PID控制器将会被复位。

//PIDSR寄存器（只读)
#define CHANGE 0	  //PIDCR值发生改变状态位，当对PIDCR寄存器进行更改时，该位被置位，用于PID计算预处理
#define FC 1		  //第一次PID计算状态位，当PID初始化或复位后，该位被置位，直到第一次PID计算完成，该位被清除，用于PID计算预处理
typedef struct 
{
	float  SetPoint;           //  设定目标 Desired Value
	float  NowPoint;			//	当前实际值 
	
	float  Proportion;         //  比例常数 Proportional Const 
	float  Integral;           //  积分常数 Integral Const 
	float  Derivative;         //  微分常数 Derivative Const 
		
	float  Integral_Min;		//积分下限幅
	float  Integral_Max;		//积分上限幅

	float  InternalTimeMax;//计算PID时间间隔上限，超过此上限则认为间隔有误，不计算PID

	float  Out;			//PID调节输出值

//不要在PID计算中直接更改下列参数
	float  LastError;          //  Error[-1] 
	float  PrevError;          //  Error[-2] 
	float  IntegralError;           //  Integral of Errors

	u32    LastTime;//用于记录最后一次计算PID的时间

	u8 PIDCR;
	u8 PIDCR_DUMMY;
	u8 PIDSR;
}PID_Struct;

void PID_Init(PID_Struct* pid,float Proportion,float Integral,float Derivative,float Integral_Min,float Integral_Max,float InternalTimeMax);
void SetPIDCR(PID_Struct* pid,u8 value);
void PID_Calc(PID_Struct *pid,u32 IntervalTime);
float PID_Driver(float SetSpeed, float NowSpeed , PID_Struct* pid,u32 IntervalTime);//PID计算预处理
void PidValControl(PID_Struct* pid);          //用于调试PID参数时，动态地改变PID值
#endif
