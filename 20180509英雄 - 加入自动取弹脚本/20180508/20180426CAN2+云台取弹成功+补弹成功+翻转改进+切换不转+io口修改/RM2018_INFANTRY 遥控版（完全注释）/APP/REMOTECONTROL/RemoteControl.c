/************************************************************
 *File		:	RemoteControl.c
  *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Remote data analysis
                Change remoteState (state of the system);
 ************************************************************/

#include "main.h"

RemoteState_e remoteState = PREPARE_STATE;
RemoteState_e remoteState_ex = PREPARE_STATE;

/*-------------  ң����ѭ��  -------------*/
void RemoteControlLoop(void)
{
	ItoE();												//����Exֵ������Exֵ�����в���
	RemoteStateFSM();             //�жϴ�ʱң����״̬
	GimbalSwitchJudge();         //�ж�ң����ģʽ�л�
}

/*-------------  ң����ģʽ״̬��  -------------*/
void RemoteStateFSM(void)    
{
	if(flag_Ready)
	{
		switch(RC_Ex_Ctl.rc.s2)       //�ж�s2ҡ��ֵ���ı�״̬
		{
			case 0:   remoteState = ERROR_STATE;						//����ģʽ
				break;
			case 1:		remoteState = NORMAL_REMOTE_STATE;  	//ң��������ģʽ
				break;	
			case 2:		remoteState = STANDBY_STATE;  				//����ģʽ
				break;
			case 3:		remoteState = KEY_REMOTE_STATE;				//����ģʽ
				break;
			default: 	remoteState = PREPARE_STATE;          //��ʼ��׼��ģʽ
				break;
		}
	}
}
 
/***************************************************************************************
 *Name     : GimbalSwitchJudge
 *Function ���жϻ���ģʽ�л�
 *Input    ����
 *Output   ���� 
 *Description : �жϻ����Ƿ��ǴӴ���ģʽ�����ģʽ�л�������ģʽ�����ǣ�������
****************************************************************************************/
void GimbalSwitchJudge(void)
{
	if((remoteState_ex == STANDBY_STATE || remoteState_ex == ERROR_STATE) && remoteState == NORMAL_REMOTE_STATE)
	{
		remoteState = PREPARE_STATE;    //ʹ���������ʼ��״̬����̨���ڳ�ʼ��ģʽ��������
		flag_Ready = 0;                 //flag_Ready���㣬��֤�л�������ģʽʱ��̨������
		GimbalPidInitPrepare();         //��̨��ʼ��pid���Խ����ٶȹ���
		remoteState_ex = NORMAL_REMOTE_STATE;  
	}
}

/*-------------  ң������ʼ��  -------------*/
void RemoteControlInit(void)
{  
	ItoE();
	remoteState = PREPARE_STATE;    //����2sǰ������ң�����źţ���������Զ���ֵΪPREPARE_STATEģʽ
}

