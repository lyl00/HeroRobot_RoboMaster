/************************************************************
 *File		:	RemoteControl.c
  *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Remote data analysis
                Change remoteState (state of the system);
 ************************************************************/

#include "main.h"

//RemoteState_e remoteState = PREPARE_STATE;
uint8_t Remote_FromBottomData[8] = {0};			//来自底层主控的控制信息
uint8_t Remote_KEYVal = 0;						//解析自底层主控的键位信息
u8 RemoteSEF_REG = 0;
u8 RemoteSDF_REG = 0;
/*------------------------------*/

/*-------------  遥控器循环  -------------*/
void RemoteControlLoop(void)
{
	GlobalMode = (Mode_WorkingMode)Remote_FromBottomData[0];
	RemoteSEF_REG = Remote_FromBottomData[1];
	RemoteSDF_REG = Remote_FromBottomData[2];
	Remote_KEYVal = Remote_FromBottomData[3];
}


