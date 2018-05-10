/************************************************************
 *File		:	monitorControl.c
 *Author	:	@YangTianhao ,490999282@qq.com
 *Version	: V1.0
 *Update	: 2017.04.13
 *Description: 	Monitor of the whole system  (return 0 for no problem, 1 for error)
								including : 
								rc
								can
								adc
													
 ************************************************************/
#include "main.h"

#define ELEDIE 6.0f
#define ELEOFFSET (80/50.0)
#define DIV_ELE 0.185f
#define rc_monitor_time_limit 30 //遥控器监控计时上限

float ele_cal = 0;
int16_t rc_monitor_time = 0; //遥控器监控计时


u8 AdcReceiveMonitor()     //待写
{
	return 0;
}

/*-------------  功率监视程序  -------------*/
int16_t CMWatt_Cal(int16_t speedX , float ele_now)
{
	float speed_out;
	if (fabs(ele_now)>ELEDIE)
		return 0;
	speed_out = speedX * (1 - fabs(ele_now)/(float)ELEDIE) ; 
	return (int16_t)speed_out;
}

void PowerMonitor()     
{
		ele_cal = ( Get_Adc(12)/4096.0*3.3f - 2.48 )/DIV_ELE;
		ele_cal = fabs(ele_cal);
		if (ele_cal < -5 || ele_cal > 5) ele_cal = ELEOFFSET;
}


/*-------------  遥控器监视程序  -------------*/
void RcReceiveMonitor()
{
		if(rc_monitor_count == rc_monitor_ex_count ) //假如值没变
			{   
				if(rc_monitor_time > rc_monitor_time_limit)   //遥控器在一定时间内没变化 判断为掉线
					remoteState = ERROR_STATE;     
				else												//进行累计
					rc_monitor_time++;
		  }
		else {                                       //值一直更新
					rc_monitor_time  = 0;	
		  }
		rc_monitor_ex_count = rc_monitor_count;
}

/*************************************************
Function		: monitorControlLoop
Description	: rcReceive
							247ms:Printf some information

*************************************************/
void monitorControlLoop(void)						//1000ms per loop 
{
	RcReceiveMonitor();
	PowerMonitor();
}
