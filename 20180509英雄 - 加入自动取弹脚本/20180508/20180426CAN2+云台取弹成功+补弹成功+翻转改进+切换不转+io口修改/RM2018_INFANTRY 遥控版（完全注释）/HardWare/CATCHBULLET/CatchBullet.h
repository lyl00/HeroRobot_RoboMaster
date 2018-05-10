#ifndef _CATCHBULLET_H_
#define _CATCHBULLET_H_

#include "rc.h"
#include "main.h"

#define Cycle_rotate 440//翻转周期
#define Rotate_POS_p 25
#define Rotate_POS_i 1
#define Rotate_POS_d 0.1
#define Rotate_POS_limit 100
#define Rotate_SPEED_p 10
#define Rotate_SPEED_i 1
#define Rotate_SPEED_d 0.1
#define Rotate_SPEED_limit 100
#define Lift_SPEED_p 5
#define Lift_SPEED_i 0
#define Lift_SPEED_d 1
#define Lift_SPEED_limit 100
#define Lift_SPEED_Max 8000
#define Lift_POS_p 50
#define Lift_POS_i 5
#define Lift_POS_d 1
#define Lift_POS_limit 120

#define LIFT_POSLIMIT		8192*3.f	//电机极限位置（单向）
#define FLIFT_POSLIMIT_H		11500.f  //预提升高度
#define FLIFT_POSLIMIT_L		0//其他档位下夹子高度
#define FLIFT_LIFTSPEED 100//预提升过程中夹子抬升速度
#define LIFT_FILTER 10 //低角度过滤
#define LIFT_SCALE 1.0f

extern int RotateFlag;      
extern float liftspeed1;
extern float liftspeed2;
//extern u8 RotateFlag2;
extern float setangle1;
extern float setangle2;
	
void catchbullet(int16_t liftspeed);
void moveforward(void);
void movebackward(void);
void catchstart(void);
void catchfinish(void);
void catchtest(void);
void moveinit(void);
void catchinit(void);
void liftinit(void);
void RotateSpeedcal(void);
void LiftPosSpeedcal(void);
void LiftSpeedcal(float speed1,float speed2);
float Lift_Legalize(float MotorCurrent , float limit);
void flift(void);
void fliftdown(void);

void AutoCatch_CatchBullet(int16_t liftspeed);  //catchbullet()的改版
#endif
