#ifndef __CAN_H__
#define __CAN_H__
#include "stm32f4xx.h"
//#include "sys.h"    
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//CAN���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/7
//�汾��V1.0 
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

volatile extern int16_t 	current_cm_speed[4];
volatile extern int16_t 	current_cm_angle[4];
extern float 				can1_current_angel205,can1_current_angel206;
extern volatile int16_t 	can1_current_motor_current205,can1_current_motor_current206;
extern volatile int16_t 	can1_current_motor_speed205,can1_current_motor_speed206;

u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN��ʼ��
void CAN1_Send_Bottom(int16_t candata1,int16_t candata2,int16_t candata3,int16_t candata4);
void CAN1_Rotate(int16_t candata1, int16_t candata2);
void CAN1_SendMessage(uint8_t len, uint8_t *data);

#define CAN1_MESSAGE_ID_BOTTOM			0x080
#define CAN1_MESSAGE_ID_UPPER			0x0c0

#endif



