#include "main.h"
u8 pressflagl=0;
u8 lastpressflagl=0;
u8 pressflagr=0;
u8 lastpressflagr=0;
u8 IsInitialSet0=0;
int16_t liftspeed=0,rollspeed=0;


/***************************************************************************************
 *Name     : CatchControl
 *Function £º×¥µ¯ÉÏ²ãº¯Êý
 *Input    £ºÎÞ
 *Output   £ºÎÞ 
 *Description : ¿ØÖÆÒ»¸öÌ§Éýµç»úºÍ2¸ö·­×ªµç»ú
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
		flift();//Ô¤Ì§Éý
		liftspeed=RC_Ex_Ctl.rc.ch3*Kv1;//ËÙ¶È×ª»¯
		
		catchbullet(liftspeed);
	
		
		
		switch(RC_Ex_Ctl.key.v & (KEY_PRESSED_OFFSET_Q | KEY_PRESSED_OFFSET_E ))//¼ì²âQ¼üºÍE¼üÊÇ·ñ±»°´ÏÂ
		{
			case ( KEY_PRESSED_OFFSET_Q)://¼Ð×ÓÇ°Éì
				moveforward();
				break;
			case ( KEY_PRESSED_OFFSET_E)://¼Ð×ÓÊÕ»Ø
				movebackward();
				break;
			default:
				break;
		}

		switch(RC_Ex_Ctl.key.v & (KEY_PRESSED_OFFSET_SHIFT | KEY_PRESSED_OFFSET_CTRL))//¼ì²âSHIFT¼üºÍCTRL¼üÊÇ·ñ±»°´ÏÂ
		{ 
			case(KEY_PRESSED_OFFSET_SHIFT):	   //¼Ð×Ó¼Ð½ô
				catchstart();
				break;
			case(KEY_PRESSED_OFFSET_CTRL):	   //¼Ð×Ó·ÅËÉ
				catchfinish();
				break;
			default: 													  
				break;
		}
		
		if(RC_Ex_Ctl.mouse.press_l)            //·­×ªµç»úÅÐ¶Ï
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
//		  GimbalControl_Pitch(MIDDLE_PITCH);        //¶ÔÓÚPITCHÖá ÓÉÓÚ°å×Ó·½ÏòÓë²½±ø²»Í¬ ´Ë´¦ÎªµÚÒ»¸öÐÞ¸ Ä
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
		fliftdown();//¼Ð×Ó×Ô¶¯ÏÂ½µ»ØÔ­Î»,¹éÔ­Î»Ö®ºó·¢0
//			GimbalPidInit();
//			GimbalControl_PitchAngel(0 - Gimbal_control.angle_pitch_set);           //¶ÔÓÚPITCHÖá ÓÉÓÚ°å×Ó·½ÏòÓë²½±ø²»Í¬ ´Ë´¦ÎªµÚÒ»¸öÐÞ¸ Ä
//			GimbalControl_YawAngel(Gimbal_control.angle_yaw_set + angleSave);		    //yawÖáÎÞÐèÐÞ¸Ä
//	

	}
	
		

}

/***************************************************************************************
 *Name     £º CatchInit
 *Function £º×¥È¡³õÊ¼»¯º¯Êý
 *Input    £ºÎÞ
 *Output   £ºÎÞ 
 *Description £º×¥È¡gpio³õÊ¼»¯
****************************************************************************************/

void CatchInit()
{
	moveinit();
	catchinit();
	liftinit();
	CAN2_Send_Bottom(0,0,0);
}



