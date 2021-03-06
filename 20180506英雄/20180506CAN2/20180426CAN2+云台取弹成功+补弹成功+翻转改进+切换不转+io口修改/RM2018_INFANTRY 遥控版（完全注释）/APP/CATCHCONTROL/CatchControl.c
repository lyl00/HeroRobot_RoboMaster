#include "main.h"
u8 pressflagl=0;
u8 lastpressflagl=0;
u8 pressflagr=0;
u8 lastpressflagr=0;
u8 IsInitialSet0=0;
int16_t liftspeed=0,rollspeed=0;


/***************************************************************************************
 *Name     : CatchControl
 *Function ：抓弹上层函数
 *Input    ：无
 *Output   ：无 
 *Description : 控制一个抬升电机和2个翻转电机
****************************************************************************************/
void CatchControl(void)
{
	if(remoteState == PREPARE_STATE)
	{
		
    TIM_SetCompare1(TIM8,50+100); 
		CAN2_Send_Bottom(0,0,0);
	}
	else if(remoteState == NORMAL_REMOTE_STATE )    
	{
		if(setangle1!=0&&IsInitialSet0==0)
		{
			setangle1=current_angle_201;
			setangle2=-setangle1;
			IsInitialSet0=1;
		}
		
		TIM_SetCompare1(TIM8,50+250 ); 
		flift();//预抬升
		liftspeed=RC_Ex_Ctl.rc.ch3*Kv1;//速度转化
		
		catchbullet(liftspeed);
	
		
		
		switch(RC_Ex_Ctl.key.v & (KEY_PRESSED_OFFSET_Q | KEY_PRESSED_OFFSET_E ))//检测Q键和E键是否被按下
		{
			case ( KEY_PRESSED_OFFSET_Q)://夹子前伸
				moveforward();
				break;
			case ( KEY_PRESSED_OFFSET_E)://夹子收回
				movebackward();
				break;
			default:
				break;
		}

		switch(RC_Ex_Ctl.key.v & (KEY_PRESSED_OFFSET_SHIFT | KEY_PRESSED_OFFSET_CTRL))//检测SHIFT键和CTRL键是否被按下
		{ 
			case(KEY_PRESSED_OFFSET_SHIFT):	   //夹子夹紧
				catchstart();
				break;
			case(KEY_PRESSED_OFFSET_CTRL):	   //夹子放松
				catchfinish();
				break;
			default: 													  
				break;
		}
		
		if(RC_Ex_Ctl.mouse.press_l)            //翻转电机判断
		{
			pressflagl=1;
		}
		else 
		{
			pressflagl=0;
			if(lastpressflagl)
				RotateFlag=1;
		}              //why
		lastpressflagl=pressflagl;  
		if(RC_Ex_Ctl.mouse.press_r)
		{
			pressflagr=1;
		}
		else 
		{
			pressflagr=0;
			if(lastpressflagr)
			 	RotateFlag=-1;
		}
		lastpressflagr=pressflagr;
	}

//		  GimbalPidInitPrepare();
//		  GimbalControl_Pitch(MIDDLE_PITCH);        //对于PITCH轴 由于板子方向与步兵不同 此处为第一个修� �
//		  GimbalControl_Yaw(0);
//		  angleSave=- ( Yaw - Yaw_Offset ) *57.3f;
//		
//		  TIM_SetCompare1(TIM8,749); 
		
	else if(remoteState == STANDBY_STATE )    //????
	{
//		  return;	
    IsInitialSet0=0;		
    TIM_SetCompare1(TIM8,50+90);
		CAN2_Send_Bottom(0,0,0);
	}
	else if(remoteState == ERROR_STATE )      //????
	{
		IsInitialSet0=0;
		TIM_SetCompare1(TIM8,50+100);
		CAN2_Send_Bottom(0,0,0);
//		  return;
	}
	else if(remoteState == KEY_REMOTE_STATE ) //????
	{
		IsInitialSet0=0;
		TIM_SetCompare1(TIM8,50+100);
		fliftdown();//夹子自动下降回原位,归原位之后发0
//			GimbalPidInit();
//			GimbalControl_PitchAngel(0 - Gimbal_control.angle_pitch_set);           //对于PITCH轴 由于板子方向与步兵不同 此处为第一个修� �
//			GimbalControl_YawAngel(Gimbal_control.angle_yaw_set + angleSave);		    //yaw轴无需修改
//	

	}
	
		

}

/***************************************************************************************
 *Name     ： CatchInit
 *Function ：抓取初始化函数
 *Input    ：无
 *Output   ：无 
 *Description ：抓取gpio初始化
****************************************************************************************/

void CatchInit()
{
	moveinit();
	catchinit();
	liftinit();
	CAN2_Send_Bottom(0,0,0);
}



