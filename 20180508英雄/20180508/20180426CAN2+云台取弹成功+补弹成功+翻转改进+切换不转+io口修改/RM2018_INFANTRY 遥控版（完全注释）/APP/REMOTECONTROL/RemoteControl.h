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
    STANDBY_STATE,			//云台停止不转状态
		PREPARE_STATE,      //上电后初始化状态 4s钟左右
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
