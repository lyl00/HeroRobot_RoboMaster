/************************************************************
 *File		:	RemoteControl.c
  *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Remote data analysis
                Change remoteState (state of the system);
 ************************************************************/

#include "main.h"

RemoteState_e remoteState = PREPARE_STATE;
RemoteState_e remoteState_ex = PREPARE_STATE;

/*-------------  遥控器循环  -------------*/
void RemoteControlLoop(void)
{
	ItoE();												//更新Ex值，并用Ex值来进行操作
	RemoteStateFSM();             //判断此时遥控器状态
	GimbalSwitchJudge();         //判断遥控器模式切换
}

/*-------------  遥控器模式状态机  -------------*/
void RemoteStateFSM(void)    
{
	if(flag_Ready)
	{
		switch(RC_Ex_Ctl.rc.s2)       //判断s2摇杆值，改变状态
		{
			case 0:   remoteState = ERROR_STATE;						//出错模式
				break;
			case 1:		remoteState = NORMAL_REMOTE_STATE;  	//遥控器控制模式
				break;	
			case 2:		remoteState = STANDBY_STATE;  				//待机模式
				break;
			case 3:		remoteState = KEY_REMOTE_STATE;				//键鼠模式
				break;
			default: 	remoteState = PREPARE_STATE;          //初始化准备模式
				break;
		}
	}
}
 
/***************************************************************************************
 *Name     : GimbalSwitchJudge
 *Function ：判断机器模式切换
 *Input    ：无
 *Output   ：无 
 *Description : 判断机器是否是从待机模式或出错模式切换成正常模式，若是，则软起动
****************************************************************************************/
void GimbalSwitchJudge(void)
{
	if((remoteState_ex == STANDBY_STATE || remoteState_ex == ERROR_STATE) && remoteState == NORMAL_REMOTE_STATE)
	{
		remoteState = PREPARE_STATE;    //使机器进入初始化状态，云台处于初始化模式，软启动
		flag_Ready = 0;                 //flag_Ready置零，保证切换至正常模式时云台软启动
		GimbalPidInitPrepare();         //云台初始化pid，以较慢速度归中
		remoteState_ex = NORMAL_REMOTE_STATE;  
	}
}

/*-------------  遥控器初始化  -------------*/
void RemoteControlInit(void)
{  
	ItoE();
	remoteState = PREPARE_STATE;    //开机2s前不接收遥控器信号，这里程序自动赋值为PREPARE_STATE模式
}

