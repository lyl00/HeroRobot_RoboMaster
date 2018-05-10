/************************************************************
 *File		:	GimbalControl.c
 *Author	:  @YangTianhao ,490999282@qq.com,@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Control Gimbal Motors.
								GimbalContPitchoop() shows the way to control the motion of Gimbal in different states. 
								Use PID to optimize Gimbal motor control.
 ************************************************************/
#include "main.h"

#define MOUSE_YAW_SPEED 0.004
#define MOUSE_PIT_SPEED 0.005

Gimbal_Control_t Gimbal_control; //??????,??pid???pid
MotorPID_t	MotorPID_pitch;	  //Pitch?PID??
MotorPID_t	MotorPID_yaw;     //Yaw?PID??
float PIDOut_Position_Pit,PIDOut_Speed_Pit,PIDOut_Whole_Pit;					//??pid??????????
float PIDOut_Position_Yaw,PIDOut_Speed_Yaw,PIDOut_Whole_Yaw,PIDOut_Electricity_Yaw;//??pid??????????
float angleSave;       //???????????
int16_t position_yaw_relative = 0; //yaw???MIDDLE_YAW????
int16_t tch3;//??????ban?pitch
u8 IsInitialKey=0;

	
/*-------------  ??????  -------------*/
void GimbalControlLoop(void)
{	
	position_yaw_relative = GimbalValLigal(current_position_205	,	MIDDLE_YAW);  //???????????,????????
	tch3=RC_Ex_Ctl.rc.ch3;
	if (remoteState == NORMAL_REMOTE_STATE)
	{
		tch3=0;
	}
	GimbalControlValSet();				//???Gimbal_control?????,?????????
	GimbalMove();									//????????,??????
	CAN1_Cmd_All((int16_t)-PIDOut_Whole_Yaw, (int16_t)PIDOut_Whole_Pit);		//?CAN????????????,????
}


