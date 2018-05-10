/*************************************************
Function		: BSP_Init
Description	: Init NVIC, Delay, GPIO, USART, CAN , RC, PWM ,TIMER
Input				: None
Output			: None
Return			: None
*************************************************/

#include "main.h"

void BSP_Init(void)	//注意初始化的顺序
{
	//初始化延时函数	
	delay_init(168);  
  //初始化led底层	
		
	USART6_DMA_Init(); //裁判系统初始化
	TIM8_PWM_Init(2000-1,840-1);//PWM初始化
	Led_Configuration(); 
	//初始化气缸IO底层
	Cylinder_Configuration();
	//初始化红外传感IO底层
	Infrared_Configuration();
	//初始化蜂鸣器
	Buzzer_Init();
	//初始化激光
	Laser_Configuration();
	//初始化电流检测模块
	Adc_Init();
	//初始化串口6底层
	USART6_Configuration(115200); 
	//初始化串口3、DMA1底层
	USART3_DMA_Init();  
	//初始化SPI5,用于读取板载陀螺仪数据
	SPI5Init(); 
	//初始化usart2串口、DMA2，用于遥控器数据读取		
	RC_Init();   
	//CAN1初始化，波特率1Mbps
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_9tq,3,CAN_Mode_Normal); 
	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_9tq,3,CAN_Mode_Normal); 
	//初始化定时器12，用于控制摩擦轮转动
	TIM12_Init(); 
	
  /* 所有任务初始化*/
	WholeInitTask();	
	
	/*定时器初始化*/
	TIM2_Init();   //初始化定时器2，用于陀螺仪读值跟新，此时陀螺仪还没有初始化，不工作
  delay_ms(500); //等待0.5s，等云台稳定
	TIM5_Init();   //初始化定时器5，用于控制任务执行，此时陀螺仪开始工作，位置记为初始位置
}
