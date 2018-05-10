#ifndef _CMCONTROL_H_
#define _CMCONTROL_H_

/*
*********************************************************************************************************
*                                           PID≤Œ ˝…Ë÷√
*********************************************************************************************************
*/
#include "main.h"

#define PIDVAL_CM_SPEED_p 1.5f
#define PIDVAL_CM_SPEED_i 5.0f
#define PIDVAL_CM_SPEED_d 0.f
#define PIDVAL_CM_SPEED_limit 200.f

#define PIDVAL_CM_POSI_p 0.2f
#define PIDVAL_CM_POSI_i 0.0f
#define PIDVAL_CM_POSI_d 0.0f
#define PIDVAL_CM_POSI_limit 200.f

//#define PIDVAL_CM_FOLLOW_p 0.5
//#define PIDVAL_CM_FOLLOW_i 0.3
//#define PIDVAL_CM_FOLLOW_d 0.00
//#define PIDVAL_CM_FOLLOW_limit 200

typedef struct {
	uint8_t Locked:1;
	uint8_t SaveGoal:1;
}CM_FLAG_REG;

extern CM_FLAG_REG CM_FLAG;


void CMControlInit(void);
void CMControlLoop(void);
//void CMPID_Init(void);
//void CMControlOut(int16_t speedA , int16_t speedB ,int16_t speedC ,int16_t speedD );
//float CMSpeedLegalize(float MotorCurrent , float limit);
//void move(int16_t speedX, int16_t speedY, int16_t rad);
//void CMStop(void);
//int16_t followValCal(float Setposition);

#endif
