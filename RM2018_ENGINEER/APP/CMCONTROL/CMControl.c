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

#define gears_speedXYZ 	18   	//底盘平面移动系数，可调整底盘X、Y方向速度
#define gears_speedRAD 	15   	//底盘旋转运动系数，可调整底盘旋转速度
//#define followVal_limit 30   	//底盘跟随角度最小范围，可调整底盘不跟随范围
#define K_Rotate		0.8		//摇杆与旋转量之间的系数

CM_FLAG_REG CM_FLAG;

int16_t 	speed[4]={0}; 			//对应机器顺时针方向1、2、a3、4号轮子,即左前、右前、右后、右左  
PID_Struct 	CMPid,CMFollowPid;  //底盘运动pid、跟随pid
PID_Struct	CMAnglePID[4];
int16_t		CM_AngleGoal[4]={0};

void 		CMPID_Init(void);
void 		CMControlOut(int16_t speedA , int16_t speedB ,int16_t speedC ,int16_t speedD );
float 		CMSpeedLegalize(float MotorCurrent , float limit);
void 		CMStop(void);
void 		move(int16_t speedX, int16_t speedY, int16_t rad);

/*-------------  底盘控制循环  -------------*/
void CMControlLoop(void)
{
	static int16_t CMRotate_Value=0;
	
	if(remoteState == PREPARE_STATE)
	{
		CMControlOut(0,0,0,0);
	}
	else if(remoteState == NORMAL_REMOTE_STATE)   
	{
		CMRotate_Value = RC_Ex_Ctl.rc.ch2*K_Rotate;
		//锁住底盘
//		if(CM_FLAG.Locked && !CM_FLAG.SaveGoal){
//			CM_AngleGoal[0] = current_cm_speed[0];
//			CM_AngleGoal[1] = current_cm_speed[1];
//			CM_AngleGoal[2] = current_cm_speed[2];
//			CM_AngleGoal[3] = current_cm_speed[3];
//			CM_FLAG.SaveGoal = 1;
//		}
		if(GlobalMode == MODE_DRAG){
			move(RC_Ex_Ctl.rc.ch0,RC_Ex_Ctl.rc.ch1,CMRotate_Value);
		}else{
			move(RC_Ex_Ctl.rc.ch0,RC_Ex_Ctl.rc.ch1,0);
		}
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
		CMStop();
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
	static uint8_t tmp;
	
	if(CM_FLAG.Locked){
		if(CM_FLAG.SaveGoal){
			for(tmp=0;tmp<4;tmp++){
				 speed[tmp] = PID_Driver(CM_AngleGoal[tmp], current_cm_angle[tmp], &CMAnglePID[tmp], 2000);   
			}
		}
	}
	else{
		speedX *= gears_speedXYZ;
		speedY *= gears_speedXYZ;
		rad *= gears_speedRAD;
		
		speed[0] = ( speedX + speedY + rad) ;
		speed[1] = ( speedX - speedY + rad);
		speed[2] = (-speedX - speedY + rad);
		speed[3] = (-speedX + speedY + rad);
	}
	
	for(tmp=0;tmp<4;tmp++){
		 speed[tmp] = CMSpeedLegalize(speed[tmp],7000);  
	}
	CMControlOut(speed[0],speed[1],speed[2],speed[3]);
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
	speedA = PID_Driver(speedA, current_cm_speed[0], &CMPid, 10000);    
	speedB = PID_Driver(speedB, current_cm_speed[1], &CMPid, 10000);    
	speedC = PID_Driver(speedC, current_cm_speed[2], &CMPid, 10000);    
	speedD = PID_Driver(speedD, current_cm_speed[3], &CMPid, 10000);

	/************    幅值限制    *************/
	speedA = CMSpeedLegalize(speedA,7000);
	speedB = CMSpeedLegalize(speedB,7000);
	speedC = CMSpeedLegalize(speedC,7000);
	speedD = CMSpeedLegalize(speedD,7000);
	
	CAN1_Send_Bottom(speedA,speedB,speedC,speedD);
}

/*-------------  底盘电机幅值限制   -------------*/
float CMSpeedLegalize(float MotorCurrent , float limit)
{
	return MotorCurrent<-limit?-limit:(MotorCurrent>limit?limit:MotorCurrent);
}
/*-------------  底盘电机速度PID和跟随PID初始化  -------------*/
void CMPID_Init(void)
{
	uint8_t tmp;
	PID_Init(&CMPid, PIDVAL_CM_SPEED_p,PIDVAL_CM_SPEED_i,PIDVAL_CM_SPEED_d, -PIDVAL_CM_SPEED_limit,PIDVAL_CM_SPEED_limit,0);
	SetPIDCR(&CMPid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	//位置PID
	for(tmp=0;tmp<4;tmp++){
		PID_Init(&CMAnglePID[tmp], PIDVAL_CM_POSI_p, 	PIDVAL_CM_POSI_i, 	PIDVAL_CM_POSI_d, -PIDVAL_CM_POSI_limit,PIDVAL_CM_POSI_limit,0);
		SetPIDCR(&CMAnglePID[tmp],(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	}
//	PID_Init(&CMFollowPid, PIDVAL_CM_FOLLOW_p, 	PIDVAL_CM_FOLLOW_i, 	PIDVAL_CM_FOLLOW_d, -PIDVAL_CM_FOLLOW_limit,PIDVAL_CM_FOLLOW_limit,0);
//	SetPIDCR(&CMFollowPid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
}


/*底盘初始化*/
void CMControlInit(void)
{
	CMPID_Init();   
	CMControlOut(0,0,0,0);
}






/***************************************************************************************
 *Name        : keyboardmove
 *function    : 计算底盘电机速度给定值 由键盘控制
 *input       : keyboardvalue
 *output      : 无 
 *Description : 改变了全局变量	speedA, speedB, speedC, speedD

****************************新键盘*****************************************************/
//void keyboardmove(uint16_t keyboardvalue)
//{	
//	switch(keyboardvalue & (KEY_PRESSED_OFFSET_Q | KEY_PRESSED_OFFSET_E ))
//		{
//			case ( KEY_PRESSED_OFFSET_Q):
//				flag_swing = 1;
//				break;
//			case ( KEY_PRESSED_OFFSET_E):
//				flag_swing = 0;
//				break;
//			default:
//				flag_swing = 0;
//				break;
//	  }

//	switch(keyboardvalue & (KEY_PRESSED_OFFSET_SHIFT | KEY_PRESSED_OFFSET_CTRL))
//		{ 
//			case(KEY_PRESSED_OFFSET_SHIFT):	   //speed UP
//				xlimit = 800;
//				ylimit = 800;   
//				step=15;
//				break;
//			case(KEY_PRESSED_OFFSET_CTRL):	   //Speed DOWN
//				xlimit = 100;
//				ylimit = 100;
//				step=8;
//				break;
//			default: 													 //Normal 
//				xlimit = XLIMIT_OFFSET;
//				ylimit = YLIMIT_OFFSET;
//				step = 10;
//				break;
//	  }
//	// W and S
//	switch(keyboardvalue & (KEY_PRESSED_OFFSET_W|KEY_PRESSED_OFFSET_S))
//		{   
//			case ( KEY_PRESSED_OFFSET_W):
//				m = 0;
//				if(keymove_y>0)    //正在向前
//					keymove_y += step;           //加步长
//				else
//					keymove_y += step*6;
//				break;
//			case ( KEY_PRESSED_OFFSET_S):
//				m = 0;
//				if(keymove_y<0)
//					keymove_y -= step;
//				else 
//					keymove_y -= step*6;
//				break;
//			default:      //没有输入时
//				m++;
//				if(m>1)
//				{
//					if(keymove_y>0){keymove_y=keymove_y-6*step;}
//					else if(keymove_y<0){keymove_y=keymove_y+6*step;}
//				}
//				break;	
//	  }
//	
//	//  A and D  stand for X axis
//	switch(keyboardvalue & (KEY_PRESSED_OFFSET_A | KEY_PRESSED_OFFSET_D))
//		{   
//			case ( KEY_PRESSED_OFFSET_A):
//				n = 0;
//				if(keymove_x<0)
//					keymove_x -= step;
//				else 
//					keymove_x -= step*6;
//				break;
//			case ( KEY_PRESSED_OFFSET_D):
//				n = 0;
//				if(keymove_x>0)    //正在向右
//					keymove_x += step;           //加步长
//				else
//					keymove_x += step*6;
//				break;

//			default:
//				n++;
//				if(n>1)
//				{
//					if(keymove_x>0){keymove_x=keymove_x-6*step;}
//					else if(keymove_x<0){keymove_x=keymove_x+6*step;}
//				}
//				break;	
//	  }
//	keymove_x = keymove_x>xlimit?xlimit:(keymove_x<-xlimit?-xlimit:keymove_x);  //限速
//	keymove_y = keymove_y>ylimit?ylimit:(keymove_y<-ylimit?-ylimit:keymove_y);
//	if(keymove_x<40&&keymove_x>-40) keymove_x = 0;
//	if(keymove_y<40&&keymove_y>-40) keymove_y = 0;	 //减少爬行	
//}