/*-------------  ?????pid???  -------------*/
void GimbalPidInitPrepare(void)
{
	//PITCH?PID??
	PID_Init(&MotorPID_pitch.Position, PIDVAL_PITCH_POS_p_Prepare , PIDVAL_PITCH_POS_i_Prepare, PIDVAL_PITCH_POS_d_Prepare,-120,120,0);    //15 1.5 0.04 
	SetPIDCR(&MotorPID_pitch.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_pitch.Speed,	PIDVAL_PITCH_SPEED_p_Prepare, PIDVAL_PITCH_SPEED_i_Prepare, PIDVAL_PITCH_SPEED_d_Prepare,-60 ,60 ,0);//13 0 0
	SetPIDCR(&MotorPID_pitch.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
	//YAW?PID??
	PID_Init(&MotorPID_yaw.Position, PIDVAL_YAW_POS_p_Prepare, PIDVAL_YAW_POS_i_Prepare	, PIDVAL_YAW_POS_d_Prepare,-400,400,0);    //7.0 view 10 30 0.03
	SetPIDCR(&MotorPID_yaw.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_yaw.Speed,	PIDVAL_YAW_SPEED_p_Prepare,	PIDVAL_YAW_SPEED_i_Prepare,	PIDVAL_YAW_SPEED_d_Prepare,-60,60,0);    //13.55    //view 25 0 0
	SetPIDCR(&MotorPID_yaw.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
}	

/*-------------  ??????pid???  -------------*/
void GimbalPidInit(void)
{
	//PITCH?PID??
	PID_Init(&MotorPID_pitch.Position,PIDVAL_PITCH_POS_p, 	PIDVAL_PITCH_POS_i, 	PIDVAL_PITCH_POS_d,-120,120,0);    //15 1.5 0.04 
	SetPIDCR(&MotorPID_pitch.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_pitch.Speed,		PIDVAL_PITCH_SPEED_p, PIDVAL_PITCH_SPEED_i, PIDVAL_PITCH_SPEED_d,-60,60,0);//13 0 0
	SetPIDCR(&MotorPID_pitch.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
	//YAW?PID??
	PID_Init(&MotorPID_yaw.Position,PIDVAL_YAW_POS_p,		PIDVAL_YAW_POS_i,		PIDVAL_YAW_POS_d,	-300,300,0);    //7.0 view 10 30 0.03
	SetPIDCR(&MotorPID_yaw.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_yaw.Speed,		PIDVAL_YAW_SPEED_p,	PIDVAL_YAW_SPEED_i,	PIDVAL_YAW_SPEED_d,-60,60,0);    //13.55    //view 25 0 0
	SetPIDCR(&MotorPID_yaw.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
}

/*-------------  ??????  -------------*/
void GimbalStop(void)
{
	Gimbal_control.angle_pitch_set 	= 0;
	Gimbal_control.angle_yaw_set 	= -(Yaw *57.3f - Yaw_Offset *57.3f);    //???????????
	
	PIDOut_Whole_Yaw = 0;
	PIDOut_Whole_Pit = 0;
}

/*-------------  ??????  -------------*/
float MotorPositionLegalize(float PositionPara , float MAX , float MIN)
{
	float temp;
	temp= (PositionPara<MIN) ?  MIN:( (PositionPara>MAX) ? MAX : PositionPara)  ;
	return temp;
}

/*-------------  ??????  -------------*/
float MotorCurrentLegalize(float MotorCurrent , float limit)
{
	return MotorCurrent<-limit?-limit:(MotorCurrent>limit?limit:MotorCurrent);
}


/***************************************************************************************
 *Name     : GimbalValLigal
 *Function :????????
 *Input    :raw_gimbal_data,middle_data
 *Output   :raw_gimbal_data
 *Description : ??yaw???????????????,????????
****************************************************************************************/
int16_t GimbalValLigal(int raw_gimbal_data,int middle_data)  //???0????????????,??????yaw?????180?,??????
{
	if((raw_gimbal_data - middle_data)>4096 )
		raw_gimbal_data -= 8192;
	if((raw_gimbal_data - middle_data)<-4096 )
		raw_gimbal_data += 8192;
	
	raw_gimbal_data -= middle_data;            //??yaw???????????????
	return raw_gimbal_data;
}


/***************************************************************************************
 *Name     : GimbalControlValSet
 *Function :????????? 
 *Input    :?
 *Output   :? 
 *Description : ???????	Gimbal_control.angle_yaw_set,Gimbal_control.angle_pitch_set
								Gimbal_control.angle_yaw_current,Gimbal_control.angle_pitch_current
****************************************************************************************/
//int i=100;
void GimbalControlValSet(void)	
{
	if(remoteState == PREPARE_STATE)
	{
		 Gimbal_control.angle_pitch_set 	= 0;
		 Gimbal_control.angle_yaw_set 		= 0;
	}
	else if(remoteState == NORMAL_REMOTE_STATE )     //Use remote 
{
	IsInitialKey=0;
	
//////	Gimbal_control.angle_pitch_set 					=0;
//////	Gimbal_control.angle_yaw_set	          =0;
////////			if((position_yaw_relative < YawMax)&&(position_yaw_relative>YawMin))    //????????????????,?????????????
////////			{				
////////				Gimbal_control.angle_yaw_set += 0.0002f * (RC_Ex_Ctl.rc.ch2);   //yaw??????????????
////////			}
////////			else if(position_yaw_relative > YawMax)     //????????????????,??????????(yaw??????????,??????????,?????????????????)
////////				Gimbal_control.angle_yaw_set += 0.012f;
////////			else if(position_yaw_relative < YawMin)
////////				Gimbal_control.angle_yaw_set -= 0.012f;
////////			
//////	//		Gimbal_control.angle_pitch_set += 0.0002f	*	(tch3);      //Pitch??????????????
////////			
//			Gimbal_control.angle_yaw_current = -(Yaw   - Yaw_Offset  ) *57.3f;          //????Yaw???,????Yaw????
////////			#ifdef PITCH_BORDER                                                       //??
//////			Gimbal_control.angle_pitch_current = Pitch *57.3f;      //???PITCH??????Pitch?,????Pitch???,??GimbalControl_PitchAngel()???
////////			#endif
//			  
//			#ifdef ROLL_BORDER                                                        //??
//			Gimbal_control.angle_pitch_current = -(Roll  - Roll_Offset) *57.3f;      //???PITCH??????Pitch?,????Pitch???,??GimbalControl_PitchAngel()???
//			#endif
//			
//			if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)> 180) 	Gimbal_control.angle_yaw_set -=360;  								//YAW? test????
//		    if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)<-180) 	Gimbal_control.angle_yaw_set +=360;
//			
//			Gimbal_control.angle_pitch_set = MotorPositionLegalize(Gimbal_control.angle_pitch_set,	PitMax, PitMin);  //pitch??????
	}
	else if(remoteState == KEY_REMOTE_STATE ) 
	{
			
		
		  Gimbal_control.angle_pitch_set 					-= MOUSE_PIT_SPEED * RC_Ex_Ctl.mouse.y;
			
		
		if((position_yaw_relative < YawMax)&&(position_yaw_relative>YawMin))    //????????????????,?????????????
			{				
				if(RC_Ex_Ctl.mouse.x>30)
				{RC_Ex_Ctl.mouse.x=30;}
				else if(RC_Ex_Ctl.mouse.x<-30)
				{RC_Ex_Ctl.mouse.x=-30;}
				Gimbal_control.angle_yaw_set	          += MOUSE_YAW_SPEED * RC_Ex_Ctl.mouse.x;
			}
			else if(position_yaw_relative > YawMax)     //????????????????,??????????(yaw??????????,??????????,?????????????????)
				Gimbal_control.angle_yaw_set += 0.012f;
			else if(position_yaw_relative < YawMin)
				Gimbal_control.angle_yaw_set -= 0.012f;
		

			Gimbal_control.angle_yaw_current = -(Yaw   - Yaw_Offset  ) *57.3f;        //»ñµÃµ±Ç°YawÖá½Ç¶È£¬ÓÃÀ´¿ØÖÆYawÖáÎ»ÖÃ»·
		if(Gimbal_control.angle_yaw_set-Gimbal_control.angle_yaw_current>=YawMax)
		{
			Gimbal_control.angle_yaw_set=YawMax;
		}
		else if(Gimbal_control.angle_yaw_set-Gimbal_control.angle_yaw_current<=YawMin)
		{
			Gimbal_control.angle_yaw_set=YawMin;
		}
		
		//Gimbal_control.angle_yaw_set = MotorPositionLegalize(Gimbal_control.angle_yaw_set,	YawMax, YawMin);
		
			Gimbal_control.angle_pitch_current = (Pitch  - Pitch_Offset) *57.3f;       //ÔÆÌ¨µÄPITCHÖá¶ÔÓ¦ÍÓÂÝÒÇROLLÖá£¬»ñµÃµ±Ç°PitchÖá½Ç¶È£¬½öÔÚGimbalControl_PitchAngel()ÖÐÊ¹ÓÃ
					
			if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)> 180) 	Gimbal_control.angle_yaw_set -=360;  								//YAWÖá test³¬¹ýÒ»È¦
		  if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)<-180) 	Gimbal_control.angle_yaw_set +=360;
			
			Gimbal_control.angle_pitch_set = MotorPositionLegalize(Gimbal_control.angle_pitch_set,	PitMax, PitMin);  //pitchÖá¸©Ñö½ÇÏÞÎ»
		  //Auto_aim(USART_RX_BUF3,100);
		
//		  if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)> 180) 	Gimbal_control.angle_yaw_set -=360;  								//YAW? test????
//		  if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)<-180) 	Gimbal_control.angle_yaw_set +=360;
//			
		  Gimbal_control.angle_pitch_set = MotorPositionLegalize(Gimbal_control.angle_pitch_set,	PitMax, PitMin);
//			if(i<=0)
//			{
//				printf("yawset=%d\n",(int)Gimbal_control.angle_yaw_set);
//				printf("pitchset=%d\n",(int)Gimbal_control.angle_pitch_set);
//				i=100;
//			}
//			i--;
	
	}
	else if(remoteState == STANDBY_STATE ) 
	{
		  IsInitialKey=0;
		  remoteState_ex = STANDBY_STATE;
		  GimbalStop();	
	}
//	else if(remoteState == VIEW_STATE ) 
//	{
//		  Gimbal_control.angle_yaw_current = -(Yaw   - Yaw_Offset  ) *57.3f;        //????Yaw???,????Yaw????
//		
//		  #ifdef PITCH_BORDER//??
//			Gimbal_control.angle_pitch_current = (Pitch  - Pitch_Offset) *57.3f;      //???PITCH??????Pitch?,????Pitch???,??GimbalControl_PitchAngel()???
//			#endif
//			
//			#ifdef ROLL_BORDER//??
//			Gimbal_control.angle_pitch_current = -(Roll  - Roll_Offset) *57.3f;      //???PITCH??????Pitch?,????Pitch???,??GimbalControl_PitchAngel()???
//			#endif
//			
////		  Auto_aim(USART_RX_BUF3,100);
//		
//		  if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)> 180) 	Gimbal_control.angle_yaw_set -=360;  								//YAW? test????
//		  if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)<-180) 	Gimbal_control.angle_yaw_set +=360;
//			
//		  Gimbal_control.angle_pitch_set = MotorPositionLegalize(Gimbal_control.angle_pitch_set,	PitMax, PitMin);  //pitch??????
//			Gimbal_control.angle_yaw_set = MotorPositionLegalize(Gimbal_control.angle_yaw_set,	YawMax, YawMin);
//	}
		else if(remoteState == ERROR_STATE )   //????
	{
		  remoteState_ex = ERROR_STATE;
		  GimbalStop();
	}
}


