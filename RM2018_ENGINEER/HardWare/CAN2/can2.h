#ifndef __CAN2_H
#define __CAN2_H	 
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

#define CAN2_MOTOR_COUNT		4
#define CAN2_STUCK_FILTER		38.0f				//�����ж��Ƿ��˶�

extern volatile	uint16_t 	can2_stuckflag[CAN2_MOTOR_COUNT];
extern volatile	int16_t 	can2_current_motor_current[CAN2_MOTOR_COUNT];
extern volatile int16_t 	can2_current_motor_speed[CAN2_MOTOR_COUNT];
extern float 				can2_current_angel[CAN2_MOTOR_COUNT];


u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN��ʼ��
void CAN2_Motor(int16_t candata1, int16_t candata2, int16_t candata3, int16_t candata4);
//void CAN2_Rotate(int16_t candata1, int16_t candata2);

#endif


