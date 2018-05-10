#ifndef _REMOTECONTROL_H_
#define _REMOTECONTROL_H_

/*
*********************************************************************************************************
*                                           PERIPH BUF SIZES 
*********************************************************************************************************
*/
#include "sys.h"
#include "stm32f4xx.h"

typedef enum
{
    ERROR_STATE=0,     	
		NORMAL_REMOTE_STATE,
		KEY_REMOTE_STATE,
    STANDBY_STATE,			//��ֹ̨ͣ��ת״̬
		PREPARE_STATE,      //�ϵ���ʼ��״̬ 4s������
	  VIEW_STATE,
}RemoteState_e;

extern RemoteState_e remoteState;
extern RemoteState_e remoteState_ex;

void RemoteControlInit(void);
void RemoteControlLoop(void);
void RemoteStateFSM(void);

void StateJugg(void);
void StateChangeToPrepare(void);
void StateChangeToRemote(void);
void StateChangeToKey(void);
void GimbalSwitchJudge(void);


#endif
