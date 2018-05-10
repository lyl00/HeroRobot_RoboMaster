/****************************************************************************************************
 *File		:	CMControl.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Control Chassis Motors.
								CMControlLoop() shows the way to control the motion of chassis in different states.   
                Use PID to optimize Chassis motor control.								
 *****************************************************************************************************/

#include "main.h"

int16_t CMFollowVal=0,CMSwingVal=0; 			    //���̸���ֵ
int16_t speedA,speedB,speedC,speedD;          //��Ӧ����˳ʱ�뷽��1��2��3��4������,����ǰ����ǰ���Һ�����  
PID_Struct CMPid,CMFollowPid;                 //�����˶�pid������pid
int16_t step=6,x=0,y=0,m=0,n=0,q=0;           //step-�ٶȱ仯��  x-x�᷽��仯ֵ y-y�ٶȱ仯ֵ; 
int16_t xlimit = XLIMIT_OFFSET,ylimit = YLIMIT_OFFSET;
float ele_dec = 0;

//follow
int16_t follow_val_temp = 0;

//swing
u8 flag_swing = 0;
int16_t swing_count = 0 , swing_dir = 1 , swing_add_dir = 1;   //dir = 1 : +    dir = -1 : -


/*-------------  ���̿���ѭ��  -------------*/
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


/*-------------  ����ֹͣ  -------------*/
void CMStop(void)
{
	CAN1_Send_Bottom(0,0,0,0);
}


/***************************************************************************************
 *Name     : move
 *Function ��������̵���ٶȸ���ֵ ��ң�������ơ�ͬʱ�и������ӵ��ٶ�����
 *Input    ��speedX, speedY, rad
 *Output   ���� 
 *Description :�ı���ȫ�ֱ���	speedA, speedB, speedC, speedD
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
 *function    : ������̵���ٶȸ���ֵ �ɼ��̿���
 *input       : keyboardvalue
 *output      : �� 
 *Description : �ı���ȫ�ֱ���	speedA, speedB, speedC, speedD

****************************�¼���*****************************************************/
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
				if(y>0)    //������ǰ
					y += step;           //�Ӳ���
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
			default:      //û������ʱ
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
							if(x>0)    //��������
								x += step;           //�Ӳ���
							else
							x += step*6;
							break;
				}
			
			
			
			case ( KEY_PRESSED_OFFSET_D):
			if(remoteState == KEY_REMOTE_STATE)
				{
								n = 0;
								if(x>0)    //��������
								x += step;           //�Ӳ���
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
	x = x>xlimit?xlimit:(x<-xlimit?-xlimit:x);  //����
	y = y>ylimit?ylimit:(y<-ylimit?-ylimit:y);
	if(x<40&&x>-40) x = 0;
	if(y<40&&y>-40) y = 0;	 //��������
	move(x,y,rad);
		
}

/***************************************************************************************
 *Name     : CMControlOut
 *Function �����̵���ٶ����������PID�����������͸������������ͬʱ�и������ӵ��ٶ�����
 *Input    ��speedA,speedB,speedC,speedD
 *Output   ���� 
 *Description : ���̵���ٶȻ��ͷ�ֵ����
****************************************************************************************/
void CMControlOut(int16_t speedA , int16_t speedB ,int16_t speedC ,int16_t speedD )
{
	/************    ���PID����   *************/
	speedA = PID_Driver(speedA, current_cm_201, &CMPid, 10000);    
	speedB = PID_Driver(speedB, current_cm_202, &CMPid, 10000);    
	speedC = PID_Driver(speedC, current_cm_203, &CMPid, 10000);    
	speedD = PID_Driver(speedD, current_cm_204, &CMPid, 10000);    
	
	/************    ��������    *************/
	ele_dec = ele_cal;
	
	speedA = CMWatt_Cal(speedA,ele_dec);
	speedB = CMWatt_Cal(speedB,ele_dec);
	speedC = CMWatt_Cal(speedC,ele_dec);
	speedD = CMWatt_Cal(speedD,ele_dec);

	/************    ��ֵ����    *************/
	speedA = CMSpeedLegalize(speedA,7000);
	speedB = CMSpeedLegalize(speedB,7000);
	speedC = CMSpeedLegalize(speedC,7000);
	speedD = CMSpeedLegalize(speedD,7000);
	
	CAN1_Send_Bottom(speedA,speedB,speedC,speedD);
}


/**********  ���̵����ֵ����  **********/
float CMSpeedLegalize(float MotorCurrent , float limit)
{
	return MotorCurrent<-limit?-limit:(MotorCurrent>limit?limit:MotorCurrent);
}


/***************************************************************************************
 *Name     : followValCal
 *Function �����̵���������
 *Input    ��Setposition,Yaw�������̸���ֵ
 *Output   �����̸�����
 *Description : ���̵�����������㺯��
****************************************************************************************/
int16_t followValCal(float Setposition)
{
	int16_t followVal = 0;
	float NowPosition = position_yaw_relative;

	followVal = PID_Driver(Setposition, NowPosition, &CMFollowPid , 10000); 
	followVal = CMSpeedLegalize(followVal,5000);	
  //��������Сֵ���Ƕȹ�С������	
	if(abs(followVal) < followVal_limit) followVal = 0;
	
	return followVal;
}


/*-------------  ���̵���ٶ�PID�͸���PID��ʼ��  -------------*/
void CMPID_Init(void)
{
	PID_Init(&CMPid, PIDVAL_CM_SPEED_p, 	PIDVAL_CM_SPEED_i, 	PIDVAL_CM_SPEED_d, -PIDVAL_CM_SPEED_limit,PIDVAL_CM_SPEED_limit,0);
	SetPIDCR(&CMPid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&CMFollowPid, PIDVAL_CM_FOLLOW_p, 	PIDVAL_CM_FOLLOW_i, 	PIDVAL_CM_FOLLOW_d, -PIDVAL_CM_FOLLOW_limit,PIDVAL_CM_FOLLOW_limit,0);
	SetPIDCR(&CMFollowPid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
}


/*���̳�ʼ��*/
void CMControlInit(void)
{
	CMPID_Init();   
	CMControlOut(0,0,0,0);
}
