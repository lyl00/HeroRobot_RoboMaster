/************************************************************
 *File		:	GimbalControl.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Control Gimbal Motors.
								GimbalControlLoop() shows the way to control the motion of Gimbal in different states. 
								Use PID to optimize Gimbal motor control.
 ************************************************************/
#include "main.h"

Gimbal_Control_t Gimbal_control; //云台控制结构，位置pid、速度pid
MotorPID_t	MotorPID_pitch;	  //Pitch轴PID结构
MotorPID_t	MotorPID_yaw;     //Yaw轴PID结构
frame unpack_fream;           //与视觉通信帧结构
float PIDOut_Position_Pit,PIDOut_Speed_Pit,PIDOut_Whole_Pit;					//位置pid临时输出和位置总输出
float PIDOut_Position_Yaw,PIDOut_Speed_Yaw,PIDOut_Whole_Yaw,PIDOut_Electricity_Yaw;//速度pid临时输出和速度总输出
float angleSave;       //陀螺仪角度临时存储变量
int16_t position_yaw_relative = 0; //yaw轴相对MIDDLE_YAW的码盘值

	
/*-------------  云台控制循环  -------------*/
void GimbalControlLoop(void)
{	
	position_yaw_relative 	= GimbalValLigal(current_position_205	,	MIDDLE_YAW);  //将当前码盘值与中值做差，得到相对码盘差值
	GimbalControlValSet();				//计算出Gimbal_control结构体的值，即需要达到的控制值
	GimbalMove();									//根据不同的状态机，有不同的输出
	CAN1_Cmd_All((int16_t)PIDOut_Whole_Yaw, (int16_t)PIDOut_Whole_Pit , (int16_t)PIDOut_Whole_Shoot);		//向CAN总线发送云台电机转动数据，为电流值
}


