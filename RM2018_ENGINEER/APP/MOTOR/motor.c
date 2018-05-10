/************************************************************
 *File		:	
 *Author	:  	@YangTao, ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.3.3
 *Description: 	Engineer robot mechanical executive motors
 ************************************************************/

#include "main.h"

//全局控制寄存器
struct MOTOR_FLAG_REG_Strc {
//	uint16_t Stucked_F: 1;		//卡住标志
//	uint16_t WhoStucked_D: 4;	//卡住具体标志
//	uint16_t StuckSaved_F: 1;	//是否停在之前卡住位置
	uint16_t Moving_F: 1;		//切换下一个位置
	uint16_t CurrPosi_D: 4;		//当前位置
	uint16_t LastPosi_D: 4;		//上一个可以到达的位置
} MOTOR_FLAG_REG;
#define FLAG_SET		1
#define FLAG_CLR		0
#define SetFlag(reg)		((reg)=FLAG_SET)
#define ClearFlag(reg)		((reg)=FLAG_CLR)

//内部函数
void 		Motor2_Stop(void);
void 		Motor2_CalcPosi(void);
void 		Motor2_CalcSpeed(float speed1[4]);
float 		Motor_Legalize(float MotorCurrent , float limit);
uint8_t	 	Motor2_CheckGoal(float goal, float* angles,float filter)	;
//void 		Motor2_CheckStuck(uint8_t strict);
//void		Motor2_ClearStuck(void);
/******************************************* MOTOR2  *******************************************/
/******************************************* MOTOR2  *******************************************/
// 记录六个目标位置
#define 	MOTOR2_POSI_COUNT		6		//预设位置数目
const float Motor2_Position[MOTOR2_POSI_COUNT] = {0.f,8192*0.5f,8192*1.f,8192*1.5f,8192*2.f,8192*2.5f};
//float Motor2_Offset;					//角度偏移(最高点为参考点，它的相对启动位置的角度值作为偏移量)

PID_Struct 	MotorSpeedPid2;  			//
PID_Struct 	MotorAnglePid2[4];  			//

//uint16_t 	Motor2_StuckCount[4]={0};	//卡住计数
float 		Motor2_SetAngel;
float 		Motor2_LastAngel[4];		//前一次角度记录
float 		Motor2_SpeedTrans[4]={0.f,0.f,0.f,0.f};		//发送给电机速度

#define 	MOTOR2_SPEED_p 			1.5f
#define 	MOTOR2_SPEED_i 			0.f
#define 	MOTOR2_SPEED_d 			0.f
#define 	MOTOR2_SPEED_limit 		100.f

#define 	MOTOR2_POSI_p 			1.6f
#define 	MOTOR2_POSI_i 			2.0f
#define 	MOTOR2_POSI_d 			0.f
#define 	MOTOR2_POSI_limit 		500.f