/***************************************************************************************
 *Name     : GimbalMove
 *Function :????????
 *Input    :?
 *Output   :? 
 *Description : ?????????????? 
****************************************************************************************/
void GimbalMove(void)
{
	if(remoteState == PREPARE_STATE)
	{
		  GimbalControl_Pitch(MIDDLE_PITCH);           //?????pitch???
		  GimbalControl_Yaw (0);                       //?????yaw???
		 // angleSave = - ( Yaw - Yaw_Offset ) *57.3f;   //??PREPARE_STATE???Yaw????
	}
	else if(remoteState == NORMAL_REMOTE_STATE )    
	{
		
	
			GimbalPidInitPrepare();
		  GimbalControl_Pitch(MIDDLE_PITCH);        //¶ÔÓÚPITCHÖá ÓÉÓÚ°å×Ó·½ÏòÓë²½±ø²»Í¬ ´Ë´¦ÎªµÚÒ»¸öÐÞ¸ Ä
		  GimbalControl_Yaw(0);
		//  angleSave=- ( Yaw - Yaw_Offset ) *57.3f;
		
		
		//  TIM_SetCompare1(TIM8,749); 
	}
	else if(remoteState == STANDBY_STATE )    //????
	{
		  return;		 
	}
	else if(remoteState == ERROR_STATE )      //????
	{
		  return;
	}
	else if(remoteState == KEY_REMOTE_STATE ) //????
	{
			GimbalPidInit();
		if(IsInitialKey==0)
		{
			Gimbal_control.angle_yaw_set	=Gimbal_control.angle_yaw_current;

			IsInitialKey=1;
		}
		
			GimbalControl_PitchAngel(0 - Gimbal_control.angle_pitch_set);           //¶ÔÓÚPITCHÖá ÓÉÓÚ°å×Ó·½ÏòÓë²½±ø²»Í¬ ´Ë´¦ÎªµÚÒ»¸öÐÞ¸ Ä
			GimbalControl_YawAngel(Gimbal_control.angle_yaw_set/* + angleSave*/);		    //yawÖáÎÞÐèÐÞ¸Ä
	

	}
//	else if(remoteState == VIEW_STATE )       //????
//	{
//		  GimbalControl_PitchAngel(0 - Gimbal_control.angle_pitch_set);
//		  GimbalControl_YawAngel(Gimbal_control.angle_yaw_set + angleSave);
//	}

}