/*-------------  云台软起动pid初始化  -------------*/
void GimbalPidInitPrepare(void)
{
	//PITCH轴PID设置
	PID_Init(&MotorPID_pitch.Position, PIDVAL_PITCH_POS_p_Prepare , PIDVAL_PITCH_POS_i_Prepare, PIDVAL_PITCH_POS_d_Prepare,-120,120,0);    //15 1.5 0.04 
	SetPIDCR(&MotorPID_pitch.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_pitch.Speed,	PIDVAL_PITCH_SPEED_p_Prepare, PIDVAL_PITCH_SPEED_i_Prepare, PIDVAL_PITCH_SPEED_d_Prepare,-60 ,60 ,0);//13 0 0
	SetPIDCR(&MotorPID_pitch.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
	//YAW轴PID设置
	PID_Init(&MotorPID_yaw.Position, PIDVAL_YAW_POS_p_Prepare, PIDVAL_YAW_POS_i_Prepare	, PIDVAL_YAW_POS_d_Prepare,-400,400,0);    //7.0 view 10 30 0.03
	SetPIDCR(&MotorPID_yaw.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_yaw.Speed,	PIDVAL_YAW_SPEED_p_Prepare,	PIDVAL_YAW_SPEED_i_Prepare,	PIDVAL_YAW_SPEED_d_Prepare,-60,60,0);    //13.55    //view 25 0 0
	SetPIDCR(&MotorPID_yaw.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
}	

/*-------------  云台正常启动pid初始化  -------------*/
void GimbalPidInit(void)
{
	//PITCH轴PID设置
	PID_Init(&MotorPID_pitch.Position,PIDVAL_PITCH_POS_p, 	PIDVAL_PITCH_POS_i, 	PIDVAL_PITCH_POS_d,-120,120,0);    //15 1.5 0.04 
	SetPIDCR(&MotorPID_pitch.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_pitch.Speed,		PIDVAL_PITCH_SPEED_p, PIDVAL_PITCH_SPEED_i, PIDVAL_PITCH_SPEED_d,-60,60,0);//13 0 0
	SetPIDCR(&MotorPID_pitch.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
	//YAW轴PID设置
	PID_Init(&MotorPID_yaw.Position,PIDVAL_YAW_POS_p,		PIDVAL_YAW_POS_i,		PIDVAL_YAW_POS_d,	-300,300,0);    //7.0 view 10 30 0.03
	SetPIDCR(&MotorPID_yaw.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_yaw.Speed,		PIDVAL_YAW_SPEED_p,	PIDVAL_YAW_SPEED_i,	PIDVAL_YAW_SPEED_d,-60,60,0);    //13.55    //view 25 0 0
	SetPIDCR(&MotorPID_yaw.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
}

/*-------------  云台停止运动  -------------*/
void GimbalStop(void)
{
	Gimbal_control.angle_pitch_set 	= 0;
	Gimbal_control.angle_yaw_set 	= -(Yaw *57.3f - Yaw_Offset *57.3f);    //保证切换状态时不会狂抖
	
	PIDOut_Whole_Yaw = 0;
	PIDOut_Whole_Pit = 0;
}

/*-------------  位置幅值函数  -------------*/
float MotorPositionLegalize(float PositionPara , float MAX , float MIN)
{
	float temp;
	temp= (PositionPara<MIN) ?  MIN:( (PositionPara>MAX) ? MAX : PositionPara)  ;
	return temp;
}

/*-------------  电流幅值函数  -------------*/
float MotorCurrentLegalize(float MotorCurrent , float limit)
{
	return MotorCurrent<-limit?-limit:(MotorCurrent>limit?limit:MotorCurrent);
}


/***************************************************************************************
 *Name     : GimbalValLigal
 *Function ：云台电机读值限制
 *Input    ：raw_gimbal_data，middle_data
 *Output   ：raw_gimbal_data
 *Description : 获得yaw轴当前位置相对中值位置的码盘值，防止云台上电疯转
****************************************************************************************/
int16_t GimbalValLigal(int raw_gimbal_data,int middle_data)  //当码盘0值在当前位置和中值之间时，为了避免云台yaw轴转过超过180度，做如下规范化
{
	if((raw_gimbal_data - middle_data)>4096 )
		raw_gimbal_data -= 8192;
	if((raw_gimbal_data - middle_data)<-4096 )
		raw_gimbal_data += 8192;
	
	raw_gimbal_data -= middle_data;            //获得yaw轴当前位置相对中值位置的码盘值
	return raw_gimbal_data;
}


/***************************************************************************************
 *Name     : GimbalControlValSet
 *Function ：云台两轴角度值获取 
 *Input    ：无
 *Output   ：无 
 *Description : 改变了全局变量	Gimbal_control.angle_yaw_set，Gimbal_control.angle_pitch_set
								Gimbal_control.angle_yaw_current，Gimbal_control.angle_pitch_current
****************************************************************************************/
void GimbalControlValSet(void)	
{
	if(remoteState == PREPARE_STATE)
	{
		Gimbal_control.angle_pitch_set 	= 0;
		Gimbal_control.angle_yaw_set 		= 0;
	}
	else if(remoteState == NORMAL_REMOTE_STATE )     //Use remote 
	{
			if((position_yaw_relative < YawMax)&&(position_yaw_relative>YawMin))    //当现在码盘值距中值在一定范围内时，转角与摇杆值呈线性比例关系
			{				
				Gimbal_control.angle_yaw_set += 0.0002f * (RC_Ex_Ctl.rc.ch2);   //yaw轴转角与摇杆值呈线性比例关系
			}
			else if(position_yaw_relative > YawMax)     //当现在码盘值距中值超过一定范围时，转角以一固定速率增大（yaw轴以固定小角速度旋转，通过调整底盘跟随速度，跟上云台旋转使相对码盘值在一定范围）
				Gimbal_control.angle_yaw_set += 0.012f;
			else if(position_yaw_relative < YawMin)
				Gimbal_control.angle_yaw_set -= 0.012f;

			
			Gimbal_control.angle_pitch_set += -0.0034f	*	(RC_Ex_Ctl.rc.ch3);      //Pitch轴转角与摇杆值呈线性比例关系
			
			Gimbal_control.angle_yaw_current = -(Yaw   - Yaw_Offset  ) *57.3f;        //获得当前Yaw轴角度，用来控制Yaw轴位置环
			Gimbal_control.angle_pitch_current = -(Roll  - Roll_Offset) *57.3f;       //云台的PITCH轴对应陀螺仪ROLL轴，获得当前Pitch轴角度，仅在GimbalControl_PitchAngel()中使用
					
			if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)> 180) 	Gimbal_control.angle_yaw_set -=360;  								//YAW轴 test超过一圈
		  if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)<-180) 	Gimbal_control.angle_yaw_set +=360;
			
			Gimbal_control.angle_pitch_set = MotorPositionLegalize(Gimbal_control.angle_pitch_set,	PitMax, PitMin);  //pitch轴俯仰角限位
	}
	else if(remoteState == KEY_REMOTE_STATE ) 
	{
	}
	else if(remoteState == STANDBY_STATE ) 
	{
	}
	else if(remoteState == VIEW_STATE ) 
	{
	}
}


