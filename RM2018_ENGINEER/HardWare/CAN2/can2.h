#ifndef __CAN2_H
#define __CAN2_H	 
#include "stm32f4xx.h"
//#include "sys.h"    
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//CAN驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/7
//版本：V1.0 
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

#define CAN2_MOTOR_COUNT		4
#define CAN2_STUCK_FILTER		38.0f				//角误差，判断是否运动

extern volatile	uint16_t 	can2_stuckflag[CAN2_MOTOR_COUNT];
extern volatile	int16_t 	can2_current_motor_current[CAN2_MOTOR_COUNT];
extern volatile int16_t 	can2_current_motor_speed[CAN2_MOTOR_COUNT];
extern float 				can2_current_angel[CAN2_MOTOR_COUNT];


u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
void CAN2_Motor(int16_t candata1, int16_t candata2, int16_t candata3, int16_t candata4);
//void CAN2_Rotate(int16_t candata1, int16_t candata2);

#endif


