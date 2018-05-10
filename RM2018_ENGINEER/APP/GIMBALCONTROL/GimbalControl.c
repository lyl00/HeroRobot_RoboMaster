/************************************************************
 *File		:	GimbalControl.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Control Gimbal Motors.
								GimbalControlLoop() shows the way to control the motion of Gimbal in different states. 
								Use PID to optimize Gimbal motor control.
 ************************************************************/
#include "main.h"

Gimbal_Control_t Gimbal_control; //��̨���ƽṹ��λ��pid���ٶ�pid
MotorPID_t	MotorPID_pitch;	  //Pitch��PID�ṹ
MotorPID_t	MotorPID_yaw;     //Yaw��PID�ṹ
frame unpack_fream;           //���Ӿ�ͨ��֡�ṹ
float PIDOut_Position_Pit,PIDOut_Speed_Pit,PIDOut_Whole_Pit;					//λ��pid��ʱ�����λ�������
float PIDOut_Position_Yaw,PIDOut_Speed_Yaw,PIDOut_Whole_Yaw,PIDOut_Electricity_Yaw;//�ٶ�pid��ʱ������ٶ������
float angleSave;       //�����ǽǶ���ʱ�洢����
int16_t position_yaw_relative = 0; //yaw�����MIDDLE_YAW������ֵ

	
/*-------------  ��̨����ѭ��  -------------*/
void GimbalControlLoop(void)
{	
	position_yaw_relative 	= GimbalValLigal(current_position_205	,	MIDDLE_YAW);  //����ǰ����ֵ����ֵ����õ�������̲�ֵ
	GimbalControlValSet();				//�����Gimbal_control�ṹ���ֵ������Ҫ�ﵽ�Ŀ���ֵ
	GimbalMove();									//���ݲ�ͬ��״̬�����в�ͬ�����
	CAN1_Cmd_All((int16_t)PIDOut_Whole_Yaw, (int16_t)PIDOut_Whole_Pit , (int16_t)PIDOut_Whole_Shoot);		//��CAN���߷�����̨���ת�����ݣ�Ϊ����ֵ
}