/***************************************************************************************
 *Name     : GimbalMove
 *Function ：云台运动模式判断
 *Input    ：无
 *Output   ：无 
 *Description : 根据机器模式判断云台运动模式 
****************************************************************************************/
void GimbalMove(void)
{
	if(remoteState == PREPARE_STATE)
	{
		GimbalControl_Pitch(MIDDLE_PITCH);           //云台初始化pitch轴归中
		GimbalControl_Yaw (0);                       //云台初始化yaw轴归中
		angleSave = - ( Yaw - Yaw_Offset ) *57.3f;   //记录PREPARE_STATE模式时Yaw轴角度值
	}
	else if(remoteState == NORMAL_REMOTE_STATE )    
	{
		  GimbalControl_Pitch(MIDDLE_PITCH - Gimbal_control.angle_pitch_set);    //在pitch轴水平状态下控制
		  GimbalControl_YawAngel(Gimbal_control.angle_yaw_set + angleSave);      //根据angleSave存储的PREPARE_STATE模式下最后一次角度值，可消除启动时Yaw轴偏角
	}
	else if(remoteState == STANDBY_STATE ) //  待机模式/视觉模式
	{
		  remoteState_ex = STANDBY_STATE;
		  GimbalStop();
		  //Auto_aim(USART_RX_BUF3,50,&unpack_fream);
	}
	else if(remoteState == ERROR_STATE )   //出错模式
	{
		  remoteState_ex = ERROR_STATE;
		  GimbalStop();
	}
	else if(remoteState == VIEW_STATE ) //  视觉模式
	{
		  Auto_aim(USART_RX_BUF3,50,&unpack_fream);
	}
}



/*--------------------------------------------  PITCH 控制程序  --------------------------------------------*/

/***************************************************************************************
 *Name     : GimbalControl_PitchAngel
 *Function ：pitch轴角度控制
 *Input    ：setPosition
 *Output   ：无 
 *Description : 用陀螺仪做位置环和速度环，使pitch轴保持世界坐标系角度不变
****************************************************************************************/
void GimbalControl_PitchAngel(float SetPosition)
{
	float NowPosition = 	Gimbal_control.angle_pitch_current;						//pitch轴当前角度值
	float NowSpeed = mpu6500_real_data.Gyro_X * 57.3f;					        //pitch轴当前角速度值
	SetPosition = SetPosition*22.756f;//8192/360.0f;										//将角度值换算为码盘值(0~360 to 0~8191)
	NowPosition = NowPosition*22.756f;//8192/360.0f;

	PIDOut_Position_Pit = PID_Driver(SetPosition, NowPosition, &MotorPID_pitch.Position,2000);        //位置环输出
	PIDOut_Speed_Pit 		= PID_Driver(-PIDOut_Position_Pit/20.0f,NowSpeed,&MotorPID_pitch.Speed,1000); //速度环输出

	PIDOut_Whole_Pit = MotorCurrentLegalize(PIDOut_Speed_Pit,5000); //总输出，幅值限制
}

