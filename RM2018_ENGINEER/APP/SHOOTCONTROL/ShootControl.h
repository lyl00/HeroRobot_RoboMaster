#ifndef _SHOOTCONTROL_H_
#define _SHOOTCONTROL_H_

/*
*********************************************************************************************************
*                                           PERIPH BUF SIZES 
*********************************************************************************************************
*/
#include "main.h"

#define PIDVAL_SHOOT_POS_p 0
#define PIDVAL_SHOOT_POS_i 0
#define PIDVAL_SHOOT_POS_d 0
#define PIDVAL_SHOOT_SPEED_p 2.5
#define PIDVAL_SHOOT_SPEED_i 2.5    
#define PIDVAL_SHOOT_SPEED_d 0

#define Shoot_UP 150       //摩擦轮开  
#define Shoot_DOWN 100     //摩擦轮关	

#define ShootInterval 30
#define ShootTime 23        //拨盘电机转动时间
#define ShootSpeedOffset 4230  //拨盘电机转速

	
void ShootControlLoop(void);
void FrictionControl(void);
void FrictionJudge(void);
void ShootStop(void);
void GunControl(void);								//拨弹电机操作
void GunShootAction(void);
void ShootControlMove(float SetPosition);      //电机驱动
float ShootSpeedControl(float SetPosition,float NowPosition,PID_Struct* pid);
void ShootControlInit(void);

extern float PIDOut_Speed_Shoot,PIDOut_Whole_Shoot;					//Pit,Yaw PID总输出
extern float shoot_speed_set ;
#endif
