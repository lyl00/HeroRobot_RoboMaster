/************************************************************
 *File		:	ShootControl.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com	
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Application for Shoot Motor, including 2 friction motor and 1 common DC motor.
								ShootControlLoop() shows the main control for Shoot motors.
								Pay attention to FrictionControl()
 ************************************************************/

#include "main.h"

#define Friction_OFF 0
#define Friction_ON  1
#define friction_count_time 40

MotorPID_t	MotorShoot;	

int16_t frictionState;
int16_t friction_count ;
u8 flag_FrtctionDelay = 0;
u8 flag_trigger = 0;			//�жϲ�������Ƿ�ִ��
static uint32_t t1_5ms = 0;	//��������ļ��ʱ��
static uint32_t t2_5ms = 0;	//�����תPID��ֵʱ��  t2<t1   һֱ��
static int gun_count = 0;          //���ڰ�������
float PIDOut_Speed_Shoot,PIDOut_Whole_Shoot;					//Pit,Yaw PID�����
float shoot_speed_set = 0;

/***************************************************************************************
 *Name     : ShootControlLoop
 *Function ��Ħ���ֺͲ��̵������ 
 *Input    ����
 *Output   ���� 
 *Description : ���ݻ���״̬�ж�Ħ���ֺͲ��̵�����Ʒ���
****************************************************************************************/
void ShootControlLoop(void)
{
	if(remoteState == PREPARE_STATE)
	{
		ShootStop();
	}
	else if(remoteState == NORMAL_REMOTE_STATE)
	{
		FrictionJudge();					//�ж�Ħ�����Ƿ�Ӧ�ô�
		GunControl();
	}
	else if(remoteState == KEY_REMOTE_STATE )  
	{
		FrictionJudge();
		GunControl();
	}
	else if(remoteState == STANDBY_STATE ) 
	{
		ShootStop();
	}
	else if(remoteState == ERROR_STATE ) 
	{
		ShootStop();
	}
}


/***************************************************************************************
 *Name     : FrictionJudge
 *Function ���ж�Ħ�����Ƿ�Ӧ�ô�
 *Input    ����
 *Output   ���� 
 *Description : ����Ҽ�����������ҡ��s1��3��1λ���ٴ�1��3λ���л�һ�ο���
****************************************************************************************/
void FrictionJudge(void)
{
	if(RC_Ex_Ctl.mouse.press_r) 
		friction_count++;                  //��곤������
	else	friction_count = 0;
	
	//�ж�Ħ���ֿ�����ر�
	if(frictionState == Friction_OFF && flag_FrtctionDelay == 0 )  //ֻ�е�flag_FrtctionDelayΪ0ʱ�����ж�Ħ����״̬
	 {                                                                                      
		if(flag_friction_switch == 1)                     //flag_friction_switch��rc.c�и�ֵ����s1�л�����flag_friction_switch=1
      {
			  frictionState = Friction_ON;
			  flag_friction_switch=0;
				FrictionControl();
			}   
		if(friction_count) 
		{
			frictionState = Friction_ON;   //�������Ҽ�������Ħ���ֿ���
			FrictionControl();
		}
	 }
	else if(frictionState == Friction_ON)
	 {
		if( flag_friction_switch == 1)
      {
				frictionState = Friction_OFF;
				flag_friction_switch = 0;
				FrictionControl();
			}
		if(friction_count > friction_count_time)          //Ħ���ֿ���״̬�£�����Ҽ�������Ħ���֣�ͨ������friction_count_time�ɵ��ڳ���ʱ��
		{
			frictionState = Friction_OFF;
			flag_FrtctionDelay = 200;                       //����flag_FrtctionDelayֵ�ɵ������β������ʱ��	
			FrictionControl();
		}
	 }
	
	//�������β����������ֹ��������
	if(flag_FrtctionDelay > 0) flag_FrtctionDelay--;    
}