/***************************************************************************************
 *Name     : GimbalControl_Pitch
 *Function ：pitch轴角度控制
 *Input    ：setPosition
 *Output   ：无 
 *Description : 用编码器做位置环，用陀螺仪做速度环，使pitch轴相对机器自身坐标系角度不变
****************************************************************************************/
void GimbalControl_Pitch(float SetPosition)
{
	float NowPosition = 	current_position_206;						               //pitch轴当前码盘值
	float NowSpeed = mpu6500_real_data.Gyro_X * 57.3f;					         //pitch轴当前角速度值

	PIDOut_Position_Pit = PID_Driver(SetPosition, NowPosition, &MotorPID_pitch.Position,2000);       //位置环输出
	PIDOut_Speed_Pit 		= PID_Driver(PIDOut_Position_Pit/20.0f,NowSpeed,&MotorPID_pitch.Speed,1000); //速度环输出

	PIDOut_Whole_Pit = - MotorCurrentLegalize(PIDOut_Speed_Pit,5000);  //总输出，幅值限制
}


/*--------------------------------------------  YAW 控制程序  --------------------------------------------*/

/***************************************************************************************
 *Name     : GimbalControl_Yaw
 *Function ：yaw轴角度控制
 *Input    ：setPosition
 *Output   ：无 
 *Description : 用码盘做位置环，陀螺仪做速度环，初始化时yaw轴依靠码盘归中
****************************************************************************************/
void GimbalControl_Yaw(float SetPosition)
{
	//读云台电机当前值,角速度值
	float NowPosition = position_yaw_relative;						        //yaw轴当前码盘值	
	float NowSpeed = -mpu6500_real_data.Gyro_Z * 57.3f;						//yaw轴当前角速度值
	
	PIDOut_Position_Yaw = PID_Driver(SetPosition, NowPosition, &MotorPID_yaw.Position , 2000);       //位置环输出
	PIDOut_Speed_Yaw		= PID_Driver(-PIDOut_Position_Yaw/20.0f,NowSpeed,&MotorPID_yaw.Speed , 1000);//速度环输出
	
	PIDOut_Whole_Yaw = MotorCurrentLegalize(PIDOut_Speed_Yaw,5000);  //总输出，幅值限制

	PIDOut_Whole_Yaw = -PIDOut_Whole_Yaw;
}

/***************************************************************************************
 *Name     : GimbalControl_YawAngel
 *Function ：yaw轴角度控制
 *Input    ：setPosition
 *Output   ：无 
 *Description : 用陀螺仪做位置环和速度环，使yaw轴相对机器自身坐标系角度不变
****************************************************************************************/
void GimbalControl_YawAngel(float SetPosition)
{
	float NowPosition = Gimbal_control.angle_yaw_current;						//yaw轴当前角度值	
	float NowSpeed = -mpu6500_real_data.Gyro_Z * 57.3f;					      //yaw轴当前角速度值
	
	SetPosition = SetPosition*22.756f;//8192/360.0f;									//将角度值换算为码盘值(0~360 to 0~8191)
	NowPosition = NowPosition*22.756f;//8192/360.0f;
	
	PIDOut_Position_Yaw = PID_Driver(SetPosition, NowPosition, &MotorPID_yaw.Position , 2000);      //位置环输出
	PIDOut_Speed_Yaw		= PID_Driver(PIDOut_Position_Yaw/20.0f,NowSpeed,&MotorPID_yaw.Speed , 1000);//速度环输出
	
	PIDOut_Whole_Yaw = MotorCurrentLegalize(PIDOut_Speed_Yaw,5000);  //总输出，幅值限制

	PIDOut_Whole_Yaw = -PIDOut_Whole_Yaw;

}


/********  云台初始化  **********/
void GimbalControlInit(void)
{
	GimbalPidInitPrepare();   //云台初始化pid，以较慢速度归中
	GimbalStop();							//云台初始停转
	CAN1_Cmd_All((int16_t)PIDOut_Whole_Yaw, (int16_t)PIDOut_Whole_Pit , 0);	 
}


