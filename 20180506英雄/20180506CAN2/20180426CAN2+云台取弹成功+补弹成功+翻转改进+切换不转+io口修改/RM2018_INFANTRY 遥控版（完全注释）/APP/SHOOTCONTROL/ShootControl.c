#include "main.h"

#define Friction_OFF 0
#define Friction_ON  1
#define friction_count_time 40

int16_t frictionState;
int16_t friction_count;
u8 flag_FrtctionDelay = 0;//Ħ���ֲ���

static uint32_t t1_5ms = 200;
static uint32_t t3_5ms =0;
static uint32_t t2_5ms = 20;	//������Ƶ�ͷ����������ʱ��

static int gun_count = 0;     //���ڰ�������
int bullet_loaded=1;          //�ж��Ƿ����ӵ�����װ
static int empty_flag=0;   	  //�����ѿձ�ʶ
int bullet_count=-1;					//�����жϱ�־

int infrared1_state_prvious=1;
int infrared1_state_now=1;
int infrared2_state_prvious=1;
int infrared2_state_now=1;     //���⴫������ȡ��ֵ


int blocking_flag=100;
int blocking_target;
int empty_count=100;

RemoteState_e now_remote_state;
RemoteState_e pre_remote_state;

int press_judge=0;	
int read_count=0;
Plate_Control_t Plate_Control;
MotorPID_t MotorPID_plate;
float PIDOut_Position_Plate,PIDOut_Speed_Plate,PIDOut_Whole_Shoot,PIDOut_Electricity_Plate;//�ٶ�pid��ʱ������ٶ������


float OriginalPlatePos=4000;//��ֵ��Ӧ��can��ת����ĳ�ʼ��ֵ��ͬ




