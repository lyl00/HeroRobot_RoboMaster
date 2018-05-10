/************************************************************
 *File		:	pid.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com	
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Pid Struct
								Pid output calculation
 ************************************************************/
#include "pid.h"
#include "sys.h"
#include "usart6.h"
#include "main.h"

float test_p = 0,test_i = 0,test_d = 0.00,test_limit=120;   //调PID用

u8 PID_BitIsSet(u8 Register,u8 Position)
{
	if(Register & 1<<Position)
		return 1;
	return 0;
}
//PID_Init
//功能：PID定义函数
//参数：pid指针, p, i ,d ,积分下限，积分上限，积分时间值
//返回值：void
void PID_Init(PID_Struct* pid,float Proportion,float Integral,float Derivative,float Integral_Min,float Integral_Max,float InternalTimeMax)
{
	pid->Proportion=Proportion;
	pid->Integral=Integral;
	pid->Derivative=Derivative;
	pid->Integral_Min=Integral_Min;
	pid->Integral_Max=Integral_Max;
	pid->PIDCR=0x00;
	pid->PIDCR_DUMMY=0x00;
	pid->PIDSR=0x00;
}

void SetPIDCR(PID_Struct* pid,u8 value)
{
	pid->PIDCR_DUMMY=value;
	pid->PIDSR|=1<<CHANGE;
}
//PID_PreCalc
//功能：PID计算预处理
//参数：pid指针
//返回值：void
void PID_PreCalc(PID_Struct* pid)//PID计算预处理
{
	u8 ChangeFlag,EdgeTypeFlag;//ChangeFlag对应位为1表示标志位发生改变，若发生改变，EdgeTypeFlag对应位为0表示上升沿，为1表示下降沿
	ChangeFlag=pid->PIDCR^pid->PIDCR_DUMMY;
	EdgeTypeFlag=ChangeFlag&pid->PIDCR;

	if(PID_BitIsSet(ChangeFlag,PIDEN))//PIDEN位发生任何变化，都将清除微分和积分误差纪录值
	{
		pid->IntegralError=pid->PrevError=pid->LastError=0;
		if(PID_BitIsSet(EdgeTypeFlag,PIDEN))//如果PIDEN位由关闭变为开启，则将PIDSR的FC置位
			pid->PIDSR|=1<<FC;

	}
	else//若PIDEN位未发生变化，再处理PEN,IEN,DEN位的变化
	{
		if(PID_BitIsSet(ChangeFlag,IEN) && PID_BitIsSet(EdgeTypeFlag,IEN) && !PID_BitIsSet(pid->PIDCR,IHEN))//积分误差关闭，如果PIDCR的IHEN位使能，则不清除积分值，否则清除
			pid->IntegralError=0;

		if(PID_BitIsSet(ChangeFlag,DEN) && PID_BitIsSet(EdgeTypeFlag,DEN))//微分误差关闭，则清除微分纪录值
			pid->PrevError=pid->LastError=0;
	}

	pid->PIDCR=pid->PIDCR_DUMMY;//修改PIDCR的值
	pid->PIDSR&=~(1<<CHANGE);//已经处理完PIDCR变化，清除PIDSR的CHANGE位
}

//PID_Calc
//功能：PID计算
//参数：pid指针 ,积分时间值
//返回值：void 改变pid里的output值
void PID_Calc(PID_Struct* pid,u32 IntervalTime) 
{ 
	float  	dError,//误差微分
					Error, //误差
					Out=0;//PID输出
	float 	Interval;//PID计算时间间隔
	
	if(PID_BitIsSet(pid->PIDSR,CHANGE))//PIDCR寄存器发生更改，则进行相应处理
		PID_PreCalc(pid);

	if(PID_BitIsSet(pid->PIDCR,INTERVAL)==0)//传入的是系统时间
	{		
		Interval=0.000001f*(IntervalTime-pid->LastTime);
		pid->LastTime=IntervalTime;
		if(PID_BitIsSet(pid->PIDSR,FC))//传入的是系统时间，并且是第一次调用计算函数，那么只记录当前系统时间，不进行PID计算
		{
			pid->PIDSR&=~(1<<FC);
			return;
		}
		if(PID_BitIsSet(pid->PIDCR,TOR) && Interval>pid->InternalTimeMax)//传入的是系统时间，如果PIDCR的TOR置位，则会对时间间隔进行检查，若其大于InternalTimeMax，则认为间隔过长，重启PID计算
		{
			pid->IntegralError=pid->PrevError=pid->LastError=0;
			return;
		}
	}
	else
		Interval=0.000001f*IntervalTime;//将us转换成s


	if(PID_BitIsSet(pid->PIDCR,PIDEN))//PIDEN位使能
	{		
		Error = pid->SetPoint - pid->NowPoint;          // 误差

		if(PID_BitIsSet(pid->PIDCR,PEN))//PEN位使能，计算比例误差
		{
			Out+= pid->Proportion * Error;
		}

		if(PID_BitIsSet(pid->PIDCR,IEN))//IEN位使能，计算积分误差
		{
			pid->IntegralError += Error*Interval;                      // 积分 
			if(pid->IntegralError>pid->Integral_Max)	pid->IntegralError=pid->Integral_Max;	//积分上限幅
			if(pid->IntegralError<pid->Integral_Min)	pid->IntegralError=pid->Integral_Min;	//积分下限幅
			Out+= pid->Integral * pid->IntegralError;
		}
		
		if(PID_BitIsSet(pid->PIDCR,DEN))//DEN位使能，计算微分误差
		{
			dError = (pid->LastError - pid->PrevError)/Interval;     // 当前微分
			pid->PrevError = pid->LastError; 
			pid->LastError = Error;
			Out+= pid->Derivative * dError;
		}
	}

	pid->Out=Out;										//最后输出一个out
}

//PID_Driver
//功能：PID计算预处理
//参数：setspeed, nowspeed , pidstruct, intervalTime
//返回值：float
float PID_Driver(float SetSpeed, float NowSpeed , PID_Struct* pid,u32 IntervalTime)//PID计算预处理
{
	float pidout;
	(*pid).SetPoint = SetSpeed ;//MotorPIDOutYaw;//代入设定值和测定值
	(*pid).NowPoint = NowSpeed;
	PID_Calc(pid,IntervalTime);
	pidout=(*pid).Out ;
	return pidout;
}

//PidValControl
//功能：PID调参用函数
//参数： pidstruct,  
//返回值：void
void PidValControl(PID_Struct* pid)          //用于调试PID参数时，动态地改变PID值
{
	u8 temp_uartbuf;
//	printf("Receive: %c \r\n",usart1_rxbuf);
	temp_uartbuf = usart6_rxbuf;
	usart6_rxbuf = 0;
	switch(temp_uartbuf)
	{
		case 'q':test_p += 2;break;
		case 'w':test_i += 0.5f;break;
		case 'e':test_d += 0.001f;break;
		case 'r':test_limit += 5;break;
		case 'a':test_p -= 1;break;
		case 's':test_i -= 0.1f;break;
		case 'd':test_d -= 0.001f;break;
		case 'f':test_limit -= 5;break;
		case 'z':test_p *= 10;break;
		default:break;
	}
	printf("p:%f\t i:%f\t d:%f\t limit:%f\t \r\n",test_p,test_i,test_d,test_limit);
	PID_Init(pid,test_p,test_i,test_d,test_limit*(-1),test_limit,0);    //
	SetPIDCR(pid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
}
