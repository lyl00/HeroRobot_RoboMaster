#ifndef _REMOTECONTROL_H_
#define _REMOTECONTROL_H_

/*
*********************************************************************************************************
*                                           PERIPH BUF SIZES 
*********************************************************************************************************
*/
#include "sys.h"
#include "stm32f4xx.h"

#define RC_CH_VALUE_MIN ((uint16_t)364 )
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)

/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP ((uint16_t)1)
#define RC_SW_MID ((uint16_t)3)
#define RC_SW_DOWN ((uint16_t)2)

/* ----------------------- PC Key Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W ((uint16_t)0x01<<0)
#define KEY_PRESSED_OFFSET_S ((uint16_t)0x01<<1)
#define KEY_PRESSED_OFFSET_A ((uint16_t)0x01<<2)
#define KEY_PRESSED_OFFSET_D ((uint16_t)0x01<<3)
#define KEY_PRESSED_OFFSET_Q ((uint16_t)0x01<<6)
#define KEY_PRESSED_OFFSET_E ((uint16_t)0x01<<7)
#define KEY_PRESSED_OFFSET_SHIFT ((uint16_t)0x01<<4)
#define KEY_PRESSED_OFFSET_CTRL ((uint16_t)0x01<<5)

// 三档开关事件标志(Switch Enable Flag)
#define RemoteSEF_S1_UP			0x01
#define RemoteSEF_S1_MID		0x02
#define RemoteSEF_S1_DOWN		0x04
#define RemoteSEF_S2_UP			0x08
#define RemoteSEF_S2_MID		0x10
#define RemoteSEF_S2_DOWN		0x20

// 三档开关事件标志(Switch Disable Flag)
#define RemoteSDF_S1_UP			0x01
#define RemoteSDF_S1_MID		0x02
#define RemoteSDF_S1_DOWN		0x04
#define RemoteSDF_S2_UP			0x08
#define RemoteSDF_S2_MID		0x10
#define RemoteSDF_S2_DOWN		0x20

extern uint8_t Remote_FromBottomData[8];
extern u8 RemoteSEF_REG;
extern u8 RemoteSDF_REG;
extern uint8_t Remote_KEYVal;


//void RemoteControlInit(void);
void RemoteControlLoop(void);


//void RemoteStateFSM(void);
//void RemoteSwitchHandler(void);

//void StateJugg(void);
//void StateChangeToPrepare(void);
//void StateChangeToRemote(void);
//void StateChangeToKey(void);
//void GimbalSwitchJudge(void);


#endif
