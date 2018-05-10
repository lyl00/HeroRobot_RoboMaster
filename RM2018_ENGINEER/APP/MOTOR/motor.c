/************************************************************
 *File		:	
 *Author	:  	@YangTao, ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.3.3
 *Description: 	Engineer robot mechanical executive motors
 ************************************************************/

#include "main.h"

//ȫ�ֿ��ƼĴ���
struct MOTOR_FLAG_REG_Strc {
//	uint16_t Stucked_F: 1;		//��ס��־
//	uint16_t WhoStucked_D: 4;	//��ס�����־
//	uint16_t StuckSaved_F: 1;	//�Ƿ�ͣ��֮ǰ��סλ��
	uint16_t Moving_F: 1;		//�л���һ��λ��
	uint16_t CurrPosi_D: 4;		//��ǰλ��
	uint16_t LastPosi_D: 4;		//��һ�����Ե����λ��
} MOTOR_FLAG_REG;
#define FLAG_SET		1
#define FLAG_CLR		0
#define SetFlag(reg)		((reg)=FLAG_SET)
#define ClearFlag(reg)		((reg)=FLAG_CLR)

//�ڲ�����
void 		Motor2_Stop(void);
void 		Motor2_CalcPosi(void);
void 		Motor2_CalcSpeed(float speed1[4]);
float 		Motor_Legalize(float MotorCurrent , float limit);
uint8_t	 	Motor2_CheckGoal(float goal, float* angles,float filter)	;
//void 		Motor2_CheckStuck(uint8_t strict);
//void		Motor2_ClearStuck(void);
/******************************************* MOTOR2  *******************************************/
/******************************************* MOTOR2  *******************************************/
// ��¼����Ŀ��λ��
#define 	MOTOR2_POSI_COUNT		6		//Ԥ��λ����Ŀ
const float Motor2_Position[MOTOR2_POSI_COUNT] = {0.f,8192*0.5f,8192*1.f,8192*1.5f,8192*2.f,8192*2.5f};
//float Motor2_Offset;					//�Ƕ�ƫ��(��ߵ�Ϊ�ο��㣬�����������λ�õĽǶ�ֵ��Ϊƫ����)

PID_Struct 	MotorSpeedPid2;  			//
PID_Struct 	MotorAnglePid2[4];  			//

//uint16_t 	Motor2_StuckCount[4]={0};	//��ס����
float 		Motor2_SetAngel;
float 		Motor2_LastAngel[4];		//ǰһ�νǶȼ�¼
float 		Motor2_SpeedTrans[4]={0.f,0.f,0.f,0.f};		//���͸�����ٶ�

#define 	MOTOR2_SPEED_p 			1.5f
#define 	MOTOR2_SPEED_i 			0.f
#define 	MOTOR2_SPEED_d 			0.f
#define 	MOTOR2_SPEED_limit 		100.f

#define 	MOTOR2_POSI_p 			1.6f
#define 	MOTOR2_POSI_i 			2.0f
#define 	MOTOR2_POSI_d 			0.f
#define 	MOTOR2_POSI_limit 		500.f

