/************************************************************
 *File		:	
 *Author	:  	@YangTao, ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.3.3
 *Description: 	Duct contorl
				TIM8_CH1(PI5)
 ************************************************************/

#include "main.h"

#define DUCT_SPEED_MIN		1000		// 1ms
#define DUCT_SPEED_MAX		2000		// 2ms
#define DUCT_SPEED_CLSOE	DUCT_SPEED_MIN
#define DUCT_SPEED_OPEN		1500		// 1.5ms


uint32_t Duct_Limit(uint16_t val);
uint32_t Duct_Speed(int16_t ratio);


/*-------------  涵道控制初始化  -------------*/
//用摩擦轮的PWM控制
void Duct_Configuration(void)
{	
	TIM_SetCompare1(TIM4,DUCT_SPEED_MAX);
	TIM_SetCompare2(TIM4,DUCT_SPEED_MAX);
	delay_ms(500);
	TIM_SetCompare1(TIM4,DUCT_SPEED_MIN);
	TIM_SetCompare2(TIM4,DUCT_SPEED_MIN);
	delay_ms(1000);
}


/*-------------  涵道控制控制循环  -------------*/
/***************************************************************************************
 *Name     : 
 *Function ：
 *Input    ：无
 *Output   ：无 
 *Description : 
****************************************************************************************/
void Duct_Loop(void)
{
	#ifdef RC_FUNC
//	if(GlobalMode==MODE_LIFT){
//		//上拨开，下拨关
//		if(RemoteSEF_REG&RemoteSEF_S1_UP){
//			TIM_SetCompare1(TIM4,DUCT_SPEED_OPEN);
//			TIM_SetCompare1(TIM4,DUCT_SPEED_OPEN);
//		}else if(RemoteSEF_REG&RemoteSEF_S1_DOWN){
//			TIM_SetCompare1(TIM4,DUCT_SPEED_CLSOE);
//			TIM_SetCompare1(TIM4,DUCT_SPEED_CLSOE);
//		}
//	}
//	else{
//		TIM_SetCompare1(TIM4,DUCT_SPEED_CLSOE);
//		TIM_SetCompare1(TIM4,DUCT_SPEED_CLSOE);
//	}
	#else
	if(GlobalMode==MODE_LIFT){
		if(Remote_KEYVal&KEY_PRESSED_OFFSET_A){
			TIM_SetCompare1(TIM4,DUCT_SPEED_OPEN);
		}else{
			TIM_SetCompare1(TIM4,DUCT_SPEED_CLSOE);
		}
		
		if(Remote_KEYVal&KEY_PRESSED_OFFSET_D){
			TIM_SetCompare2(TIM4,DUCT_SPEED_OPEN);
		}else{
			TIM_SetCompare2(TIM4,DUCT_SPEED_CLSOE);
		}
	}
	else{
		TIM_SetCompare1(TIM4,DUCT_SPEED_CLSOE);
		TIM_SetCompare1(TIM4,DUCT_SPEED_CLSOE);
	}
	#endif
}

//uint32_t Duct_Limit(uint16_t val)
//{
//	return (val>DUCT_SPEED_MAX)?DUCT_SPEED_MAX:((val<DUCT_SPEED_MIN)?DUCT_SPEED_MIN:val);
//}

//uint32_t Duct_Speed(int16_t ratio)
//{
//	if(ratio<0){
//		return DUCT_SPEED_MIN;
//	}else{
//		return Duct_Limit(ratio+980);
//	}
//}