/*--------------------------------------------  PITCH ????  --------------------------------------------*/

/***************************************************************************************
 *Name     : GimbalControl_PitchAngel
 *Function :pitch?????
 *Input    :setPosition
 *Output   :? 
 *Description : ????????????,?pitch????????????
****************************************************************************************/
void GimbalControl_PitchAngel(float SetPosition)
{
	float NowPosition = Gimbal_control.angle_pitch_current;						  //pitch??????
	
	#ifdef PITCH_BORDER //??
	float NowSpeed = -mpu6500_real_data.Gyro_Y * 57.3f;					        //pitch???????
	#endif
	
  #ifdef ROLL_BORDER //??
	float NowSpeed = mpu6500_real_data.Gyro_X * 57.3f;					        //pitch???????
	#endif
	
	SetPosition = SetPosition*22.756f;//8192/360.0f;										//??????????(0~360 to 0~8191)
	NowPosition = NowPosition*22.756f;//8192/360.0f;

	PIDOut_Position_Pit = PID_Driver(SetPosition, NowPosition, &MotorPID_pitch.Position,2000);        //´Ë´¦ÎªµÚ¶þ¸ö¶ÔÓÚPITCHÖáµÄÐÞ¸Ä
	PIDOut_Speed_Pit 	= PID_Driver(-PIDOut_Position_Pit/20.0f,-NowSpeed,&MotorPID_pitch.Speed,1000); //

	PIDOut_Whole_Pit = - MotorCurrentLegalize(PIDOut_Speed_Pit,5000); //???,????
}

