#include "main.h"

#define Friction_OFF 0
#define Friction_ON  1
#define friction_count_time 40

int16_t frictionState;
int16_t friction_count;
u8 flag_FrtctionDelay = 0;//摩擦轮参数

static uint32_t t1_5ms = 200;
static uint32_t t3_5ms =0;
static uint32_t t2_5ms = 20;	//控制射频和发射气缸伸出时间

static int gun_count = 0;     //用于按键计数
int bullet_loaded=1;          //判断是否有子弹被填装
static int empty_flag=0;   	  //弹仓已空标识
int bullet_count=-1;					//连发判断标志

int infrared1_state_prvious=1;
int infrared1_state_now=1;
int infrared2_state_prvious=1;
int infrared2_state_now=1;     //红外传感器读取数值


int blocking_flag=100;
int blocking_target;
int empty_count=100;

RemoteState_e now_remote_state;
RemoteState_e pre_remote_state;

int press_judge=0;	
int read_count=0;
Plate_Control_t Plate_Control;
MotorPID_t MotorPID_plate;
float PIDOut_Position_Plate,PIDOut_Speed_Plate,PIDOut_Whole_Shoot,PIDOut_Electricity_Plate;//速度pid临时输出和速度总输出


float OriginalPlatePos=4000;//定值，应与can的转换后的初始读值相同




/***************************************************************************************
 *Name     : ShootControlLoop
 *Function ：摩擦轮和拨盘电机控制 
 *Input    ：无
 *Output   ：无 
 *Description : 根据机器状态判断摩擦轮和拨盘电机控制方法
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
		blocking_judge();//卡弹判断和解决循环，拨弹盘的PID计算放在这里面
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
 *Function ：判断摩擦轮是否应该打开
 *Input    ：无
 *Output   ：无 
 *Description : 鼠标右键长按开启，摇杆s1从3到1位置再从1到3位置切换一次开启
****************************************************************************************/
void FrictionJudge(void)
{
	if(RC_Ex_Ctl.mouse.press_r) 
		friction_count++;                  //鼠标长按计数
	else	friction_count = 0;
	
	//判断摩擦轮开启或关闭
	if(frictionState == Friction_OFF && flag_FrtctionDelay == 0 )  //只有当flag_FrtctionDelay为0时，才判断摩擦轮状态
	 {                                                                                      
		if(flag_friction_switch == 1)                     //flag_friction_switch在rc.c中赋值，若s1切换过，flag_friction_switch=1
      {
			  frictionState = Friction_ON;
			  flag_friction_switch=0;
				FrictionControl();
			}   
		if(friction_count) 
		{
			frictionState = Friction_ON;   //如果鼠标右键长按，摩擦轮开启
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
		if(friction_count > friction_count_time)          //摩擦轮开启状态下，鼠标右键长按关摩擦轮，通过调整friction_count_time可调节长按时间
		{
			frictionState = Friction_OFF;
			flag_FrtctionDelay = 200;                       //调整flag_FrtctionDelay值可调节两次操作间隔时间	
			FrictionControl();
		}
	 }
	
	//调整两次操作间隔，防止操作过快
	if(flag_FrtctionDelay > 0) flag_FrtctionDelay--;    
}


/*-------------  停止射击函数  -------------*/
void ShootStop(void)				
{
	TIM_SetCompare1(TIM12,Shoot_DOWN);	
	TIM_SetCompare2(TIM12,Shoot_DOWN);
	PIDOut_Whole_Shoot=0;
}


/*-------------  摩擦轮实现函数  -------------*/
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
/*-------------  操作拨弹电机  -------------*/
void GunControl(void)			
{
	if(frictionState == Friction_OFF)
	{
		ShootStop();
	}
	if(frictionState == Friction_ON)	   //只有在摩擦轮开启的情况下，才能启动拨弹电机
	{
		GunShootAction();	
	}
}

/*-------------  拨弹电机动作函数  -------------*/
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
	
	if(t1_5ms > 0)	t1_5ms--;	          //两次射击的间隔时间计数 	
	if(t3_5ms>0)		t3_5ms--;

	if((t1_5ms<=0&& gun_count > 0)||(t1_5ms<=0&&bullet_count>0))	  
	 {
			//调整t1可以改变射频	
		 Cylinder_PUSH();//弹出气缸

		 
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



/*-------------  拨弹盘pid初始化函数  -------------*/
void PlatePidInit(void)
{
	PID_Init(&MotorPID_plate.Position,		PIDVAL_PLATE_POS_p, PIDVAL_PLATE_POS_i, PIDVAL_PLATE_POS_d,-200,200,0);//13 0 0
	SetPIDCR(&MotorPID_plate.Position,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
	PID_Init(&MotorPID_plate.Speed,		PIDVAL_PLATE_SPEED_p, PIDVAL_PLATE_SPEED_i, PIDVAL_PLATE_SPEED_d,-200,200,0);//13 0 0
	SetPIDCR(&MotorPID_plate.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
}


/*-------------  电流幅值函数  -------------*/
float PlateMotorCurrentLegalize(float MotorCurrent , float limit)
{
	return MotorCurrent<-limit?-limit:(MotorCurrent>limit?limit:MotorCurrent);
}



void PlatePID(float SetPosition)
{
	float NowPosition = current_angle_204	;
	float NowSpeed = can2_current_speed204;

	PIDOut_Position_Plate = PID_Driver(4000, NowPosition, &MotorPID_plate.Position , 2000);       //位置环输出
	PIDOut_Speed_Plate = PID_Driver(PIDOut_Position_Plate/20.0f,NowSpeed,&MotorPID_plate.Speed , 800);//速度环输出
	
	PIDOut_Whole_Shoot = PlateMotorCurrentLegalize(PIDOut_Speed_Plate,1000);  //总输出，幅值限制

	PIDOut_Whole_Shoot = PIDOut_Whole_Shoot;
}


/*-----------------------------------------------*/
void ShootControlInit(void)
{
	PlatePidInit();	
	//电调解锁，pwm频率为50HZ，即周期2ms，先设置量程上限（大概为2ms），3s内再设置停转下限（大概为1ms）。每次开机都要设置。
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


void Set_Target(void)//是每圈开始的角度都是0°机械角，以消除每次旋转的偏差
{
		current_angle_204-=1024;
	
		bullet_count++;		
		if(bullet_count>=1)//修改此参数可以调整连发子弹数
			{bullet_count=-1;}
}



void blocking_judge(void)//卡弹判断和解决循环，拨弹盘的PID计算放在这里面
{
		if(current_angle_204<3900)
		{
			blocking_flag++;
		}
		if(current_angle_204<3900&&blocking_flag>=1500)//修改参数1100可以修改判定为堵转的时间
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
			blocking_flag=500;//修改此参数可以改变回转角度停留时间
		}
}



void Empty_Detection(void)
{
	empty_count--;
	if(current_angle_204>=3900&&current_angle_204<=4100&&empty_flag<8&&empty_count<=0)//不知道是否能准确停在目标角，所以设有正负2°的误差值，调试后在加以调整
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




