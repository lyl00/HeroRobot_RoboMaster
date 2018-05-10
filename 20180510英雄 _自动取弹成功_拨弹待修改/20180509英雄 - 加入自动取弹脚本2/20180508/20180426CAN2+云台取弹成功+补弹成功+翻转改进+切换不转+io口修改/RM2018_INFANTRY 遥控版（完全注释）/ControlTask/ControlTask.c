/************************************************************
 *File		:	main.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	RM2018_INFANTRY project. 
								Control the CM (Chassis motor) , Gimbal motor , Shoot motor BY Remote controller.
								USB CAN to connect motor made by DJI.
 ************************************************************/
 
#include "main.h"
 
#define _chassis_move            //定义_chassis_move可使底盘运动
//int timelll=0;
uint32_t time_tick_1ms = 0; //1毫秒计数，以“分时”操作各子控制程序
int rc_count00=0;

void WholeInitTask()
{

	//射击初始化
	ShootControlInit();
	//陀螺仪初始化
	MPU6500_Init();				
	//欧拉变换初始化	
	init_euler();						
	//遥控器任务初始化
	
	RemoteControlInit();
//  //底盘任务初始化	
	CMControlInit();			
	//云台控制初始化	
  GimbalControlInit();
	CatchInit();
	
	

	//提示音
	BUZZER_ON(1500);
	delay_ms(250);
	BUZZER_OFF();
	CAN2_Cmd_SHOOT(0);

}

/*-------------  主控制程序  -------------*/
void Control_Task()
{
	time_tick_1ms++;
	//TIM_SetCompare1(TIM8,1850);
	//遥控器读取循环
	 RemoteControlLoop();
	//监控程序循环
	 monitorControlLoop();    //监控程序要在云台程序前面执行，否则遥控器掉线启动会疯转
	//云台控制循环
	 GimbalControlLoop();
	 CatchControl();
	
	if(time_tick_1ms%5 == 2)
	{
		//底盘控制循环
		update_from_dma();
	}
	
	if(time_tick_1ms%10 == 0)
	{
#ifdef _chassis_move
	//底盘控制循环
		CMControlLoop();
#endif
	}
		if(time_tick_1ms%10 == 1)
	{
	//射击控制循环
		ShootControlLoop();
		
	}
	//出错模式，每秒闪8次
	if(time_tick_1ms%125 == 0 && remoteState == ERROR_STATE)
	{
		  LED_G_TOGGLE(); 
			LED_R_TOGGLE();
	}
	//待机模式，每秒闪2次
	if(time_tick_1ms%500 == 0 && remoteState == STANDBY_STATE)
	{
		  LED_G_TOGGLE(); 
			LED_R_TOGGLE();
	}
	//正常模式，每秒闪1次
	if(time_tick_1ms%999 == 1 && remoteState == NORMAL_REMOTE_STATE)
	{
			LED_G_TOGGLE(); 
			LED_R_TOGGLE();
	}
	if(time_tick_1ms == 10000)   //清零，防止计数过大
		time_tick_1ms = 0;
	
}
