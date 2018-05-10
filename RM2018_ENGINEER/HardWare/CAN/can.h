#ifndef __CAN_H__
#define __CAN_H__
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

volatile extern int16_t 	current_cm_speed[4];
volatile extern int16_t 	current_cm_angle[4];
extern float 				can1_current_angel205,can1_current_angel206;
extern volatile int16_t 	can1_current_motor_current205,can1_current_motor_current206;
extern volatile int16_t 	can1_current_motor_speed205,can1_current_motor_speed206;

u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
void CAN1_Send_Bottom(int16_t candata1,int16_t candata2,int16_t candata3,int16_t candata4);
void CAN1_Rotate(int16_t candata1, int16_t candata2);
void CAN1_SendMessage(uint8_t len, uint8_t *data);

#define CAN1_MESSAGE_ID_BOTTOM			0x080
#define CAN1_MESSAGE_ID_UPPER			0x0c0

#endif



