/************************************************************
 *File		:	main.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	RM2018_INFANTRY project. 
								Control the CM (Chassis motor) , Gimbal motor , Shoot motor BY Remote controller.
								USB CAN to connect motor made by DJI.
 ************************************************************/
 
#include "main.h"
 
#define _chassis_move            //����_chassis_move��ʹ�����˶�
//int timelll=0;
uint32_t time_tick_1ms = 0; //1����������ԡ���ʱ���������ӿ��Ƴ���
int rc_count00=0;

void WholeInitTask()
{

	//�����ʼ��
	ShootControlInit();
	//�����ǳ�ʼ��
	MPU6500_Init();				
	//ŷ���任��ʼ��	
	init_euler();						
	//ң���������ʼ��
	
	RemoteControlInit();
//  //���������ʼ��	
	CMControlInit();			
	//��̨���Ƴ�ʼ��	
  GimbalControlInit();
	CatchInit();
	
	

	//��ʾ��
	BUZZER_ON(1500);
	delay_ms(250);
	BUZZER_OFF();
	CAN2_Cmd_SHOOT(0);

}

/*-------------  �����Ƴ���  -------------*/
void Control_Task()
{
	time_tick_1ms++;
	//TIM_SetCompare1(TIM8,1850);
	//ң������ȡѭ��
	 RemoteControlLoop();
	//��س���ѭ��
	 monitorControlLoop();    //��س���Ҫ����̨����ǰ��ִ�У�����ң���������������ת
	//��̨����ѭ��
	 GimbalControlLoop();
	 CatchControl();
	
	if(time_tick_1ms%5 == 2)
	{
		//���̿���ѭ��
		update_from_dma();
	}
	
	if(time_tick_1ms%10 == 0)
	{
#ifdef _chassis_move
	//���̿���ѭ��
		CMControlLoop();
#endif
	}
		if(time_tick_1ms%10 == 1)
	{
	//�������ѭ��
		ShootControlLoop();
		
	}
	//����ģʽ��ÿ����8��
	if(time_tick_1ms%125 == 0 && remoteState == ERROR_STATE)
	{
		  LED_G_TOGGLE(); 
			LED_R_TOGGLE();
	}
	//����ģʽ��ÿ����2��
	if(time_tick_1ms%500 == 0 && remoteState == STANDBY_STATE)
	{
		  LED_G_TOGGLE(); 
			LED_R_TOGGLE();
	}
	//����ģʽ��ÿ����1��
	if(time_tick_1ms%999 == 1 && remoteState == NORMAL_REMOTE_STATE)
	{
			LED_G_TOGGLE(); 
			LED_R_TOGGLE();
	}
	if(time_tick_1ms == 10000)   //���㣬��ֹ��������
		time_tick_1ms = 0;
	
}
