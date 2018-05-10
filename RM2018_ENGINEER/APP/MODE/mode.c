/************************************************************
 *File		:	mode.c
 *Author	:  	@YangTao, ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.3.3
 *Description: 	Engineer robot working mode control
 ************************************************************/

#include "main.h"

Mode_WorkingMode GlobalMode;

/*-------------  模式控制初始化  -------------*/
void Mode_Configuration(void)
{
	GlobalMode = MODE_DEFAULT;		//初始模式
}

/*-------------  模式控制控制循环  -------------*/
/***************************************************************************************
 *Name     : 
 *Function ：
 *Input    ：无
 *Output   ：无 
 *Description : 
****************************************************************************************/
void Mode_Loop(void)
{
//	static Mode_WorkingMode last_GlobleMode;
//	
//	//状态切换
//	if(last_GlobleMode!=GlobalMode){
//		switch(GlobalMode){
//			case MODE_DRAG: 
//				BUZZER_ON(1500);delay_ms(150);BUZZER_OFF();delay_ms(150);
//				BUZZER_ON(1500);delay_ms(150);BUZZER_OFF();
//				break;
//			case MODE_LIFT:
//				BUZZER_ON(1500);delay_ms(150);BUZZER_OFF();delay_ms(150);
//				BUZZER_ON(1500);delay_ms(150);BUZZER_OFF();delay_ms(150);
//				BUZZER_ON(1500);delay_ms(150);BUZZER_OFF();
//				break;
//			case MODE_BULLET: 
//				BUZZER_ON(1500);delay_ms(150);BUZZER_OFF();
//				break;
//			case MODE_DEFAULT:
//				break;
//		}
//	}
//	
//	last_GlobleMode = GlobalMode;
}