#define 	MOTOR2_OPETION_FILTER	500		//摇杆动作确认值
#define 	MOTOR2_DEFAULT_POSI		2		//电机默认高度
#define 	test_motor_number		4
/***************************************************************************************
 *Name     : 
 *Function ：
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
void Motor2_Configuration(void)
{
	//速度PID初始化
	PID_Init(&MotorSpeedPid2, MOTOR2_SPEED_p, MOTOR2_SPEED_i, MOTOR2_SPEED_d, -MOTOR2_SPEED_limit, MOTOR2_SPEED_limit,0);
	SetPIDCR(&MotorSpeedPid2,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	//位置（角度）PID初始化
	for(uint8_t i=0;i<test_motor_number;i++) {
		PID_Init(&MotorAnglePid2[i], MOTOR2_POSI_p, MOTOR2_POSI_i, MOTOR2_POSI_d, -MOTOR2_POSI_limit, MOTOR2_POSI_limit,0);
		SetPIDCR(&MotorAnglePid2[i],(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	}
	//位置寄存器设置为默认值
//	MOTOR_FLAG_REG.CurrPosi_D = MOTOR2_DEFAULT_POSI;
	MOTOR_FLAG_REG.CurrPosi_D = 0;
	MOTOR_FLAG_REG.LastPosi_D = 0;
	//电机停止运动
	Motor2_Stop();
}
/***************************************************************************************
 *Name     : 
 *Function ：
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
#define MOTOR2_RCGOAL_FILTER		1000.0f		//给遥控器检测是否达到目标的误差值（考虑到控制性能问题，与运动的目标检测分开）
#define MOTOR2_GETGOAL_FILTER		80.0f
void Motor2_Loop(void)
{
	//临时变量
	static uint16_t tmp;
	
	if(remoteState == PREPARE_STATE) {
//		Motor2_SetAngel = Motor2_Position[MOTOR_FLAG_REG.CurrPosi_D];
		Motor2_SetAngel = 0.f;
	}
	else if(remoteState == NORMAL_REMOTE_STATE) {
/*-------------------------- 运动处理 -----------------------------*/
//注意 注意 注意 因为电机的安排问题，注意角度的符号
		
		//要求手动调节位置（考虑到可能的卡住情况）
		switch(GlobalMode){
		case MODE_BULLET:
//			//当前没有进行移动，摇杆值满足要求,且没有卡住则移动
//			if((RC_Ex_Ctl.rc.ch3>MOTOR2_OPETION_FILTER)||(RC_Ex_Ctl.rc.ch3<-MOTOR2_OPETION_FILTER)) {
//				//摇杆值合法，且已经到达当前目标位置则继续移动
//				if(Motor2_CheckGoal(Motor2_Position[MOTOR_FLAG_REG.CurrPosi_D],can2_current_angel,MOTOR2_RCGOAL_FILTER)){
//					tmp = MOTOR_FLAG_REG.CurrPosi_D;
//					tmp += (RC_Ex_Ctl.rc.ch3>0)?(1):(-1);
//					//符合要求，更新目标位置，开始移动
//					if(tmp<MOTOR2_POSI_COUNT) {
//						MOTOR_FLAG_REG.CurrPosi_D = tmp;
//						SetFlag(MOTOR_FLAG_REG.Moving_F);
//					}
//				}
//			}
//			//是否达到目标
//			if(MOTOR_FLAG_REG.Moving_F){
//				if(Motor2_CheckGoal(Motor2_Position[MOTOR_FLAG_REG.CurrPosi_D],can2_current_angel,MOTOR2_GETGOAL_FILTER)){
//					ClearFlag(MOTOR_FLAG_REG.Moving_F);
//					MOTOR_FLAG_REG.LastPosi_D = MOTOR_FLAG_REG.CurrPosi_D;
//				}
//			}
//			//获取目标角度
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
				//非合法操作
				ClearFlag(MOTOR_FLAG_REG.Moving_F);
			}

			if(RemoteSEF_REG&RemoteSEF_S1_UP){			//s1产生上拨动作
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
/*-------------------------- 后处理 -----------------------------*/
		Motor2_CalcPosi();
		//记录上次角度
		for(tmp=0;tmp<test_motor_number;tmp++){
			Motor2_LastAngel[tmp] = can2_current_angel[tmp];
		}
		disp0[4] = Motor2_SetAngel;
		disp0[5] = MOTOR_FLAG_REG.Moving_F;
	}
	else if(remoteState == STANDBY_STATE) {		//错误和待机模式下保留当前位置
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
 *Function ：
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
void Motor2_Stop(void)
{
	CAN2_Motor(0,0,0,0);
}
/***************************************************************************************
 *Name     : 
 *Function ：位置环+速度环控制
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
//#define DELTA_TINY_SCALE	5.f					//微小角度偏差放大
#define DELTA_MAX_FILTER	5000.f				//限制最大角度增量
#define DELTA_MIN_FILTER	10.f				//限制最小角度增量
//#define POSI_OUT_LIMIT		7000.f				//位置环输出过滤(比速度上限大一些)			
//计算位置环
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
		//位置环输出限制
//		Motor2_SpeedTrans[tmp] = Motor_Legalize(Motor2_SpeedTrans[tmp],POSI_OUT_LIMIT);
	}
	Motor2_CalcSpeed(Motor2_SpeedTrans);
}
/***************************************************************************************
 *Name     : 
 *Function ：速度环控制
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
#define MOTOR2_SPEEDLIMIT		3500	//速度上限
#define MOTOR2_FILTER			20.0f	//电机速度过滤
void Motor2_CalcSpeed(float speed[4])
{
	uint8_t tmp;
	
	for(tmp=0;tmp<test_motor_number;tmp++){
		if((speed[tmp]>MOTOR2_FILTER)||(speed[tmp]<-MOTOR2_FILTER)){
			Motor2_SpeedTrans[tmp] = PID_Driver(speed[tmp], can2_current_motor_speed[tmp], &MotorSpeedPid2, 1000);
		} else {
			Motor2_SpeedTrans[tmp] = 0;
		}
		//幅值限制
		Motor2_SpeedTrans[tmp] = Motor_Legalize(Motor2_SpeedTrans[tmp],MOTOR2_SPEEDLIMIT);
	}
	//发送速度
	disp0[0] = Motor2_SpeedTrans[0];
	disp0[1] = Motor2_SpeedTrans[1];
	disp0[2] = Motor2_SpeedTrans[2];
	disp0[3] = Motor2_SpeedTrans[3];
	CAN2_Motor(Motor2_SpeedTrans[0], Motor2_SpeedTrans[1],Motor2_SpeedTrans[2],Motor2_SpeedTrans[3]);
}
/***************************************************************************************
 *Name     : 
 *Function ：检测是否达到目标值
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
//用绝对误差之和来判定是否到达目标
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
 *Function ：检测是否卡住
 *Input    ：strict		是否严格检查（有一个卡就算卡住）
 *Output   ：无 
 *Description : 
****************************************************************************************/
//#define MOTOR2_STUCKCHK_MODE		3510
////检测是不是卡住了
//void Motor2_CheckStuck(uint8_t strict)	
//{
//	uint8_t tmp;
//#if(MOTOR2_STUCKCHK_MODE==3510)
//	#define MOTOR2_SPEED_CHECK		800		//大于这个数值，电机应该转动
//	#define MOTOR2_STUCK_COUNT		500		//连续检测到角度过小的周期数(0.1ms)
//	//电机速度输出在合理范围内不动为堵转（3510和3508都可）
//	for(tmp=0;tmp<test_motor_number;tmp++){
//		if((Motor2_SpeedTrans[tmp]>MOTOR2_SPEED_CHECK) || (Motor2_SpeedTrans[tmp]<-MOTOR2_SPEED_CHECK)){
//			if(can2_stuckflag[0]>MOTOR2_STUCK_COUNT){
//				MOTOR_FLAG_REG.WhoStucked_D |= (1<<tmp);
//			}
//		}
//	}
//#elif(MOTOR2_STUCKCHK_MODE==3508)
//	#define MOTOR2_CURRENT_CHECK	500		//电流合理值
//	#define MOTOR2_STUCK_COUNT		100		//连续检测到角度过小的周期数(0.1ms)
//	//电机返回转矩电流在合理范围内，但转速（检测角度）几乎为0视为堵转（仅3508适用）
//	for(tmp=0;tmp<test_motor_number;tmp++){
//		if((can2_current_motor_current[tmp]>MOTOR2_CURRENT_CHECK) || (can2_current_motor_current[tmp]<-MOTOR2_CURRENT_CHECK)){
//			if(can2_stuckflag[0]>MOTOR2_STUCK_COUNT){
//				MOTOR_FLAG_REG.WhoStucked_D |= (1<<tmp);
//			}
//		}
//	}
//#endif
//	if(strict){
//		//有一个卡就算卡住
//		if(MOTOR_FLAG_REG.WhoStucked_D) SetFlag(MOTOR_FLAG_REG.Stucked_F);
//	}
//	else{
//		//全都卡就算卡住
//		if(MOTOR_FLAG_REG.WhoStucked_D==0xf) SetFlag(MOTOR_FLAG_REG.Stucked_F);
//	}
////	float delta;
////	if(!MOTOR_FLAG_REG.Stucked_F) {
////		for(tmp=0;tmp<test_motor_number;tmp++){
////			delta = can2_current_angel[tmp] - Motor2_LastAngel[tmp];
////			//角度增量过小
////			if((delta<STUCK_FILTER)&&(delta>-STUCK_FILTER)) {
////				Motor2_StuckCount[tmp]++;
////				Motor2_StuckCount[tmp] &= 0xfff;		//防止溢出清零
////			} else Motor2_StuckCount[tmp] = 0;
////			//设置/清除卡住标志位
////			if(Motor2_StuckCount[tmp]>STUCK_COUNT){
////				SetFlag(MOTOR_FLAG_REG.Stucked_F);
////			}
////		}
////	}
//}
/***************************************************************************************
 *Name     : 
 *Function ：幅值限制
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
float Motor_Legalize(float MotorCurrent , float limit)
{
	return (MotorCurrent<-limit)?-limit:((MotorCurrent>limit)?limit:MotorCurrent);
}
/***************************************************************************************
 *Name     : 
 *Function ：
 *Input    ：
 *Output   ：无 
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
//						//摇杆值合法，且已经到达当前目标位置则继续移动
//						tmp=0;
//						while(tmp<test_motor_number) {
//							if(fabs(can2_current_angel[tmp]-Motor2_Position[MOTOR_FLAG_REG.CurrPosi_D])>MOTOR2_RCGOAL_FILTER){
//								break;
//							}
//							tmp++;
//						}
//						if(tmp>=test_motor_number){
//							//重置目标位置
//							tmp = MOTOR_FLAG_REG.CurrPosi_D;
//							tmp += (RC_Ex_Ctl.rc.ch3>0)?(1):(-1);
//							//符合要求，更新目标位置，开始移动
//							if(tmp<MOTOR2_POSI_COUNT) {
//								Motor2_ClearStuck();
//								MOTOR_FLAG_REG.CurrPosi_D = tmp;
//								SetFlag(MOTOR_FLAG_REG.Moving_F);
//							}
//						}
//					}else{
//						//摇杆值合法，但卡住
//						BUZZER_ON(1500);
//					}
//				}else {
//					//摇杆值不合法
//					BUZZER_OFF();
//					//非有效操作下，清除此位以假设用户已经排除所有意外卡住情况
//					Motor2_ClearStuck();
//				}
//			}

