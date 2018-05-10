#ifndef __RC_H__
#define __RC_H__


#include "stm32f4xx.h"

#define RC_CH_VALUE_MIN ((uint16_t)364 )
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)

/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP ((uint16_t)1)
#define RC_SW_MID ((uint16_t)3)
#define RC_SW_DOWN ((uint16_t)2)

/* ----------------------- PC Key Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W ((uint16_t)0x01<<0)
#define KEY_PRESSED_OFFSET_S ((uint16_t)0x01<<1)
#define KEY_PRESSED_OFFSET_A ((uint16_t)0x01<<2)
#define KEY_PRESSED_OFFSET_D ((uint16_t)0x01<<3)
#define KEY_PRESSED_OFFSET_Q ((uint16_t)0x01<<6)
#define KEY_PRESSED_OFFSET_E ((uint16_t)0x01<<7)
#define KEY_PRESSED_OFFSET_SHIFT ((uint16_t)0x01<<4)
#define KEY_PRESSED_OFFSET_CTRL ((uint16_t)0x01<<5)

/* ----------------------- Data Struct ------------------------------------- */
typedef struct 
{
		struct
		{
				int16_t ch0;//��ҡ������
				int16_t ch1;//��ҡ������
				int16_t ch2;//��ҡ������
				int16_t ch3;//��ҡ������
				int8_t s1;	//��ǰ��ť
				int8_t s2; //��ǰ��ť
		}rc;
		struct
		{
				int16_t x;//���x��
				int16_t y;//���y��
				int16_t z;
				uint8_t press_l; //������
				uint8_t press_r;// ����Ҽ�
		}mouse;
		struct
		{
				uint16_t v; 
//			ÿ��������Ӧһ��bit��������ʾ
//			Bit0 �\�\�\�\�\�\�\ W ��
//			Bit1 �\�\�\�\�\�\�\ S ��
//			Bit2 �\�\�\�\�\�\�\ A ��
//			Bit3 �\�\�\�\�\�\�\ D ��
//			Bit4 �\�\�\�\�\�\�\ Shift ��
//			Bit5 �\�\�\�\�\�\�\ Ctrl ��
//			Bit6 �\�\�\�\�\�\�\ Q ��
//			Bit7 �\�\�\�\�\�\�\ E ��
		}key;
		
		uint16_t resv;
		
}RC_Ctl_t;

/* ----------------------- Internal Data ----------------------------------- */
extern volatile unsigned char sbus_rx_buffer[18];
extern RC_Ctl_t RC_Ctl;
extern RC_Ctl_t RC_Ex_Ctl; 
extern uint8_t test_rc;
extern u8 flag_friction_switch ;
extern u8 flag_rc_offline ;
extern int16_t rc_monitor_count , rc_monitor_ex_count;
void RC_Init(void);
void TestRc(void);
void ItoE(void);


#endif
