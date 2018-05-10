/****************************************************************************************************
 *File		:	CMControl.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Control Chassis Motors.
								CMControlLoop() shows the way to control the motion of chassis in different states.   
                Use PID to optimize Chassis motor control.								
 *****************************************************************************************************/

#include "main.h"

int16_t CMFollowVal=0,CMSwingVal=0; 			    //底盘跟随值
int16_t speedA,speedB,speedC,speedD;          //对应机器顺时针方向1、2、3、4号轮子,即左前、右前、右后、右左  
PID_Struct CMPid,CMFollowPid;                 //底盘运动pid、跟随pid
int16_t step=6,x=0,y=0,m=0,n=0,q=0;           //step-速度变化率  x-x轴方向变化值 y-y速度变化值; 
int16_t xlimit = XLIMIT_OFFSET,ylimit = YLIMIT_OFFSET;
float ele_dec = 0;

//follow
int16_t follow_val_temp = 0;

//swing
u8 flag_swing = 0;
int16_t swing_count = 0 , swing_dir = 1 , swing_add_dir = 1;   //dir = 1 : +    dir = -1 : -


/*-------------  底盘控制循环  -------------*/
void CMControlLoop(void)
{
	if(remoteState == PREPARE_STATE)
	{
		CMControlOut(0,0,0,0);
	}
	else if(remoteState == NORMAL_REMOTE_STATE)   
	{
		//move(RC_Ex_Ctl.rc.ch0*0.75,RC_Ex_Ctl.rc.ch1*0.75,RC_Ex_Ctl.rc.ch2*0.25);//CMFollowVal);//);//CMFollowVal );//
			keyboardmove_old(RC_Ex_Ctl.key.v,RC_Ex_Ctl.mouse.x*10);
	}
	else if(remoteState == STANDBY_STATE ) 
	{
			CMStop();
	}
	else if(remoteState == ERROR_STATE ) 
	{
		  CMStop();
	}
		else if(remoteState == KEY_REMOTE_STATE )
	{
		
		CMFollowVal = followValCal(follow_val_temp);
			keyboardmove_old(RC_Ex_Ctl.key.v,CMFollowVal);
			swing_dir 	= 1;
			swing_count = 0;
			CMSwingVal 	= 0;
		
		
	}
}


/*-------------  底盘停止  -------------*/
void CMStop(void)
{
	CAN1_Send_Bottom(0,0,0,0);
}


/***************************************************************************************
 *Name     : move
 *Function ：计算底盘电机速度给定值 由遥控器控制。同时有各个轮子的速度限制
 *Input    ：speedX, speedY, rad
 *Output   ：无 
 *Description :改变了全局变量	speedA, speedB, speedC, speedD
****************************************************************************************/
void move(int16_t speedX, int16_t speedY, int16_t rad)
{
		speedX *= gears_speedXYZ;
		speedY *= gears_speedXYZ;		
		rad *= gears_speedRAD;
	
		speedA = ( speedX + speedY + rad) ;
		speedB = ( speedX - speedY + rad);
		speedC = (-speedX - speedY + rad);
		speedD = (-speedX + speedY + rad);
	
		speedA = CMSpeedLegalize(speedA,7000);
		speedB = CMSpeedLegalize(speedB,7000);
		speedC = CMSpeedLegalize(speedC,7000);
		speedD = CMSpeedLegalize(speedD,7000);
	
	if(remoteState == KEY_REMOTE_STATE)
	{
		CMControlOut(speedA,speedB,speedC,speedD);
	}
	else if (remoteState == NORMAL_REMOTE_STATE)
	{
		CMControlOut(-speedA,-speedB,-speedC,-speedD);
	}
}
/***************************************************************************************
 *Name        : move
 *function    : 计算底盘电机速度给定值 由键盘控制
 *input       : keyboardvalue
 *output      : 无 
 *Description : 改变了全局变量	speedA, speedB, speedC, speedD

****************************新键盘*****************************************************/
void keyboardmove_old(uint16_t keyboardvalue , int16_t rad)
{	
	
	//  Q  or E
	switch(keyboardvalue & (KEY_PRESSED_OFFSET_Q | KEY_PRESSED_OFFSET_E ))
		{
			case ( KEY_PRESSED_OFFSET_Q):
				flag_swing = 1;
				//follow_val_temp = 1300;
				break;
			case ( KEY_PRESSED_OFFSET_E):
				flag_swing = 0;
				//follow_val_temp = 0;
				break;
			default:
				//flag_feed = 0;
				break;
	  }

	switch(keyboardvalue & (KEY_PRESSED_OFFSET_SHIFT | KEY_PRESSED_OFFSET_CTRL))
		{ 
			case(KEY_PRESSED_OFFSET_SHIFT):	   //speed UP
				xlimit = 800;
				ylimit = 800;   
				step=15;
				break;
			case(KEY_PRESSED_OFFSET_CTRL):	   //Speed DOWN
				xlimit = 100;
				ylimit = 100;
				step=8;
				break;
			default: 													 //Normal 
				xlimit = XLIMIT_OFFSET;
				ylimit = YLIMIT_OFFSET;
				step = 10;
				break;
	  }
	// W and S
	switch(keyboardvalue & (KEY_PRESSED_OFFSET_W|KEY_PRESSED_OFFSET_S))
		{   
			case ( KEY_PRESSED_OFFSET_W):
				m = 0;
				if(y>0)    //正在向前
					y += step;           //加步长
				else
					y += step*6;
				break;
			case ( KEY_PRESSED_OFFSET_S):
				m = 0;
				if(y<0)
					y -= step;
				else 
					y -= step*6;
				break;
			default:      //没有输入时
				m++;
				if(m>1)
				{
					if(y>0)			{y=y-6*step;}
					else if(y<0){y=y+6*step;}
				}
				break;	
	  }
	
	//  A and D  stand for X axis
	switch(keyboardvalue & (KEY_PRESSED_OFFSET_A | KEY_PRESSED_OFFSET_D))
		{   
			case ( KEY_PRESSED_OFFSET_A):
				if(remoteState == KEY_REMOTE_STATE)
				{
							n = 0;
							if(x<0)
							x -= step;
							else 
							x -= step*6;
							break;
				}
				else if (remoteState == NORMAL_REMOTE_STATE)
				{
								n = 0;
							if(x>0)    //正在向右
								x += step;           //加步长
							else
							x += step*6;
							break;
				}
			
			
			
			case ( KEY_PRESSED_OFFSET_D):
			if(remoteState == KEY_REMOTE_STATE)
				{
								n = 0;
								if(x>0)    //正在向右
								x += step;           //加步长
								else
								x += step*6;
								break;
				}
			else if (remoteState == NORMAL_REMOTE_STATE)
				{
							n = 0;
								if(x<0)
									x -= step;
								else 
									x -= step*6;
								break;
				}

			default:
				n++;
				if(n>1)
				{
					if(x>0)			{x=x-6*step;}
					else if(x<0){x=x+6*step;}
				}
				break;	
	  }
	x = x>xlimit?xlimit:(x<-xlimit?-xlimit:x);  //限速
	y = y>ylimit?ylimit:(y<-ylimit?-ylimit:y);
	if(x<40&&x>-40) x = 0;
	if(y<40&&y>-40) y = 0;	 //减少爬行
	move(x,y,rad);
		
}

