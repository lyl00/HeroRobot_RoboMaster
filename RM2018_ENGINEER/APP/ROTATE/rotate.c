/************************************************************
 *File		:	两个个旋转电机
 *Author	:  	@YangTao, ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.3.3
 *Description: 	Engineer robot mechanical executive motors
 ************************************************************/

#include "main.h"

//全局控制寄存器
struct ROTATE_FLAG_REG_Strc {
	uint16_t BaoMove: 1;			//移动Bao电机
	uint16_t BaoDirect: 1;			//Bao电机转向,0=打开,1=关闭
	uint16_t FanMoving_F: 1;		//Fan切换下一个位置
	uint16_t FanCurrPosi_D: 4;		//Fan当前位置
	uint16_t : 8;
} ROTATE_FLAG_REG;
#define FLAG_SET			1
#define FLAG_CLR			0
#define SetFlag(reg)		((reg)=FLAG_SET)
#define ClearFlag(reg)		((reg)=FLAG_CLR)

//PID
PID_Struct MotorSpeedPid_Bao,MotorSpeedPid_Fan;
PID_Struct MotorAnglePid_Fan;		//MotorAnglePid_Bao



//卡住计数
//uint16_t 	Rotate_StuckCount_Bao,Rotate_StuckCount_Fan;
//目标角度
float 		Rotate_SetAngel_Bao,Rotate_SetAngel_Fan;
//前一次角度记录
float 		Rotate_LastAngel_Bao,Rotate_LastAngel_Fan;
//发送给电机速度
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
 *Function ：
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
void Rotate_Configuration(void)
{
	//速度PID初始化
	PID_Init(&MotorSpeedPid_Bao, ROTATE_SPEED_p, ROTATE_SPEED_i, ROTATE_SPEED_d, -ROTATE_POSI_limit, ROTATE_POSI_limit,0);
	SetPIDCR(&MotorSpeedPid_Bao,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&MotorSpeedPid_Fan, ROTATE_SPEED_p, ROTATE_SPEED_i, ROTATE_SPEED_d, -ROTATE_POSI_limit, ROTATE_POSI_limit,0);
	SetPIDCR(&MotorSpeedPid_Fan,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	//位置（角度）PID初始化
	PID_Init(&MotorAnglePid_Fan, ROTATE_POSI_p, ROTATE_POSI_i, ROTATE_POSI_d, -ROTATE_POSI_limit, ROTATE_POSI_limit,0);
	SetPIDCR(&MotorAnglePid_Fan,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	ROTATE_FLAG_REG.BaoMove = 0;
	ROTATE_FLAG_REG.BaoDirect = 0;
	ROTATE_FLAG_REG.FanMoving_F = 0;
	ROTATE_FLAG_REG.FanCurrPosi_D = 0;
	//电机停止运动
	Rotate_Stop();
}
/***************************************************************************************
 *Name     : 
 *Function ：
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
const float Rotate_Posi_Fan[2] = {3072.f,0.f};

void Rotate_Loop(void)
{	
	switch(GlobalMode){
		case MODE_LIFT:
			#ifdef RC_TEST
			if(RemoteSEF_REG&RemoteSEF_S1_UP){			//s1产生上拨动作
				Rotate_Bao_Toggle();
			}
			#endif
			break;
		default:
			ClearFlag(ROTATE_FLAG_REG.BaoMove);
	}
/*-------------------------- 后处理 -----------------------------*/
	if(ROTATE_FLAG_REG.FanMoving_F){
		//检测是否到达目标
		if(Rotate_CheckGoal(Rotate_SetAngel_Fan,ROTATE_CURR_ANGLE_Fan)){
			ClearFlag(ROTATE_FLAG_REG.FanMoving_F);
		}
	}
	Rotate_SetAngel_Fan = Rotate_Posi_Fan[ROTATE_FLAG_REG.FanCurrPosi_D];
	Rotate_LastAngel_Fan = ROTATE_CURR_ANGLE_Fan;
	Rotate_Move();
}
/***************************************************************************************
 抱柱电机运动处理
****************************************************************************************/
void Rotate_Bao_Start(void)
{
	//开始旋转
	ROTATE_FLAG_REG.BaoMove = 1;
}
void Rotate_Bao_Stop(void)
{
	//结束旋转
	ROTATE_FLAG_REG.BaoMove = 0;
}
void Rotate_Bao_Toggle(void)
{
	//切换旋转
	ROTATE_FLAG_REG.BaoMove = ~ROTATE_FLAG_REG.BaoMove;
}
void Rotate_Bao_DirePos(void)
{
	//正转
	ROTATE_FLAG_REG.BaoDirect = 0;
}
void Rotate_Bao_DireRev(void)
{
	//反转
	ROTATE_FLAG_REG.BaoDirect = 1;
}
void Rotate_Bao_DireToggle(void)
{
	//切换方向
	ROTATE_FLAG_REG.BaoDirect = ~ROTATE_FLAG_REG.BaoDirect;
}
/***************************************************************************************
 翻箱电机运动处理
****************************************************************************************/
void Rotate_Fan(void)
{
	//切换位置
	ROTATE_FLAG_REG.FanCurrPosi_D = (ROTATE_FLAG_REG.FanCurrPosi_D+1)%2;
}

/***************************************************************************************
 *Name     : 
 *Function ：
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
void Rotate_Stop(void)
{
	CAN1_Rotate(0,0);
}
/***************************************************************************************
 *Name     : 
 *Function ：位置环+速度环控制
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
#define DELTA_MAX_FILTER	5000.f				//限制最大角度增量
#define DELTA_MIN_FILTER	10.f				//限制最小角度增量
#define POSI_OUT_LIMIT		10000.f				//位置环输出过滤(比速度上限大一些)
#define ROTATE_BAO_POSI		1
void Rotate_Move(void)
{
	float delta;
	//翻箱电机
	delta = (Rotate_SetAngel_Fan - ROTATE_CURR_ANGLE_Fan);
	if((delta>DELTA_MIN_FILTER)||(delta<-DELTA_MIN_FILTER)){
		delta = (delta>DELTA_MAX_FILTER)? DELTA_MAX_FILTER:((delta<-DELTA_MAX_FILTER)? -DELTA_MAX_FILTER:delta);
		Rotate_SpeedTrans_Fan = PID_Driver(ROTATE_CURR_ANGLE_Fan+delta, ROTATE_CURR_ANGLE_Fan, &MotorAnglePid_Fan, 2000);
	}else{
		Rotate_SpeedTrans_Fan = 0.f;
	}
	//位置环输出限制
	Rotate_SpeedTrans_Bao = (ROTATE_FLAG_REG.BaoMove)?((ROTATE_FLAG_REG.BaoDirect)?-1200:1200):0;
	Rotate_SpeedTrans_Fan = Rotate_Legalize(Rotate_SpeedTrans_Fan,POSI_OUT_LIMIT);
	
	//输出速度
	Rotate_CalcSpeed(Rotate_SpeedTrans_Bao,Rotate_SpeedTrans_Fan);
}
/***************************************************************************************
 *Name     : 
 *Function ：速度环控制
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
#define ROTATE_SPEEDLIMIT		3000.0f	//速度上限
#define ROTATE_FILTER			20.0f	//电机速度过滤
void Rotate_CalcSpeed(float speed_Bao, float speed_Fan)
{	
	//抱柱电机
	if((speed_Bao>ROTATE_FILTER)||(speed_Bao<-ROTATE_FILTER)){
		Rotate_SpeedTrans_Bao = PID_Driver(speed_Bao, ROTATE_CURR_SPEED_Bao, &MotorSpeedPid_Bao, 1000);
	} else {
		Rotate_SpeedTrans_Bao = 0;
	}
	//翻箱电机
	if((speed_Fan>ROTATE_FILTER)||(speed_Fan<-ROTATE_FILTER)){
		Rotate_SpeedTrans_Fan = PID_Driver(speed_Fan, ROTATE_CURR_SPEED_Fan, &MotorSpeedPid_Fan, 1000);
	} else {
		Rotate_SpeedTrans_Fan = 0;
	}
	//幅值限制
	Rotate_SpeedTrans_Bao = Rotate_Legalize(Rotate_SpeedTrans_Bao,ROTATE_SPEEDLIMIT);
	Rotate_SpeedTrans_Fan = Rotate_Legalize(Rotate_SpeedTrans_Fan,ROTATE_SPEEDLIMIT);

	CAN1_Rotate(Rotate_SpeedTrans_Bao, Rotate_SpeedTrans_Fan);
}



/***************************************************************************************
 *Name     : 
 *Function ：检测是否达到目标值
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
#define ROTATE_GETGOAL_FILTER		50.0f
uint8_t Rotate_CheckGoal(float goal, float angles)	
{
	return fabs(goal-angles)<ROTATE_GETGOAL_FILTER;
}
/***************************************************************************************
 *Name     : 
 *Function ：幅值限制
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
float Rotate_Legalize(float MotorCurrent, float limit)
{
	return (MotorCurrent<-limit)?-limit:((MotorCurrent>limit)?limit:MotorCurrent);
}

