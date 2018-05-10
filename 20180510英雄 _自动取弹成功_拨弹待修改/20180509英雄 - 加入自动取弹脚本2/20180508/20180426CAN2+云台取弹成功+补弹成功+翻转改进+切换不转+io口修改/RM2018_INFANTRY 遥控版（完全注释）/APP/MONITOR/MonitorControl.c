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
#define rc_monitor_time_limit 30 //ң������ؼ�ʱ����

float ele_cal = 0;
int16_t rc_monitor_time = 0; //ң������ؼ�ʱ


u8 AdcReceiveMonitor()     //��д
{
	return 0;
}

/*-------------  ���ʼ��ӳ���  -------------*/
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


/*-------------  ң�������ӳ���  -------------*/
void RcReceiveMonitor()
{
		if(rc_monitor_count == rc_monitor_ex_count ) //����ֵû��
			{   
				if(rc_monitor_time > rc_monitor_time_limit)   //ң������һ��ʱ����û�仯 �ж�Ϊ����
					remoteState = ERROR_STATE;     
				else												//�����ۼ�
					rc_monitor_time++;
		  }
		else {                                       //ֵһֱ����
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
