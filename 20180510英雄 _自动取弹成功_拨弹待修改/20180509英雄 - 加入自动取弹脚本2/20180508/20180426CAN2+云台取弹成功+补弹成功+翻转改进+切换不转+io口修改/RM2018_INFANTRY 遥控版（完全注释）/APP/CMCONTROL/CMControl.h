#ifndef _CMCONTROL_H_
#define _CMCONTROL_H_

/*
*********************************************************************************************************
*                                           PID��������
*********************************************************************************************************
*/
#include "main.h"

#define PIDVAL_CM_SPEED_p 2.7
#define PIDVAL_CM_SPEED_i 6.2
#define PIDVAL_CM_SPEED_d 0
#define PIDVAL_CM_SPEED_limit 200

#define PIDVAL_CM_FOLLOW_p 0.8//0.5
#define PIDVAL_CM_FOLLOW_i 0.3
#define PIDVAL_CM_FOLLOW_d 0.07//0
#define PIDVAL_CM_FOLLOW_limit 200

#define XLIMIT_OFFSET 400
#define YLIMIT_OFFSET 400

#define gears_speedXYZ 	18   //����ƽ���ƶ�ϵ�����ɵ�������X��Y�����ٶ�
#define gears_speedRAD 	15   //������ת�˶�ϵ�����ɵ���������ת�ٶ�
#define gears_speedRAD_QE 45
#define followVal_limit 30   //���̸���Ƕ���С��Χ���ɵ������̲����淶Χ

extern int quick_spin_step;
extern int quick_spin_flag;
void CMControlInit(void);
void CMPID_Init(void);
void CMControlOut(int16_t speedA , int16_t speedB ,int16_t speedC ,int16_t speedD );
float CMSpeedLegalize(float MotorCurrent , float limit);
void move(int16_t speedX, int16_t speedY, int16_t rad);
void CMControlLoop(void);
void CMStop(void);
int16_t followValCal(float Setposition);
void keyboardmove_old(uint16_t keyboardvalue , int16_t rad);
#endif