/***************************************************************************************
 *Name     : CMControlOut
 *Function ：底盘电机速度输出，经过PID控制器，发送给电机驱动器，同时有各个轮子的速度限制
 *Input    ：speedA,speedB,speedC,speedD
 *Output   ：无 
 *Description : 底盘电机速度环和幅值函数
****************************************************************************************/
void CMControlOut(int16_t speedA , int16_t speedB ,int16_t speedC ,int16_t speedD )
{
	/************    电机PID计算   *************/
	speedA = PID_Driver(speedA, current_cm_201, &CMPid, 10000);    
	speedB = PID_Driver(speedB, current_cm_202, &CMPid, 10000);    
	speedC = PID_Driver(speedC, current_cm_203, &CMPid, 10000);    
	speedD = PID_Driver(speedD, current_cm_204, &CMPid, 10000);    
	
	/************    功率限制    *************/
	ele_dec = ele_cal;
	
	speedA = CMWatt_Cal(speedA,ele_dec);
	speedB = CMWatt_Cal(speedB,ele_dec);
	speedC = CMWatt_Cal(speedC,ele_dec);
	speedD = CMWatt_Cal(speedD,ele_dec);

	/************    幅值限制    *************/
	speedA = CMSpeedLegalize(speedA,7000);
	speedB = CMSpeedLegalize(speedB,7000);
	speedC = CMSpeedLegalize(speedC,7000);
	speedD = CMSpeedLegalize(speedD,7000);
	
	CAN1_Send_Bottom(speedA,speedB,speedC,speedD);
}


/**********  底盘电机幅值限制  **********/
float CMSpeedLegalize(float MotorCurrent , float limit)
{
	return MotorCurrent<-limit?-limit:(MotorCurrent>limit?limit:MotorCurrent);
}


/***************************************************************************************
 *Name     : followValCal
 *Function ：底盘电机跟随计算
 *Input    ：Setposition,Yaw轴电机码盘跟随值
 *Output   ：底盘跟随量
 *Description : 底盘电机跟随量计算函数
****************************************************************************************/
int16_t followValCal(float Setposition)
{
	int16_t followVal = 0;
	float NowPosition = position_yaw_relative;

	followVal = PID_Driver(Setposition, NowPosition, &CMFollowPid , 10000); 
	followVal = CMSpeedLegalize(followVal,5000);	
  //跟随量最小值，角度过小不跟随	
	if(abs(followVal) < followVal_limit) followVal = 0;
	
	return followVal;
}


/*-------------  底盘电机速度PID和跟随PID初始化  -------------*/
void CMPID_Init(void)
{
	PID_Init(&CMPid, PIDVAL_CM_SPEED_p, 	PIDVAL_CM_SPEED_i, 	PIDVAL_CM_SPEED_d, -PIDVAL_CM_SPEED_limit,PIDVAL_CM_SPEED_limit,0);
	SetPIDCR(&CMPid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&CMFollowPid, PIDVAL_CM_FOLLOW_p, 	PIDVAL_CM_FOLLOW_i, 	PIDVAL_CM_FOLLOW_d, -PIDVAL_CM_FOLLOW_limit,PIDVAL_CM_FOLLOW_limit,0);
	SetPIDCR(&CMFollowPid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
}


/*底盘初始化*/
void CMControlInit(void)
{
	CMPID_Init();   
	CMControlOut(0,0,0,0);
}
