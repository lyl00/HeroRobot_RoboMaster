/************************************************************
 *File		:	��������ת���
 *Author	:  	@YangTao, ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.3.3
 *Description: 	Engineer robot mechanical executive motors
 ************************************************************/

#include "main.h"

//ȫ�ֿ��ƼĴ���
struct ROTATE_FLAG_REG_Strc {
	uint16_t BaoMove: 1;			//�ƶ�Bao���
	uint16_t BaoDirect: 1;			//Bao���ת��,0=��,1=�ر�
	uint16_t FanMoving_F: 1;		//Fan�л���һ��λ��
	uint16_t FanCurrPosi_D: 4;		//Fan��ǰλ��
	uint16_t : 8;
} ROTATE_FLAG_REG;
#define FLAG_SET			1
#define FLAG_CLR			0
#define SetFlag(reg)		((reg)=FLAG_SET)
#define ClearFlag(reg)		((reg)=FLAG_CLR)

//PID
PID_Struct MotorSpeedPid_Bao,MotorSpeedPid_Fan;
PID_Struct MotorAnglePid_Fan;		//MotorAnglePid_Bao



//��ס����
//uint16_t 	Rotate_StuckCount_Bao,Rotate_StuckCount_Fan;
//Ŀ��Ƕ�
float 		Rotate_SetAngel_Bao,Rotate_SetAngel_Fan;
//ǰһ�νǶȼ�¼
float 		Rotate_LastAngel_Bao,Rotate_LastAngel_Fan;
//���͸�����ٶ�
float 		Rotate_SpeedTrans_Bao,Rotate_SpeedTrans_Fan;

#define ROTATE_SPEED_p 			1.5f
#define ROTATE_SPEED_i 			0.f
#define ROTATE_SPEED_d 			0.f
#define ROTATE_SPEED_limit 		100.f

#define ROTATE_POSI_p 			2.5f
#define ROTATE_POSI_i 			1.5f
#define ROTATE_POSI_d 			0.f
#define ROTATE_POSI_limit 		200.f

#define ROTATE_CURR_ANGLE_Bao		can1_current_angel205
#define ROTATE_CURR_ANGLE_Fan		can1_current_angel206
#define ROTATE_CURR_SPEED_Bao		can1_current_motor_speed205
#define ROTATE_CURR_SPEED_Fan		can1_current_motor_speed206
#define ROTATE_CURR_CURRENT_Bao		can1_current_motor_current205
#define ROTATE_CURR_CURRENT_Fan		can1_current_motor_current206

void 		Rotate_Stop(void);
float 		Rotate_Legalize(float MotorCurrent , float limit);
uint8_t 	Rotate_CheckGoal(float goal, float angles);
void 		Rotate_Move(void);
void 		Rotate_CalcSpeed(float speedA, float speedB);

/***************************************************************************************
 *Name     : 
 *Function ��
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
void Rotate_Configuration(void)
{
	//�ٶ�PID��ʼ��
	PID_Init(&MotorSpeedPid_Bao, ROTATE_SPEED_p, ROTATE_SPEED_i, ROTATE_SPEED_d, -ROTATE_POSI_limit, ROTATE_POSI_limit,0);
	SetPIDCR(&MotorSpeedPid_Bao,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorSpeedPid_Fan, ROTATE_SPEED_p, ROTATE_SPEED_i, ROTATE_SPEED_d, -ROTATE_POSI_limit, ROTATE_POSI_limit,0);
	SetPIDCR(&MotorSpeedPid_Fan,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	//λ�ã��Ƕȣ�PID��ʼ��
	PID_Init(&MotorAnglePid_Fan, ROTATE_POSI_p, ROTATE_POSI_i, ROTATE_POSI_d, -ROTATE_POSI_limit, ROTATE_POSI_limit,0);
	SetPIDCR(&MotorAnglePid_Fan,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	ROTATE_FLAG_REG.BaoMove = 0;
	ROTATE_FLAG_REG.BaoDirect = 0;
	ROTATE_FLAG_REG.FanMoving_F = 0;
	ROTATE_FLAG_REG.FanCurrPosi_D = 0;
	//���ֹͣ�˶�
	Rotate_Stop();
}
/***************************************************************************************
 *Name     : 
 *Function ��
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
const float Rotate_Posi_Fan[2] = {3072.f,0.f};

void Rotate_Loop(void)
{	
	switch(GlobalMode){
		case MODE_LIFT:
			#ifdef RC_TEST
			if(RemoteSEF_REG&RemoteSEF_S1_UP){			//s1�����ϲ�����
				Rotate_Bao_Toggle();
			}
			#endif
			break;
		default:
			ClearFlag(ROTATE_FLAG_REG.BaoMove);
	}
/*-------------------------- ���� -----------------------------*/
	if(ROTATE_FLAG_REG.FanMoving_F){
		//����Ƿ񵽴�Ŀ��
		if(Rotate_CheckGoal(Rotate_SetAngel_Fan,ROTATE_CURR_ANGLE_Fan)){
			ClearFlag(ROTATE_FLAG_REG.FanMoving_F);
		}
	}
	Rotate_SetAngel_Fan = Rotate_Posi_Fan[ROTATE_FLAG_REG.FanCurrPosi_D];
	Rotate_LastAngel_Fan = ROTATE_CURR_ANGLE_Fan;
	Rotate_Move();
}
/***************************************************************************************
 ��������˶�����
****************************************************************************************/
void Rotate_Bao_Start(void)
{
	//��ʼ��ת
	ROTATE_FLAG_REG.BaoMove = 1;
}
void Rotate_Bao_Stop(void)
{
	//������ת
	ROTATE_FLAG_REG.BaoMove = 0;
}
void Rotate_Bao_Toggle(void)
{
	//�л���ת
	ROTATE_FLAG_REG.BaoMove = ~ROTATE_FLAG_REG.BaoMove;
}
void Rotate_Bao_DirePos(void)
{
	//��ת
	ROTATE_FLAG_REG.BaoDirect = 0;
}
void Rotate_Bao_DireRev(void)
{
	//��ת
	ROTATE_FLAG_REG.BaoDirect = 1;
}
void Rotate_Bao_DireToggle(void)
{
	//�л�����
	ROTATE_FLAG_REG.BaoDirect = ~ROTATE_FLAG_REG.BaoDirect;
}
/***************************************************************************************
 �������˶�����
****************************************************************************************/
void Rotate_Fan(void)
{
	//�л�λ��
	ROTATE_FLAG_REG.FanCurrPosi_D = (ROTATE_FLAG_REG.FanCurrPosi_D+1)%2;
}