/*-------------  ��̨����pid��ʼ��  -------------*/
void GimbalPidInitPrepare(void)
{
	//PITCH��PID����
	PID_Init(&MotorPID_pitch.Position, PIDVAL_PITCH_POS_p_Prepare , PIDVAL_PITCH_POS_i_Prepare, PIDVAL_PITCH_POS_d_Prepare,-120,120,0);    //15 1.5 0.04 
	SetPIDCR(&MotorPID_pitch.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_pitch.Speed,	PIDVAL_PITCH_SPEED_p_Prepare, PIDVAL_PITCH_SPEED_i_Prepare, PIDVAL_PITCH_SPEED_d_Prepare,-60 ,60 ,0);//13 0 0
	SetPIDCR(&MotorPID_pitch.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
	//YAW��PID����
	PID_Init(&MotorPID_yaw.Position, PIDVAL_YAW_POS_p_Prepare, PIDVAL_YAW_POS_i_Prepare	, PIDVAL_YAW_POS_d_Prepare,-400,400,0);    //7.0 view 10 30 0.03
	SetPIDCR(&MotorPID_yaw.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_yaw.Speed,	PIDVAL_YAW_SPEED_p_Prepare,	PIDVAL_YAW_SPEED_i_Prepare,	PIDVAL_YAW_SPEED_d_Prepare,-60,60,0);    //13.55    //view 25 0 0
	SetPIDCR(&MotorPID_yaw.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
}	

/*-------------  ��̨��������pid��ʼ��  -------------*/
void GimbalPidInit(void)
{
	//PITCH��PID����
	PID_Init(&MotorPID_pitch.Position,PIDVAL_PITCH_POS_p, 	PIDVAL_PITCH_POS_i, 	PIDVAL_PITCH_POS_d,-120,120,0);    //15 1.5 0.04 
	SetPIDCR(&MotorPID_pitch.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_pitch.Speed,		PIDVAL_PITCH_SPEED_p, PIDVAL_PITCH_SPEED_i, PIDVAL_PITCH_SPEED_d,-60,60,0);//13 0 0
	SetPIDCR(&MotorPID_pitch.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
	//YAW��PID����
	PID_Init(&MotorPID_yaw.Position,PIDVAL_YAW_POS_p,		PIDVAL_YAW_POS_i,		PIDVAL_YAW_POS_d,	-300,300,0);    //7.0 view 10 30 0.03
	SetPIDCR(&MotorPID_yaw.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorPID_yaw.Speed,		PIDVAL_YAW_SPEED_p,	PIDVAL_YAW_SPEED_i,	PIDVAL_YAW_SPEED_d,-60,60,0);    //13.55    //view 25 0 0
	SetPIDCR(&MotorPID_yaw.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
}

/*-------------  ��ֹ̨ͣ�˶�  -------------*/
void GimbalStop(void)
{
	Gimbal_control.angle_pitch_set 	= 0;
	Gimbal_control.angle_yaw_set 	= -(Yaw *57.3f - Yaw_Offset *57.3f);    //��֤�л�״̬ʱ�����
	
	PIDOut_Whole_Yaw = 0;
	PIDOut_Whole_Pit = 0;
}

/*-------------  λ�÷�ֵ����  -------------*/
float MotorPositionLegalize(float PositionPara , float MAX , float MIN)
{
	float temp;
	temp= (PositionPara<MIN) ?  MIN:( (PositionPara>MAX) ? MAX : PositionPara)  ;
	return temp;
}

/*-------------  ������ֵ����  -------------*/
float MotorCurrentLegalize(float MotorCurrent , float limit)
{
	return MotorCurrent<-limit?-limit:(MotorCurrent>limit?limit:MotorCurrent);
}


/***************************************************************************************
 *Name     : GimbalValLigal
 *Function ����̨�����ֵ����
 *Input    ��raw_gimbal_data��middle_data
 *Output   ��raw_gimbal_data
 *Description : ���yaw�ᵱǰλ�������ֵλ�õ�����ֵ����ֹ��̨�ϵ��ת
****************************************************************************************/
int16_t GimbalValLigal(int raw_gimbal_data,int middle_data)  //������0ֵ�ڵ�ǰλ�ú���ֵ֮��ʱ��Ϊ�˱�����̨yaw��ת������180�ȣ������¹淶��
{
	if((raw_gimbal_data - middle_data)>4096 )
		raw_gimbal_data -= 8192;
	if((raw_gimbal_data - middle_data)<-4096 )
		raw_gimbal_data += 8192;
	
	raw_gimbal_data -= middle_data;            //���yaw�ᵱǰλ�������ֵλ�õ�����ֵ
	return raw_gimbal_data;
}


/***************************************************************************************
 *Name     : GimbalControlValSet
 *Function ����̨����Ƕ�ֵ��ȡ 
 *Input    ����
 *Output   ���� 
 *Description : �ı���ȫ�ֱ���	Gimbal_control.angle_yaw_set��Gimbal_control.angle_pitch_set
								Gimbal_control.angle_yaw_current��Gimbal_control.angle_pitch_current
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
			if((position_yaw_relative < YawMax)&&(position_yaw_relative>YawMin))    //����������ֵ����ֵ��һ����Χ��ʱ��ת����ҡ��ֵ�����Ա�����ϵ
			{				
				Gimbal_control.angle_yaw_set += 0.0002f * (RC_Ex_Ctl.rc.ch2);   //yaw��ת����ҡ��ֵ�����Ա�����ϵ
			}
			else if(position_yaw_relative > YawMax)     //����������ֵ����ֵ����һ����Χʱ��ת����һ�̶���������yaw���Թ̶�С���ٶ���ת��ͨ���������̸����ٶȣ�������̨��תʹ�������ֵ��һ����Χ��
				Gimbal_control.angle_yaw_set += 0.012f;
			else if(position_yaw_relative < YawMin)
				Gimbal_control.angle_yaw_set -= 0.012f;

			
			Gimbal_control.angle_pitch_set += -0.0034f	*	(RC_Ex_Ctl.rc.ch3);      //Pitch��ת����ҡ��ֵ�����Ա�����ϵ
			
			Gimbal_control.angle_yaw_current = -(Yaw   - Yaw_Offset  ) *57.3f;        //��õ�ǰYaw��Ƕȣ���������Yaw��λ�û�
			Gimbal_control.angle_pitch_current = -(Roll  - Roll_Offset) *57.3f;       //��̨��PITCH���Ӧ������ROLL�ᣬ��õ�ǰPitch��Ƕȣ�����GimbalControl_PitchAngel()��ʹ��
					
			if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)> 180) 	Gimbal_control.angle_yaw_set -=360;  								//YAW�� test����һȦ
		  if((Gimbal_control.angle_yaw_set - Gimbal_control.angle_yaw_current)<-180) 	Gimbal_control.angle_yaw_set +=360;
			
			Gimbal_control.angle_pitch_set = MotorPositionLegalize(Gimbal_control.angle_pitch_set,	PitMax, PitMin);  //pitch�ḩ������λ
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
 *Function ����̨�˶�ģʽ�ж�
 *Input    ����
 *Output   ���� 
 *Description : ���ݻ���ģʽ�ж���̨�˶�ģʽ 
****************************************************************************************/
void GimbalMove(void)
{
	if(remoteState == PREPARE_STATE)
	{
		GimbalControl_Pitch(MIDDLE_PITCH);           //��̨��ʼ��pitch�����
		GimbalControl_Yaw (0);                       //��̨��ʼ��yaw�����
		angleSave = - ( Yaw - Yaw_Offset ) *57.3f;   //��¼PREPARE_STATEģʽʱYaw��Ƕ�ֵ
	}
	else if(remoteState == NORMAL_REMOTE_STATE )    
	{
		  GimbalControl_Pitch(MIDDLE_PITCH - Gimbal_control.angle_pitch_set);    //��pitch��ˮƽ״̬�¿���
		  GimbalControl_YawAngel(Gimbal_control.angle_yaw_set + angleSave);      //����angleSave�洢��PREPARE_STATEģʽ�����һ�νǶ�ֵ������������ʱYaw��ƫ��
	}
	else if(remoteState == STANDBY_STATE ) //  ����ģʽ/�Ӿ�ģʽ
	{
		  remoteState_ex = STANDBY_STATE;
		  GimbalStop();
		  //Auto_aim(USART_RX_BUF3,50,&unpack_fream);
	}
	else if(remoteState == ERROR_STATE )   //����ģʽ
	{
		  remoteState_ex = ERROR_STATE;
		  GimbalStop();
	}
	else if(remoteState == VIEW_STATE ) //  �Ӿ�ģʽ
	{
		  Auto_aim(USART_RX_BUF3,50,&unpack_fream);
	}
}



/*--------------------------------------------  PITCH ���Ƴ���  --------------------------------------------*/

/***************************************************************************************
 *Name     : GimbalControl_PitchAngel
 *Function ��pitch��Ƕȿ���
 *Input    ��setPosition
 *Output   ���� 
 *Description : ����������λ�û����ٶȻ���ʹpitch�ᱣ����������ϵ�ǶȲ���
****************************************************************************************/
void GimbalControl_PitchAngel(float SetPosition)
{
	float NowPosition = 	Gimbal_control.angle_pitch_current;						//pitch�ᵱǰ�Ƕ�ֵ
	float NowSpeed = mpu6500_real_data.Gyro_X * 57.3f;					        //pitch�ᵱǰ���ٶ�ֵ
	SetPosition = SetPosition*22.756f;//8192/360.0f;										//���Ƕ�ֵ����Ϊ����ֵ(0~360 to 0~8191)
	NowPosition = NowPosition*22.756f;//8192/360.0f;

	PIDOut_Position_Pit = PID_Driver(SetPosition, NowPosition, &MotorPID_pitch.Position,2000);        //λ�û����
	PIDOut_Speed_Pit 		= PID_Driver(-PIDOut_Position_Pit/20.0f,NowSpeed,&MotorPID_pitch.Speed,1000); //�ٶȻ����

	PIDOut_Whole_Pit = MotorCurrentLegalize(PIDOut_Speed_Pit,5000); //���������ֵ����
}

/***************************************************************************************
 *Name     : GimbalControl_Pitch
 *Function ��pitch��Ƕȿ���
 *Input    ��setPosition
 *Output   ���� 
 *Description : �ñ�������λ�û��������������ٶȻ���ʹpitch����Ի�����������ϵ�ǶȲ���
****************************************************************************************/
void GimbalControl_Pitch(float SetPosition)
{
	float NowPosition = 	current_position_206;						               //pitch�ᵱǰ����ֵ
	float NowSpeed = mpu6500_real_data.Gyro_X * 57.3f;					         //pitch�ᵱǰ���ٶ�ֵ

	PIDOut_Position_Pit = PID_Driver(SetPosition, NowPosition, &MotorPID_pitch.Position,2000);       //λ�û����
	PIDOut_Speed_Pit 		= PID_Driver(PIDOut_Position_Pit/20.0f,NowSpeed,&MotorPID_pitch.Speed,1000); //�ٶȻ����

	PIDOut_Whole_Pit = - MotorCurrentLegalize(PIDOut_Speed_Pit,5000);  //���������ֵ����
}


/*--------------------------------------------  YAW ���Ƴ���  --------------------------------------------*/

/***************************************************************************************
 *Name     : GimbalControl_Yaw
 *Function ��yaw��Ƕȿ���
 *Input    ��setPosition
 *Output   ���� 
 *Description : ��������λ�û������������ٶȻ�����ʼ��ʱyaw���������̹���
****************************************************************************************/
void GimbalControl_Yaw(float SetPosition)
{
	//����̨�����ǰֵ,���ٶ�ֵ
	float NowPosition = position_yaw_relative;						        //yaw�ᵱǰ����ֵ	
	float NowSpeed = -mpu6500_real_data.Gyro_Z * 57.3f;						//yaw�ᵱǰ���ٶ�ֵ
	
	PIDOut_Position_Yaw = PID_Driver(SetPosition, NowPosition, &MotorPID_yaw.Position , 2000);       //λ�û����
	PIDOut_Speed_Yaw		= PID_Driver(-PIDOut_Position_Yaw/20.0f,NowSpeed,&MotorPID_yaw.Speed , 1000);//�ٶȻ����
	
	PIDOut_Whole_Yaw = MotorCurrentLegalize(PIDOut_Speed_Yaw,5000);  //���������ֵ����

	PIDOut_Whole_Yaw = -PIDOut_Whole_Yaw;
}

/***************************************************************************************
 *Name     : GimbalControl_YawAngel
 *Function ��yaw��Ƕȿ���
 *Input    ��setPosition
 *Output   ���� 
 *Description : ����������λ�û����ٶȻ���ʹyaw����Ի�����������ϵ�ǶȲ���
****************************************************************************************/
void GimbalControl_YawAngel(float SetPosition)
{
	float NowPosition = Gimbal_control.angle_yaw_current;						//yaw�ᵱǰ�Ƕ�ֵ	
	float NowSpeed = -mpu6500_real_data.Gyro_Z * 57.3f;					      //yaw�ᵱǰ���ٶ�ֵ
	
	SetPosition = SetPosition*22.756f;//8192/360.0f;									//���Ƕ�ֵ����Ϊ����ֵ(0~360 to 0~8191)
	NowPosition = NowPosition*22.756f;//8192/360.0f;
	
	PIDOut_Position_Yaw = PID_Driver(SetPosition, NowPosition, &MotorPID_yaw.Position , 2000);      //λ�û����
	PIDOut_Speed_Yaw		= PID_Driver(PIDOut_Position_Yaw/20.0f,NowSpeed,&MotorPID_yaw.Speed , 1000);//�ٶȻ����
	
	PIDOut_Whole_Yaw = MotorCurrentLegalize(PIDOut_Speed_Yaw,5000);  //���������ֵ����

	PIDOut_Whole_Yaw = -PIDOut_Whole_Yaw;

}


/********  ��̨��ʼ��  **********/
void GimbalControlInit(void)
{
	GimbalPidInitPrepare();   //��̨��ʼ��pid���Խ����ٶȹ���
	GimbalStop();							//��̨��ʼͣת
	CAN1_Cmd_All((int16_t)PIDOut_Whole_Yaw, (int16_t)PIDOut_Whole_Pit , 0);	 
}