/***************************************************************************************
 *Name     : GimbalControl_Pitch
 *Function :pitch?????
 *Input    :setPosition
 *Output   :? 
 *Description : ????????,????????,?pitch??????????????
****************************************************************************************/
void GimbalControl_Pitch(float SetPosition)
{
	float NowPosition = current_position_206;						               //pitch??????
	
	#ifdef PITCH_BORDER//??
	float NowSpeed = -mpu6500_real_data.Gyro_Y * 57.3f;					         //pitch???????
	#endif
	
	#ifdef ROLL_BORDER//??
	float NowSpeed = mpu6500_real_data.Gyro_X * 57.3f;					         //pitch???????
	#endif
	

	PIDOut_Position_Pit = PID_Driver(SetPosition, NowPosition, &MotorPID_pitch.Position,2000);       //?????
	PIDOut_Speed_Pit    = PID_Driver(PIDOut_Position_Pit/20.0f,NowSpeed,&MotorPID_pitch.Speed,1000); //?????

	PIDOut_Whole_Pit =  MotorCurrentLegalize(PIDOut_Speed_Pit,5000);  //???,????
}


/*--------------------------------------------  YAW ????  --------------------------------------------*/

/***************************************************************************************
 *Name     : GimbalControl_Yaw
 *Function :yaw?????
 *Input    :setPosition
 *Output   :? 
 *Description : ???????,???????,????yaw???????
****************************************************************************************/
void GimbalControl_Yaw(float SetPosition)
{
	//????????,????
	float NowPosition = position_yaw_relative;						        //yaw??????	
	float NowSpeed = -mpu6500_real_data.Gyro_Z * 57.3f;						//yaw???????
	
	PIDOut_Position_Yaw = PID_Driver(SetPosition, NowPosition, &MotorPID_yaw.Position , 2000);       //?????
	PIDOut_Speed_Yaw		= PID_Driver(-PIDOut_Position_Yaw/20.0f,-NowSpeed,&MotorPID_yaw.Speed , 1000);//?????
	
	PIDOut_Whole_Yaw = MotorCurrentLegalize(PIDOut_Speed_Yaw,5000);  //???,????

	PIDOut_Whole_Yaw = -PIDOut_Whole_Yaw;
}

/***************************************************************************************
 *Name     : GimbalControl_YawAngel
 *Function :yaw?????
 *Input    :setPosition
 *Output   :? 
 *Description : ????????????,?yaw??????????????
****************************************************************************************/
void GimbalControl_YawAngel(float SetPosition)
{
	float NowPosition = Gimbal_control.angle_yaw_current;						//yaw??????	
	float NowSpeed = -mpu6500_real_data.Gyro_Z * 57.3f;					      //yaw???????
	
	SetPosition = SetPosition*22.756f;//8192/360.0f;									//??????????(0~360 to 0~8191)
	NowPosition = NowPosition*22.756f;//8192/360.0f;
	
//	if(SetPosition-NowPosition>YawMax)
//	{
//		SetPosition=NowPosition+YawMax;
//	}
//	else if(SetPosition-NowPosition<YawMin)
//	{
//		SetPosition=NowPosition+YawMin;
//	}
	PIDOut_Position_Yaw = PID_Driver(SetPosition, NowPosition, &MotorPID_yaw.Position , 2000);      //?????
	PIDOut_Speed_Yaw		= PID_Driver(PIDOut_Position_Yaw/20.0f,-NowSpeed,&MotorPID_yaw.Speed , 1000);//?????
	
	PIDOut_Whole_Yaw = MotorCurrentLegalize(PIDOut_Speed_Yaw,10000);  //???,????

	PIDOut_Whole_Yaw = -PIDOut_Whole_Yaw;//ÕâÀïÈ¥µôÁËÒ»¸ö¸ººÅ

}


/********  ?????  **********/
void GimbalControlInit(void)
{
	GimbalPidInitPrepare();   //?????pid,???????
	GimbalStop();							//??????
	CAN1_Cmd_All((int16_t)PIDOut_Whole_Yaw, (int16_t)PIDOut_Whole_Pit );	 
}