#define 	MOTOR2_OPETION_FILTER	500		//ҡ�˶���ȷ��ֵ
#define 	MOTOR2_DEFAULT_POSI		2		//���Ĭ�ϸ߶�
#define 	test_motor_number		4
/***************************************************************************************
 *Name     : 
 *Function ��
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
void Motor2_Configuration(void)
{
	//�ٶ�PID��ʼ��
	PID_Init(&MotorSpeedPid2, MOTOR2_SPEED_p, MOTOR2_SPEED_i, MOTOR2_SPEED_d, -MOTOR2_SPEED_limit, MOTOR2_SPEED_limit,0);
	SetPIDCR(&MotorSpeedPid2,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	//λ�ã��Ƕȣ�PID��ʼ��
	for(uint8_t i=0;i<test_motor_number;i++) {
		PID_Init(&MotorAnglePid2[i], MOTOR2_POSI_p, MOTOR2_POSI_i, MOTOR2_POSI_d, -MOTOR2_POSI_limit, MOTOR2_POSI_limit,0);
		SetPIDCR(&MotorAnglePid2[i],(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	}
	//λ�üĴ�������ΪĬ��ֵ
//	MOTOR_FLAG_REG.CurrPosi_D = MOTOR2_DEFAULT_POSI;
	MOTOR_FLAG_REG.CurrPosi_D = 0;
	MOTOR_FLAG_REG.LastPosi_D = 0;
	//���ֹͣ�˶�
	Motor2_Stop();
}
/***************************************************************************************
 *Name     : 
 *Function ��
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
#define MOTOR2_RCGOAL_FILTER		1000.0f		//��ң��������Ƿ�ﵽĿ������ֵ�����ǵ������������⣬���˶���Ŀ����ֿ���
#define MOTOR2_GETGOAL_FILTER		80.0f
void Motor2_Loop(void)
{
	//��ʱ����
	static uint16_t tmp;
	
	if(remoteState == PREPARE_STATE) {
//		Motor2_SetAngel = Motor2_Position[MOTOR_FLAG_REG.CurrPosi_D];
		Motor2_SetAngel = 0.f;
	}
	else if(remoteState == NORMAL_REMOTE_STATE) {
/*-------------------------- �˶����� -----------------------------*/
//ע�� ע�� ע�� ��Ϊ����İ������⣬ע��Ƕȵķ���
		
		//Ҫ���ֶ�����λ�ã����ǵ����ܵĿ�ס�����
		switch(GlobalMode){
		case MODE_BULLET:
//			//��ǰû�н����ƶ���ҡ��ֵ����Ҫ��,��û�п�ס���ƶ�
//			if((RC_Ex_Ctl.rc.ch3>MOTOR2_OPETION_FILTER)||(RC_Ex_Ctl.rc.ch3<-MOTOR2_OPETION_FILTER)) {
//				//ҡ��ֵ�Ϸ������Ѿ����ﵱǰĿ��λ��������ƶ�
//				if(Motor2_CheckGoal(Motor2_Position[MOTOR_FLAG_REG.CurrPosi_D],can2_current_angel,MOTOR2_RCGOAL_FILTER)){
//					tmp = MOTOR_FLAG_REG.CurrPosi_D;
//					tmp += (RC_Ex_Ctl.rc.ch3>0)?(1):(-1);
//					//����Ҫ�󣬸���Ŀ��λ�ã���ʼ�ƶ�
//					if(tmp<MOTOR2_POSI_COUNT) {
//						MOTOR_FLAG_REG.CurrPosi_D = tmp;
//						SetFlag(MOTOR_FLAG_REG.Moving_F);
//					}
//				}
//			}
//			//�Ƿ�ﵽĿ��
//			if(MOTOR_FLAG_REG.Moving_F){
//				if(Motor2_CheckGoal(Motor2_Position[MOTOR_FLAG_REG.CurrPosi_D],can2_current_angel,MOTOR2_GETGOAL_FILTER)){
//					ClearFlag(MOTOR_FLAG_REG.Moving_F);
//					MOTOR_FLAG_REG.LastPosi_D = MOTOR_FLAG_REG.CurrPosi_D;
//				}
//			}
//			//��ȡĿ��Ƕ�
//			Motor2_SetAngel = Motor2_Position[MOTOR_FLAG_REG.CurrPosi_D];
			break;
		case MODE_LIFT:
			if(RC_Ex_Ctl.rc.ch3>50 || RC_Ex_Ctl.rc.ch3<-50){
				if(!MOTOR_FLAG_REG.Moving_F){
					SetFlag(MOTOR_FLAG_REG.Moving_F);
					Motor2_SetAngel += 500.f;
				}else{
					Motor2_SetAngel += RC_Ex_Ctl.rc.ch3*0.02f;
				}
			}else{
				//�ǺϷ�����
				ClearFlag(MOTOR_FLAG_REG.Moving_F);
			}

			if(RemoteSEF_REG&RemoteSEF_S1_UP){			//s1�����ϲ�����
				BUZZER_OFF();
				MOTOR_FLAG_REG.CurrPosi_D = 0;
				Motor2_SetAngel = 0.0;
				can2_current_angel[0] = 0.0;
				can2_current_angel[1] = 0.0;
				can2_current_angel[2] = 0.0;
				can2_current_angel[3] = 0.0;
			}
			break;
		default:
			break;
		}