/***************************************************************************************
 *Name     : ShootControlLoop
 *Function ��Ħ���ֺͲ��̵������ 
 *Input    ����
 *Output   ���� 
 *Description : ���ݻ���״̬�ж�Ħ���ֺͲ��̵�����Ʒ���
****************************************************************************************/
void ShootControlLoop(void)
{
	now_remote_state=remoteState;
	if(remoteState == PREPARE_STATE)
	{
		ShootStop();
		CAN2_Cmd_SHOOT((int16_t)PIDOut_Whole_Shoot);
	}
	else if(remoteState == NORMAL_REMOTE_STATE)
	{
		ShootStop();
		CAN2_Cmd_SHOOT((int16_t)PIDOut_Whole_Shoot);
	}
	else if(remoteState == KEY_REMOTE_STATE )  
	{
		if(now_remote_state!=pre_remote_state)
		{
			current_angle_204=4000;
		}
		FrictionJudge();
		GunControl();
		//PlatePID(OriginalPlatePos);
		blocking_judge();//�����жϺͽ��ѭ���������̵�PID�������������
		Empty_Detection();
//		
//		if(read_count%200==0)
//	{
//	if(infrared1_state_now==1)
//		{printf("infrared:%d\n",1);}
//	else if(infrared1_state_now==0)
//	{printf("infrared:%d\n",0);}
//	printf("bullet_loaded%d\n",bullet_loaded);
//	read_count-=1000;
//	}
//	read_count++;
		
		
		CAN2_Cmd_SHOOT((int16_t)PIDOut_Whole_Shoot);
	}
	else if(remoteState == STANDBY_STATE ) 
	{
		ShootStop();
		CAN2_Cmd_SHOOT((int16_t)PIDOut_Whole_Shoot);
	}
	else if(remoteState == ERROR_STATE ) 
	{
		ShootStop();
		CAN2_Cmd_SHOOT((int16_t)PIDOut_Whole_Shoot);
	}
	pre_remote_state=now_remote_state;
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


/*-------------  ֹͣ�������  -------------*/
void ShootStop(void)				
{
	TIM_SetCompare1(TIM12,Shoot_DOWN);	
	TIM_SetCompare2(TIM12,Shoot_DOWN);
	PIDOut_Whole_Shoot=0;
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
int now_press=0;
int last_press=0;
void GunShootAction(void)						
{
	now_press=RC_Ex_Ctl.mouse.press_l;
	if(RC_Ex_Ctl.mouse.press_l == 1)
	{		
		gun_count++;
		bullet_count=0;		
		
	}
		if((now_press!=last_press)&&now_press==1&&t3_5ms<=0)
	{
		press_judge++;
	}
			
	last_press=now_press;
	
	if(t1_5ms > 0)	t1_5ms--;	          //��������ļ��ʱ����� 	
	if(t3_5ms>0)		t3_5ms--;

	if((t1_5ms<=0&& gun_count > 0)||(t1_5ms<=0&&bullet_count>0))	  
	 {
			//����t1���Ըı���Ƶ	
		 Cylinder_PUSH();//��������

		 
		 if(t2_5ms>0)
		 {t2_5ms--;}
		 else
		 {
			  
			 t2_5ms = ShootTime;
				bullet_loaded=0;
				Cylinder_BACK();
			if(press_judge>0)
			 {
					Set_Target();
				 	empty_count=100;
				 press_judge=0;
			 }
				gun_count=0;
			 	 t1_5ms=100;
		 }
	 }
}



/*-------------  ������pid��ʼ������  -------------*/
void PlatePidInit(void)
{
	PID_Init(&MotorPID_plate.Position,		PIDVAL_PLATE_POS_p, PIDVAL_PLATE_POS_i, PIDVAL_PLATE_POS_d,-200,200,0);//13 0 0
	SetPIDCR(&MotorPID_plate.Position,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
	PID_Init(&MotorPID_plate.Speed,		PIDVAL_PLATE_SPEED_p, PIDVAL_PLATE_SPEED_i, PIDVAL_PLATE_SPEED_d,-200,200,0);//13 0 0
	SetPIDCR(&MotorPID_plate.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
}


/*-------------  ������ֵ����  -------------*/
float PlateMotorCurrentLegalize(float MotorCurrent , float limit)
{
	return MotorCurrent<-limit?-limit:(MotorCurrent>limit?limit:MotorCurrent);
}



void PlatePID(float SetPosition)
{
	float NowPosition = current_angle_204	;
	float NowSpeed = can2_current_speed204;

	PIDOut_Position_Plate = PID_Driver(4000, NowPosition, &MotorPID_plate.Position , 2000);       //λ�û����
	PIDOut_Speed_Plate = PID_Driver(PIDOut_Position_Plate/20.0f,NowSpeed,&MotorPID_plate.Speed , 800);//�ٶȻ����
	
	PIDOut_Whole_Shoot = PlateMotorCurrentLegalize(PIDOut_Speed_Plate,1000);  //���������ֵ����

	PIDOut_Whole_Shoot = PIDOut_Whole_Shoot;
}


/*-----------------------------------------------*/
void ShootControlInit(void)
{
	PlatePidInit();	
	//���������pwmƵ��Ϊ50HZ��������2ms���������������ޣ����Ϊ2ms����3s��������ͣת���ޣ����Ϊ1ms����ÿ�ο�����Ҫ���á�
	TIM_SetCompare1(TIM12,200);	  
	TIM_SetCompare2(TIM12,200);
	delay_ms(1000);
	ShootStop();
}

void Read_Infrared_State(void)
{
	infrared1_state_prvious=infrared1_state_now;
	//infrared2_state_prvious=infrared2_state_now;
	infrared1_state_now=GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_11);
	
	
	
	//	infrared2_state_now=GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13);
		Bullet_Load_Judge();
}

void Bullet_Load_Judge(void)
{
	if(infrared1_state_prvious==0&&infrared1_state_now==1)
	{
		bullet_loaded=1;
	}
//		if(infrared1_state_prvious==1&&infrared1_state_now==0)
//	{
//		if(infrared2_state_now==0)
//		{
//			bullet_loaded=1;
//		}
//	}
//	else if(infrared2_state_prvious==1&&infrared2_state_now==0)
//	{
//		if(infrared1_state_now==0)
//		{
//			bullet_loaded=1;
//		}
//	}
}


void Set_Target(void)//��ÿȦ��ʼ�ĽǶȶ���0���е�ǣ�������ÿ����ת��ƫ��
{
		current_angle_204-=1024;
	
		bullet_count++;		
		if(bullet_count>=1)//�޸Ĵ˲������Ե��������ӵ���
			{bullet_count=-1;}
}



void blocking_judge(void)//�����жϺͽ��ѭ���������̵�PID�������������
{
		if(current_angle_204<3900)
		{
			blocking_flag++;
		}
		if(current_angle_204<3900&&blocking_flag>=1500)//�޸Ĳ���1100�����޸��ж�Ϊ��ת��ʱ��
		{
			blocking_flag=0;
			blocking_target=current_angle_204-500;
		}
		if(blocking_flag<=500)
		{
			PlatePID(blocking_target);
		}
		else
		{
			PlatePID(OriginalPlatePos);
		}
			if(current_angle_204>3950)
		{
			blocking_flag=500;//�޸Ĵ˲������Ըı��ת�Ƕ�ͣ��ʱ��
		}
}



void Empty_Detection(void)
{
	empty_count--;
	if(current_angle_204>=3900&&current_angle_204<=4100&&empty_flag<8&&empty_count<=0)//��֪���Ƿ���׼ȷͣ��Ŀ��ǣ�������������2������ֵ�����Ժ��ڼ��Ե���
		{
			if(bullet_loaded==0)
			{
			Set_Target();
			empty_flag++;
			empty_count=100;
			}
		}		
	if(empty_flag>=8)
	{
		bullet_loaded=1;
		empty_flag=0;
	}
	
}




