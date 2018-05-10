/************************************************************
 *File		:	
 *Author	:  	@YangTao, ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.3.3
 *Description: 	Duct contorl
				TIM8_CH1(PI5)
 ************************************************************/

#include "main.h"

uint8_t ServoOpen = 0;

//uint32_t Servo_Limit(uint16_t val);
//uint32_t Servo_Speed(int16_t ratio);


/*-------------  �������Ƴ�ʼ��  -------------*/
//��Ħ���ֵ�PWM����
void Servo_Configuration(void)
{		
	//ע�� timer.c �ĳ�ʼ��s
}


/*-------------  �������ƿ���ѭ��  -------------*/
/***************************************************************************************
 *Name     : 
 *Function ��
 *Input    ����
 *Output   ���� 
 *Description :
****************************************************************************************/
void Servo_Loop(void)
{
	#ifdef RC_FUNC
	if(GlobalMode==MODE_DRAG){
		//�ϲ������²���
		if(RemoteSEF_REG&RemoteSEF_S1_UP){
			TIM_SetCompare3(TIM4,SERVO_OPEN_OC3);
			TIM_SetCompare4(TIM4,SERVO_OPEN_OC4);
		}else if(RemoteSEF_REG&RemoteSEF_S1_DOWN){
			TIM_SetCompare3(TIM4,SERVO_CLOSE_OC3);
			TIM_SetCompare4(TIM4,SERVO_CLOSE_OC4);
		}
	}
	else{
		//��ʼ״̬
		TIM_SetCompare3(TIM4,SERVO_CLOSE_OC3);
		TIM_SetCompare4(TIM4,SERVO_CLOSE_OC4);
	}
	#else
	if(GlobalMode==MODE_BULLET){
		//Open
		if(Remote_KEYVal&KEY_PRESSED_OFFSET_S){
			TIM_SetCompare3(TIM4,SERVO_OPEN_OC3);
			TIM_SetCompare4(TIM4,SERVO_OPEN_OC4);
		}
		//Close
		else if(Remote_KEYVal&KEY_PRESSED_OFFSET_W){
			TIM_SetCompare3(TIM4,SERVO_CLOSE_OC3);
			TIM_SetCompare4(TIM4,SERVO_CLOSE_OC4);
		}
	}
	else{
		//��ʼ״̬
		TIM_SetCompare3(TIM4,SERVO_CLOSE_OC3);
		TIM_SetCompare4(TIM4,SERVO_CLOSE_OC4);
	}
	#endif
}

//uint32_t Servo_Limit(uint16_t val)
//{
//	return (val>SERVO_SPEED_MAX)?SERVO_SPEED_MAX:((val<SERVO_SPEED_MIN)?SERVO_SPEED_MIN:val);
//}

//uint32_t Servo_Speed(int16_t ratio)
//{
//	if(ratio<0){
//		return SERVO_SPEED_MIN;
//	}else{
//		return Servo_Limit(ratio*2.88+SERVO_SPEED_MIN);
//	}
//}