/*-------------  Ħ����ʵ�ֺ���  -------------*/
void FrictionControl(void)
{
	if(frictionState == Friction_OFF )
	{
		TIM_SetCompare1(TIM12,Shoot_DOWN);	
		TIM_SetCompare2(TIM12,Shoot_DOWN);
		PIDOut_Whole_Shoot = 0;
		LASER_OFF();
	}
	if(frictionState == Friction_ON)
	{
		TIM_SetCompare1(TIM12,Shoot_UP);	
		TIM_SetCompare2(TIM12,Shoot_UP);

		LASER_ON();
	}
}

/*-------------  ֹͣ�������  -------------*/
void ShootStop(void)				
{
	TIM_SetCompare1(TIM12,Shoot_DOWN);	
	TIM_SetCompare2(TIM12,Shoot_DOWN);
	shoot_speed_set = 0;
	PIDOut_Whole_Shoot = 0;
}

/*-------------  �����������  -------------*/
void GunControl(void)			
{
	if(frictionState == Friction_OFF)
	{
		ShootStop();
	}
	if(frictionState == Friction_ON)	   //ֻ����Ħ���ֿ���������£����������������
	{
		GunShootAction();   
	}
}

/*-------------  ���������������  -------------*/
void GunShootAction(void)						
{
	if(RC_Ex_Ctl.mouse.press_l == 1 ||  RC_Ex_Ctl.rc.s1 == 2) gun_count++;
	else gun_count = 0;
	
	if(t1_5ms > 0)	t1_5ms--;	          //��������ļ��ʱ����� 	
	
	if(t1_5ms == 0 && gun_count > 0)	  //t1_5ms Ϊ0ʱ��ҡ��s1����1λ�û����������£���������ӵ�
	 {
			t1_5ms = ShootInterval;		      //������һ�������ĵ���ʱ ������t1>t2������ڶ������ʱ���̵����δͣת������t1���Ըı���Ƶ
			t2_5ms = ShootTime; 	          //������Կ�ת
			shoot_speed_set = ShootSpeedOffset;	
	 }
	
	if(t2_5ms > 0)                      //���������t2_5ms ���Ʋ��̵��ת��ʱ��
	 {
		 t2_5ms--;	                      //�����תʱ�䣬t2<t1��һֱ�� 
	 }
	else
	 {
		shoot_speed_set = 0;
	 }
	
	 ShootControlMove(shoot_speed_set);	
}

/*-------------  �������pid��ʼ������  -------------*/
void PidShootInit(void)
{
	//Shoot��PID����
	PID_Init(&MotorShoot.Position,PIDVAL_SHOOT_POS_p, 	PIDVAL_SHOOT_POS_i, 	PIDVAL_SHOOT_POS_d,-120,120,0);    //15 1.5 0.04 
	SetPIDCR(&MotorShoot.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorShoot.Speed,		PIDVAL_SHOOT_SPEED_p, PIDVAL_SHOOT_SPEED_i, PIDVAL_SHOOT_SPEED_d,-200,200,0);//13 0 0
	SetPIDCR(&MotorShoot.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
}

/*-------------  ���������������  -------------*/
void ShootControlMove(float SetPosition)     
{
	float NowPosition = current_shoot_speed_207;		//���̵����ǰ�ٶ�
	PIDOut_Speed_Shoot = ShootSpeedControl(SetPosition, NowPosition, &MotorShoot.Speed);    
	PIDOut_Whole_Shoot = PIDOut_Speed_Shoot;	
}

/*-------------  ��������ٶȿ��ƺ���  -------------*/
float ShootSpeedControl(float SetPosition,float NowPosition,PID_Struct* pid)
{
	float pidout;
	(*pid).SetPoint = SetPosition;  //�����趨ֵ�Ͳⶨֵ
	(*pid).NowPoint = NowPosition;
	PID_Calc(pid,10000);
	pidout=(*pid).Out;
	pidout= MotorCurrentLegalize(pidout,8000);
	return pidout;
}

void ShootControlInit(void)
{
	PidShootInit();	
	//���������pwmƵ��Ϊ50HZ��������2ms���������������ޣ����Ϊ2ms����3s��������ͣת���ޣ����Ϊ1ms����ÿ�ο�����Ҫ���á�
	TIM_SetCompare1(TIM12,200);	  
	TIM_SetCompare2(TIM12,200);
	delay_ms(1000);
	ShootStop();
	
}
