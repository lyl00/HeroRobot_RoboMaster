/************************************************************
 *File		:	
 *Author	:  	@YangTao, ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.3.3
 *Description: 	Camera contorl
				TIM8_CH3(PI7) for yaw
				TIM8_CH4(PI2) for pitch
 ************************************************************/

#include "main.h"

struct CAMERA_FLAG_REG {
	uint8_t 			CameraDragLow:1;		//Low pitch for Drag
} CAMERA_FLAG;

//uint16_t Camera_Limit(uint16_t val);

/*-------------  涵道控制初始化  -------------*/
//用摩擦轮的PWM控制
void Camera_Configuration(void)
{
	CAMERA_FLAG.CameraDragLow = 0;
	//注意 timer.c 的初始化
}


/*-------------  涵道控制控制循环  -------------*/
/***************************************************************************************
 *Name     : 
 *Function ：
 *Input    ：无
 *Output   ：无 
 *Description :
****************************************************************************************/

void Camera_Loop(void)
{
	static uint32_t camera_loop_count = 0;
	camera_loop_count++;
	
	// Pitch low?
	if(GlobalMode==MODE_DRAG){
		if(Remote_KEYVal&KEY_PRESSED_OFFSET_S){
			CAMERA_FLAG.CameraDragLow = 1;
		}else if(Remote_KEYVal&KEY_PRESSED_OFFSET_W){
			CAMERA_FLAG.CameraDragLow = 0;
		}
	}else{
		CAMERA_FLAG.CameraDragLow = 0;
	}
	
	if(camera_loop_count%99==0){
		//切换Yaw位置
		switch(GlobalMode){
		case MODE_LIFT:
			TIM_SetCompare3(CAMERA_TIMx,CAMERA_DUTY_LIFT_yaw);
			break;
		case MODE_BULLET:
			TIM_SetCompare3(CAMERA_TIMx,CAMERA_DUTY_BULLET_yaw);
			break;
		case MODE_DRAG:
			TIM_SetCompare3(CAMERA_TIMx,CAMERA_DUTY_DARG_yaw);
			break;
		default:
			break;
		}
		//切换Pitch位置
		TIM_SetCompare4(CAMERA_TIMx,(CAMERA_FLAG.CameraDragLow)?CAMERA_DUTY_DRAG_pitch:CAMERA_DUTY_pitch);
	}
}

