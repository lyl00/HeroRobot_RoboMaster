#ifndef _SHOOTCONTROL_H_
#define _SHOOTCONTROL_H_

/*
*********************************************************************************************************
*                                           PERIPH BUF SIZES 
*********************************************************************************************************
*/
#include "main.h"


#define PIDVAL_PLATE_POS_p 40.0//参照yaw轴进行设置，原值为0
#define PIDVAL_PLATE_POS_i 0.2//参照yaw轴进行设置，原值为0
#define PIDVAL_PLATE_POS_d 0
#define PIDVAL_PLATE_SPEED_p 5//70//8
#define PIDVAL_PLATE_SPEED_i 0//2    
#define PIDVAL_PLATE_SPEED_d 0//0.9
#define Shoot_UP 280       //摩擦轮开  
#define Shoot_DOWN 100     //摩擦轮关	
#define ShootInterval 30
#define ShootTime 20//23       //拨盘电机转动时间



extern float PIDOut_Position_Plate,PIDOut_Speed_Plate,PIDOut_Whole_Shoot;
typedef struct
{
	float angle_plate_set;
	float angle_plate_current;
}Plate_Control_t;


void Read_Infrared_State(void);
void ShootControlLoop(void);
void FrictionJudge(void);
void FrictionControl(void);
void GunControl(void);
void GunShootAction(void)	;
void PltePidInit(void);
void ShootStop(void)	;
float PlateMotorCurrentLegalize(float MotorCurrent , float limit);
void PlatePID(float SetPosition);
void ShootControlInit(void);
void Bullet_Load_Judge(void);
void Set_Target(void);
void Empty_Detection(void);
void blocking_judge(void);
#endif

