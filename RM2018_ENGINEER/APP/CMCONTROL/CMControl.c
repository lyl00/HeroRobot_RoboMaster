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

#define gears_speedXYZ 	18   	//����ƽ���ƶ�ϵ�����ɵ�������X��Y�����ٶ�
#define gears_speedRAD 	15   	//������ת�˶�ϵ�����ɵ���������ת�ٶ�
//#define followVal_limit 30   	//���̸���Ƕ���С��Χ���ɵ������̲����淶Χ
#define K_Rotate		0.8		//ҡ������ת��֮���ϵ��

CM_FLAG_REG CM_FLAG;

int16_t 	speed[4]={0}; 			//��Ӧ����˳ʱ�뷽��1��2��a3��4������,����ǰ����ǰ���Һ�����  
PID_Struct 	CMPid,CMFollowPid;  //�����˶�pid������pid
PID_Struct	CMAnglePID[4];
int16_t		CM_AngleGoal[4]={0};

void 		CMPID_Init(void);
void 		CMControlOut(int16_t speedA , int16_t speedB ,int16_t speedC ,int16_t speedD );
float 		CMSpeedLegalize(float MotorCurrent , float limit);
void 		CMStop(void);
void 		move(int16_t speedX, int16_t speedY, int16_t rad);

/*-------------  ���̿���ѭ��  -------------*/
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
		//��ס����
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
 *Function �����̵���ٶ����������PID�����������͸������������ͬʱ�и������ӵ��ٶ�����
 *Input    ��speedA,speedB,speedC,speedD
 *Output   ���� 
 *Description : ���̵���ٶȻ��ͷ�ֵ����
****************************************************************************************/
void CMControlOut(int16_t speedA , int16_t speedB ,int16_t speedC ,int16_t speedD )
{
	/************    ���PID����   *************/
	speedA = PID_Driver(speedA, current_cm_speed[0], &CMPid, 10000);    
	speedB = PID_Driver(speedB, current_cm_speed[1], &CMPid, 10000);    
	speedC = PID_Driver(speedC, current_cm_speed[2], &CMPid, 10000);    
	speedD = PID_Driver(speedD, current_cm_speed[3], &CMPid, 10000);

	/************    ��ֵ����    *************/
	speedA = CMSpeedLegalize(speedA,7000);
	speedB = CMSpeedLegalize(speedB,7000);
	speedC = CMSpeedLegalize(speedC,7000);
	speedD = CMSpeedLegalize(speedD,7000);
	
	CAN1_Send_Bottom(speedA,speedB,speedC,speedD);
}

/*-------------  ���̵����ֵ����   -------------*/
float CMSpeedLegalize(float MotorCurrent , float limit)
{
	return MotorCurrent<-limit?-limit:(MotorCurrent>limit?limit:MotorCurrent);
}
/*-------------  ���̵���ٶ�PID�͸���PID��ʼ��  -------------*/
void CMPID_Init(void)
{
	uint8_t tmp;
	PID_Init(&CMPid, PIDVAL_CM_SPEED_p,PIDVAL_CM_SPEED_i,PIDVAL_CM_SPEED_d, -PIDVAL_CM_SPEED_limit,PIDVAL_CM_SPEED_limit,0);
	SetPIDCR(&CMPid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	//λ��PID
	for(tmp=0;tmp<4;tmp++){
		PID_Init(&CMAnglePID[tmp], PIDVAL_CM_POSI_p, 	PIDVAL_CM_POSI_i, 	PIDVAL_CM_POSI_d, -PIDVAL_CM_POSI_limit,PIDVAL_CM_POSI_limit,0);
		SetPIDCR(&CMAnglePID[tmp],(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	}
//	PID_Init(&CMFollowPid, PIDVAL_CM_FOLLOW_p, 	PIDVAL_CM_FOLLOW_i, 	PIDVAL_CM_FOLLOW_d, -PIDVAL_CM_FOLLOW_limit,PIDVAL_CM_FOLLOW_limit,0);
//	SetPIDCR(&CMFollowPid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
}


/*���̳�ʼ��*/
void CMControlInit(void)
{
	CMPID_Init();   
	CMControlOut(0,0,0,0);
}






/***************************************************************************************
 *Name        : keyboardmove
 *function    : ������̵���ٶȸ���ֵ �ɼ��̿���
 *input       : keyboardvalue
 *output      : �� 
 *Description : �ı���ȫ�ֱ���	speedA, speedB, speedC, speedD

****************************�¼���*****************************************************/
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
//				if(keymove_y>0)    //������ǰ
//					keymove_y += step;           //�Ӳ���
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
//			default:      //û������ʱ
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
//				if(keymove_x>0)    //��������
//					keymove_x += step;           //�Ӳ���
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
//	keymove_x = keymove_x>xlimit?xlimit:(keymove_x<-xlimit?-xlimit:keymove_x);  //����
//	keymove_y = keymove_y>ylimit?ylimit:(keymove_y<-ylimit?-ylimit:keymove_y);
//	if(keymove_x<40&&keymove_x>-40) keymove_x = 0;
//	if(keymove_y<40&&keymove_y>-40) keymove_y = 0;	 //��������	
//}