/***************************************************************************************
 *Name     : 
 *Function ��
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
void Rotate_Stop(void)
{
	CAN1_Rotate(0,0);
}
/***************************************************************************************
 *Name     : 
 *Function ��λ�û�+�ٶȻ�����
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
#define DELTA_MAX_FILTER	5000.f				//�������Ƕ�����
#define DELTA_MIN_FILTER	10.f				//������С�Ƕ�����
#define POSI_OUT_LIMIT		10000.f				//λ�û��������(���ٶ����޴�һЩ)
#define ROTATE_BAO_POSI		1
void Rotate_Move(void)
{
	float delta;
	//������
	delta = (Rotate_SetAngel_Fan - ROTATE_CURR_ANGLE_Fan);
	if((delta>DELTA_MIN_FILTER)||(delta<-DELTA_MIN_FILTER)){
		delta = (delta>DELTA_MAX_FILTER)? DELTA_MAX_FILTER:((delta<-DELTA_MAX_FILTER)? -DELTA_MAX_FILTER:delta);
		Rotate_SpeedTrans_Fan = PID_Driver(ROTATE_CURR_ANGLE_Fan+delta, ROTATE_CURR_ANGLE_Fan, &MotorAnglePid_Fan, 2000);
	}else{
		Rotate_SpeedTrans_Fan = 0.f;
	}
	//λ�û��������
	Rotate_SpeedTrans_Bao = (ROTATE_FLAG_REG.BaoMove)?((ROTATE_FLAG_REG.BaoDirect)?-1200:1200):0;
	Rotate_SpeedTrans_Fan = Rotate_Legalize(Rotate_SpeedTrans_Fan,POSI_OUT_LIMIT);
	
	//����ٶ�
	Rotate_CalcSpeed(Rotate_SpeedTrans_Bao,Rotate_SpeedTrans_Fan);
}
/***************************************************************************************
 *Name     : 
 *Function ���ٶȻ�����
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
#define ROTATE_SPEEDLIMIT		3000.0f	//�ٶ�����
#define ROTATE_FILTER			20.0f	//����ٶȹ���
void Rotate_CalcSpeed(float speed_Bao, float speed_Fan)
{	
	//�������
	if((speed_Bao>ROTATE_FILTER)||(speed_Bao<-ROTATE_FILTER)){
		Rotate_SpeedTrans_Bao = PID_Driver(speed_Bao, ROTATE_CURR_SPEED_Bao, &MotorSpeedPid_Bao, 1000);
	} else {
		Rotate_SpeedTrans_Bao = 0;
	}
	//������
	if((speed_Fan>ROTATE_FILTER)||(speed_Fan<-ROTATE_FILTER)){
		Rotate_SpeedTrans_Fan = PID_Driver(speed_Fan, ROTATE_CURR_SPEED_Fan, &MotorSpeedPid_Fan, 1000);
	} else {
		Rotate_SpeedTrans_Fan = 0;
	}
	//��ֵ����
	Rotate_SpeedTrans_Bao = Rotate_Legalize(Rotate_SpeedTrans_Bao,ROTATE_SPEEDLIMIT);
	Rotate_SpeedTrans_Fan = Rotate_Legalize(Rotate_SpeedTrans_Fan,ROTATE_SPEEDLIMIT);

	CAN1_Rotate(Rotate_SpeedTrans_Bao, Rotate_SpeedTrans_Fan);
}



/***************************************************************************************
 *Name     : 
 *Function ������Ƿ�ﵽĿ��ֵ
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
#define ROTATE_GETGOAL_FILTER		50.0f
uint8_t Rotate_CheckGoal(float goal, float angles)	
{
	return fabs(goal-angles)<ROTATE_GETGOAL_FILTER;
}
/***************************************************************************************
 *Name     : 
 *Function ����ֵ����
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
float Rotate_Legalize(float MotorCurrent, float limit)
{
	return (MotorCurrent<-limit)?-limit:((MotorCurrent>limit)?limit:MotorCurrent);
}

