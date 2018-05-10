/************************************************************
 *File		:	ShootControl.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com	
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
u8 flag_trigger = 0;			//判断拨弹电机是否执行
static uint32_t t1_5ms = 0;	//两次射击的间隔时间
static uint32_t t2_5ms = 0;	//电机旋转PID给值时间  t2<t1   一直减
static int gun_count = 0;          //用于按键计数
float PIDOut_Speed_Shoot,PIDOut_Whole_Shoot;					//Pit,Yaw PID总输出
float shoot_speed_set = 0;

/***************************************************************************************
 *Name     : ShootControlLoop
 *Function ：摩擦轮和拨盘电机控制 
 *Input    ：无
 *Output   ：无 
 *Description : 根据机器状态判断摩擦轮和拨盘电机控制方法
****************************************************************************************/
void ShootControlLoop(void)
{
	if(remoteState == PREPARE_STATE)
	{
		ShootStop();
	}
	else if(remoteState == NORMAL_REMOTE_STATE)
	{
		FrictionJudge();					//判断摩擦轮是否应该打开
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

/*-------------  停止射击函数  -------------*/
void ShootStop(void)				
{
	TIM_SetCompare1(TIM12,Shoot_DOWN);	
	TIM_SetCompare2(TIM12,Shoot_DOWN);
	shoot_speed_set = 0;
	PIDOut_Whole_Shoot = 0;
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
void GunShootAction(void)						
{
	if(RC_Ex_Ctl.mouse.press_l == 1 ||  RC_Ex_Ctl.rc.s1 == 2) gun_count++;
	else gun_count = 0;
	
	if(t1_5ms > 0)	t1_5ms--;	          //两次射击的间隔时间计数 	
	
	if(t1_5ms == 0 && gun_count > 0)	  //t1_5ms 为0时，摇杆s1处于1位置或鼠标左键按下，可以射击子弹
	 {
			t1_5ms = ShootInterval;		      //距离下一次启动的倒计时 ，必须t1>t2，以免第二次射击时拨盘电机还未停转，调整t1可以改变射频
			t2_5ms = ShootTime; 	          //电机可以开转
			shoot_speed_set = ShootSpeedOffset;	
	 }
	
	if(t2_5ms > 0)                      //启动电机，t2_5ms 控制拨盘电机转动时间
	 {
		 t2_5ms--;	                      //电机旋转时间，t2<t1，一直减 
	 }
	else
	 {
		shoot_speed_set = 0;
	 }
	
	 ShootControlMove(shoot_speed_set);	
}

/*-------------  拨弹电机pid初始化函数  -------------*/
void PidShootInit(void)
{
	//Shoot轴PID设置
	PID_Init(&MotorShoot.Position,PIDVAL_SHOOT_POS_p, 	PIDVAL_SHOOT_POS_i, 	PIDVAL_SHOOT_POS_d,-120,120,0);    //15 1.5 0.04 
	SetPIDCR(&MotorShoot.Position,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorShoot.Speed,		PIDVAL_SHOOT_SPEED_p, PIDVAL_SHOOT_SPEED_i, PIDVAL_SHOOT_SPEED_d,-200,200,0);//13 0 0
	SetPIDCR(&MotorShoot.Speed,(1<<PEN)+(1<<IEN)+(1<<DEN)+(1<<PIDEN)+(1<<INTERVAL));
}

/*-------------  拨弹电机驱动函数  -------------*/
void ShootControlMove(float SetPosition)     
{
	float NowPosition = current_shoot_speed_207;		//拨盘电机当前速度
	PIDOut_Speed_Shoot = ShootSpeedControl(SetPosition, NowPosition, &MotorShoot.Speed);    
	PIDOut_Whole_Shoot = PIDOut_Speed_Shoot;	
}

/*-------------  拨弹电机速度控制函数  -------------*/
float ShootSpeedControl(float SetPosition,float NowPosition,PID_Struct* pid)
{
	float pidout;
	(*pid).SetPoint = SetPosition;  //代入设定值和测定值
	(*pid).NowPoint = NowPosition;
	PID_Calc(pid,10000);
	pidout=(*pid).Out;
	pidout= MotorCurrentLegalize(pidout,8000);
	return pidout;
}

void ShootControlInit(void)
{
	PidShootInit();	
	//电调解锁，pwm频率为50HZ，即周期2ms，先设置量程上限（大概为2ms），3s内再设置停转下限（大概为1ms）。每次开机都要设置。
	TIM_SetCompare1(TIM12,200);	  
	TIM_SetCompare2(TIM12,200);
	delay_ms(1000);
	ShootStop();
	
}
