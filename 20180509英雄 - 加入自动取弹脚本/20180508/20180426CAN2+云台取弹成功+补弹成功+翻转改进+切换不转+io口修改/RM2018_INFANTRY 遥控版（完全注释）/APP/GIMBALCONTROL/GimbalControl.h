#ifndef _GIMBALCONTROL_H_
#define _GIMBALCONTROL_H_

#include "main.h"

/*
*********************************************************************************************************
*                                         PID参数设置
*********************************************************************************************************
*/


#define PIDVAL_YAW_POS_p_Prepare				12.0
#define PIDVAL_YAW_POS_i_Prepare				0.0//10.0
#define PIDVAL_YAW_POS_d_Prepare				0.0
#define PIDVAL_YAW_SPEED_p_Prepare			16.0
#define PIDVAL_YAW_SPEED_i_Prepare			0.0//3.0
#define PIDVAL_YAW_SPEED_d_Prepare			0.1


#define PIDVAL_PITCH_POS_p_Prepare			5.5
#define PIDVAL_PITCH_POS_i_Prepare			0//2
#define PIDVAL_PITCH_POS_d_Prepare			0.0
#define PIDVAL_PITCH_SPEED_p_Prepare		14.0
#define PIDVAL_PITCH_SPEED_i_Prepare		0.0//2.0
#define PIDVAL_PITCH_SPEED_d_Prepare		0.0


#define PIDVAL_YAW_POS_p				25.0//15.0//25.0
#define PIDVAL_YAW_POS_i				10.0//20.0
#define PIDVAL_YAW_POS_d				4//4
#define PIDVAL_YAW_SPEED_p			20//60.0//45.0
#define PIDVAL_YAW_SPEED_i			2.0//2.0
#define PIDVAL_YAW_SPEED_d			0                       //


#define PIDVAL_PITCH_POS_p			28.0//28.0                  //pitch轴平稳 还可以再提高P以提高相响应
#define PIDVAL_PITCH_POS_i			10.0//10
#define PIDVAL_PITCH_POS_d			0.5
#define PIDVAL_PITCH_SPEED_p		20.0//20.0
#define PIDVAL_PITCH_SPEED_i		2.0//2.0
#define PIDVAL_PITCH_SPEED_d		0.0
typedef struct 
{
	PID_Struct Position;
	PID_Struct Speed;
}MotorPID_t;

extern MotorPID_t	MotorPID_pitch;	
extern MotorPID_t	MotorPID_yaw;
extern float PIDOut_Position_Pit,PIDOut_Speed_Pit,PIDOut_Whole_Pit;					//Pit,Yaw PID总输出
extern float PIDOut_Position_Yaw,PIDOut_Speed_Yaw,PIDOut_Whole_Yaw;
extern int16_t position_yaw_relative ,current_position_pitch;

/**********************************************************************************************************
																					云台电机参数
**********************************************************************************************************/

/*************0号车************/
//#define MIDDLE_YAW 	3365
//#define MIDDLE_PITCH  6380

/*************英雄云台************/
#define MIDDLE_YAW 	6940
#define MIDDLE_PITCH  1110                      //读出的英雄云台中值
//5000 7300


#define YawMax 700		
#define YawMin -700					
#define PitMax 30
#define PitMin -10 		

//#define ROLL_BORDER			//主控板竖放
#define PITCH_BORDER			//主控板横放

typedef struct 
{
	float angle_yaw_set;
	float angle_yaw_current;
	float angle_pitch_set;
	float angle_pitch_current;
}Gimbal_Control_t;

extern Gimbal_Control_t Gimbal_control;
float MotorCurrentLegalize(float MotorCurrent , float limit);
void GimbalControlInit(void);
void GimbalControlLoop(void);
void GimbalMove(void);
void GimbalStop(void);
void GimbalControlValSet(void);
void GimbalPidInit(void);
void GimbalPidInitPrepare(void);
void GimbalControl_Yaw(float SetPosition);
void GimbalControl_YawAngel(float SetPosition);
void GimbalControl_PitchAngel(float SetPosition);
void GimbalControl_Pitch(float SetPosition);
float MotorPositionLegalize(float PositionPara , float MAX , float MIN);
int16_t GimbalValLigal(int raw_gimbal_data,int middle_data);
void ViewModeChange(uint16_t keyboardvalue);
void HandShoot(uint16_t keyboardvalue);



#endif




