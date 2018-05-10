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

#define _automove

float disp0[8];
int16_t disp1[8];

void WholeInitTask()
{
//	//陀螺仪初始化
//	MPU6500_Init();				
//	//欧拉变换初始化	
//	init_euler();
	
	//气缸控制(继电器)初始化
	Cylinder_Configuration();
	//涵道控制初始化
	Duct_Configuration();
	//抱柱电机和翻箱电机控制初始化
	Rotate_Configuration();
	//舵机控制初始化
	Servo_Configuration();
	//摄像机视角初始化
	Camera_Configuration();
	//看门狗初始化
	IWDG_Configuration();
	
	//模式控制初始化
	Mode_Configuration();
#ifdef _automove
	//自动化控制初始化
	AutoMove_Configuration();	
#endif

	//提示音
	printf("Hello world!\r\n");
	BUZZER_ON(1500);delay_ms(500);BUZZER_OFF();
}

/*-------------  初始化过程控制程序  -------------*/
void Init_Task()
{
	static uint32_t time_tick_init = 0; //0.2毫秒计数，以“分时”操作各子控制程序
	
	time_tick_init++;
	
	INITREADY_FLAG = 1;
	
	//每约11ms喂一次狗
	if(time_tick_init%11==0){
		IWDG_FeedDog();
	}
}


/*-------------  主控制程序  -------------*/
void Control_Task()
{
	static uint32_t time_tick_1ms = 0; //0.1毫秒计数，以“分时”操作各子控制程序
	time_tick_1ms++;
	
	//读取控制循环
	RemoteControlLoop();
//	//监控程序循环
//	monitorControlLoop();    //监控程序要在云台程序前面执行，否则遥控器掉线启动会疯转
	
	//涵道控制控制循环
	Duct_Loop();
	//气缸控制循环
	Cylinder_Loop();
	//抱柱电机和翻箱电机控制循环
	Rotate_Loop();
	//舵机控制控制循环
	Servo_Loop();
	//摄像机视角控制循环 10Hz
	Camera_Loop();
	

#ifdef _automove
	//自动化控制程序
	AutoMove_Loop(time_tick_1ms);
#endif
	
	//每约11ms喂一次狗
	if(time_tick_1ms%11==0){
		IWDG_FeedDog();
	}
	
//	if(time_tick_1ms%999==0){
//		printf("%d, %d\r\n",disp1[0],disp1[1]);
//		printf("\r\n");
//	}
	
	//信号灯
	switch(GlobalMode){
	case MODE_DRAG:
		if(time_tick_1ms%999 == 0) {
			LED_G_TOGGLE(); LED_R_TOGGLE();
		}
		break;
	case MODE_LIFT:
		if(time_tick_1ms%499 == 0) {
			LED_G_TOGGLE(); LED_R_TOGGLE();
		}
		break;
	case MODE_BULLET:
		if(time_tick_1ms%333 == 0) {
			LED_G_TOGGLE(); LED_R_TOGGLE();
		}
		break;
	default:
		LED_G_OFF(); LED_R_ON();
	}
	
}