/*-------------------------- ���� -----------------------------*/
		Motor2_CalcPosi();
		//��¼�ϴνǶ�
		for(tmp=0;tmp<test_motor_number;tmp++){
			Motor2_LastAngel[tmp] = can2_current_angel[tmp];
		}
		disp0[4] = Motor2_SetAngel;
		disp0[5] = MOTOR_FLAG_REG.Moving_F;
	}
	else if(remoteState == STANDBY_STATE) {		//����ʹ���ģʽ�±�����ǰλ��
		Motor2_Stop();
	}
	else if(remoteState == ERROR_STATE) {
		Motor2_Stop();
	}
	else if(remoteState == KEY_REMOTE_STATE) {
		Motor2_Stop();
	}
}
/***************************************************************************************
 *Name     : 
 *Function ��
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
void Motor2_Stop(void)
{
	CAN2_Motor(0,0,0,0);
}
/***************************************************************************************
 *Name     : 
 *Function ��λ�û�+�ٶȻ�����
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
//#define DELTA_TINY_SCALE	5.f					//΢С�Ƕ�ƫ��Ŵ�
#define DELTA_MAX_FILTER	5000.f				//�������Ƕ�����
#define DELTA_MIN_FILTER	10.f				//������С�Ƕ�����
//#define POSI_OUT_LIMIT		7000.f				//λ�û��������(���ٶ����޴�һЩ)			
//����λ�û�
void Motor2_CalcPosi(void)
{
	float delta;
	uint8_t tmp;
	
	for(tmp=0;tmp<test_motor_number;tmp++){
		delta = (Motor2_SetAngel - can2_current_angel[tmp]);
		if((delta>DELTA_MIN_FILTER)||(delta<-DELTA_MIN_FILTER)){
			delta = (delta>DELTA_MAX_FILTER)? DELTA_MAX_FILTER:((delta<-DELTA_MAX_FILTER)? -DELTA_MAX_FILTER:delta);
			Motor2_SpeedTrans[tmp] = PID_Driver(can2_current_angel[tmp]+delta, can2_current_angel[tmp], &MotorAnglePid2[tmp], 2000);
		}else{
			Motor2_SpeedTrans[tmp] = 0.f;
		}
		if(tmp==2||tmp==3) {
			Motor2_SpeedTrans[tmp] = -Motor2_SpeedTrans[tmp];
		}
		//λ�û��������
//		Motor2_SpeedTrans[tmp] = Motor_Legalize(Motor2_SpeedTrans[tmp],POSI_OUT_LIMIT);
	}
	Motor2_CalcSpeed(Motor2_SpeedTrans);
}
/***************************************************************************************
 *Name     : 
 *Function ���ٶȻ�����
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
#define MOTOR2_SPEEDLIMIT		3500	//�ٶ�����
#define MOTOR2_FILTER			20.0f	//����ٶȹ���
void Motor2_CalcSpeed(float speed[4])
{
	uint8_t tmp;
	
	for(tmp=0;tmp<test_motor_number;tmp++){
		if((speed[tmp]>MOTOR2_FILTER)||(speed[tmp]<-MOTOR2_FILTER)){
			Motor2_SpeedTrans[tmp] = PID_Driver(speed[tmp], can2_current_motor_speed[tmp], &MotorSpeedPid2, 1000);
		} else {
			Motor2_SpeedTrans[tmp] = 0;
		}
		//��ֵ����
		Motor2_SpeedTrans[tmp] = Motor_Legalize(Motor2_SpeedTrans[tmp],MOTOR2_SPEEDLIMIT);
	}
	//�����ٶ�
	disp0[0] = Motor2_SpeedTrans[0];
	disp0[1] = Motor2_SpeedTrans[1];
	disp0[2] = Motor2_SpeedTrans[2];
	disp0[3] = Motor2_SpeedTrans[3];
	CAN2_Motor(Motor2_SpeedTrans[0], Motor2_SpeedTrans[1],Motor2_SpeedTrans[2],Motor2_SpeedTrans[3]);
}
/***************************************************************************************
 *Name     : 
 *Function ������Ƿ�ﵽĿ��ֵ
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
//�þ������֮�����ж��Ƿ񵽴�Ŀ��
uint8_t Motor2_CheckGoal(float goal, float* angles,float filter)	
{
	uint8_t tmp=0;
	while(tmp<test_motor_number) {
		if(fabs(angles[tmp]-goal)>filter){
			break;
		}
		tmp++;
	}
	return tmp>=test_motor_number;
}
/***************************************************************************************
 *Name     : 
 *Function ������Ƿ�ס
 *Input    ��strict		�Ƿ��ϸ��飨��һ�������㿨ס��
 *Output   ���� 
 *Description : 
****************************************************************************************/
//#define MOTOR2_STUCKCHK_MODE		3510
////����ǲ��ǿ�ס��
//void Motor2_CheckStuck(uint8_t strict)	
//{
//	uint8_t tmp;
//#if(MOTOR2_STUCKCHK_MODE==3510)
//	#define MOTOR2_SPEED_CHECK		800		//���������ֵ�����Ӧ��ת��
//	#define MOTOR2_STUCK_COUNT		500		//������⵽�Ƕȹ�С��������(0.1ms)
//	//����ٶ�����ں���Χ�ڲ���Ϊ��ת��3510��3508���ɣ�
//	for(tmp=0;tmp<test_motor_number;tmp++){
//		if((Motor2_SpeedTrans[tmp]>MOTOR2_SPEED_CHECK) || (Motor2_SpeedTrans[tmp]<-MOTOR2_SPEED_CHECK)){
//			if(can2_stuckflag[0]>MOTOR2_STUCK_COUNT){
//				MOTOR_FLAG_REG.WhoStucked_D |= (1<<tmp);
//			}
//		}
//	}
//#elif(MOTOR2_STUCKCHK_MODE==3508)
//	#define MOTOR2_CURRENT_CHECK	500		//��������ֵ
//	#define MOTOR2_STUCK_COUNT		100		//������⵽�Ƕȹ�С��������(0.1ms)
//	//�������ת�ص����ں���Χ�ڣ���ת�٣����Ƕȣ�����Ϊ0��Ϊ��ת����3508���ã�
//	for(tmp=0;tmp<test_motor_number;tmp++){
//		if((can2_current_motor_current[tmp]>MOTOR2_CURRENT_CHECK) || (can2_current_motor_current[tmp]<-MOTOR2_CURRENT_CHECK)){
//			if(can2_stuckflag[0]>MOTOR2_STUCK_COUNT){
//				MOTOR_FLAG_REG.WhoStucked_D |= (1<<tmp);
//			}
//		}
//	}
//#endif
//	if(strict){
//		//��һ�������㿨ס
//		if(MOTOR_FLAG_REG.WhoStucked_D) SetFlag(MOTOR_FLAG_REG.Stucked_F);
//	}
//	else{
//		//ȫ�������㿨ס
//		if(MOTOR_FLAG_REG.WhoStucked_D==0xf) SetFlag(MOTOR_FLAG_REG.Stucked_F);
//	}
////	float delta;
////	if(!MOTOR_FLAG_REG.Stucked_F) {
////		for(tmp=0;tmp<test_motor_number;tmp++){
////			delta = can2_current_angel[tmp] - Motor2_LastAngel[tmp];
////			//�Ƕ�������С
////			if((delta<STUCK_FILTER)&&(delta>-STUCK_FILTER)) {
////				Motor2_StuckCount[tmp]++;
////				Motor2_StuckCount[tmp] &= 0xfff;		//��ֹ�������
////			} else Motor2_StuckCount[tmp] = 0;
////			//����/�����ס��־λ
////			if(Motor2_StuckCount[tmp]>STUCK_COUNT){
////				SetFlag(MOTOR_FLAG_REG.Stucked_F);
////			}
////		}
////	}
//}
/***************************************************************************************
 *Name     : 
 *Function ����ֵ����
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
float Motor_Legalize(float MotorCurrent , float limit)
{
	return (MotorCurrent<-limit)?-limit:((MotorCurrent>limit)?limit:MotorCurrent);
}
/***************************************************************************************
 *Name     : 
 *Function ��
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
//void Motor2_ClearStuck(void)
//{
//	BUZZER_OFF();
//	ClearFlag(MOTOR_FLAG_REG.Stucked_F);
//	ClearFlag(MOTOR_FLAG_REG.StuckSaved_F);
//	ClearFlag(MOTOR_FLAG_REG.WhoStucked_D);
////	Motor2_StuckCount[0] = 0;
////	Motor2_StuckCount[1] = 0;
////	Motor2_StuckCount[2] = 0;
////	Motor2_StuckCount[3] = 0;
//}


//			if(!MOTOR_FLAG_REG.Moving_F){
//				if((RC_Ex_Ctl.rc.ch3>MOTOR2_OPETION_FILTER)||(RC_Ex_Ctl.rc.ch3<-MOTOR2_OPETION_FILTER)) {
//					if(!MOTOR_FLAG_REG.Stucked_F) {
//						//ҡ��ֵ�Ϸ������Ѿ����ﵱǰĿ��λ��������ƶ�
//						tmp=0;
//						while(tmp<test_motor_number) {
//							if(fabs(can2_current_angel[tmp]-Motor2_Position[MOTOR_FLAG_REG.CurrPosi_D])>MOTOR2_RCGOAL_FILTER){
//								break;
//							}
//							tmp++;
//						}
//						if(tmp>=test_motor_number){
//							//����Ŀ��λ��
//							tmp = MOTOR_FLAG_REG.CurrPosi_D;
//							tmp += (RC_Ex_Ctl.rc.ch3>0)?(1):(-1);
//							//����Ҫ�󣬸���Ŀ��λ�ã���ʼ�ƶ�
//							if(tmp<MOTOR2_POSI_COUNT) {
//								Motor2_ClearStuck();
//								MOTOR_FLAG_REG.CurrPosi_D = tmp;
//								SetFlag(MOTOR_FLAG_REG.Moving_F);
//							}
//						}
//					}else{
//						//ҡ��ֵ�Ϸ�������ס
//						BUZZER_ON(1500);
//					}
//				}else {
//					//ҡ��ֵ���Ϸ�
//					BUZZER_OFF();
//					//����Ч�����£������λ�Լ����û��Ѿ��ų��������⿨ס���
//					Motor2_ClearStuck();
//				}
//			}

