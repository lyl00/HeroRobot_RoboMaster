/************************************************************
 *File		:	
 *Author	:  	@YangTao, ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.3.3
 *Description: 	Duct contorl
				TIM8_CH1(PI5)
 ************************************************************/

#include "main.h"

void AutoMove_INNER(AutoMoveEvent* event);
///************************************************************
// 测试用LED程序
//************************************************************/
//void AutoMove_LED_Init(void);
//void AutoMove_LED(void);
//void AutoMove_LED_End(void);
/************************************************************
 自动取弹控制
************************************************************/
void AutoMove_CatchBullet_Init(void);
void AutoMove_CatchBullet_End(void);
void AutoMove_CatchBullet_MoveUp(void);
void AutoMove_CatchBullet_MoveDown(void);
/***************************************************************************************
* 抱柱开关
****************************************************************************************/
void AutoMove_BAO_Init(void);
void AutoMove_BAO_End(void);
void AutoMove_BAO_MoveDown(void);
void AutoMove_BAO_MoveUp(void);
///***************************************************************************************
//* 视角控制
//****************************************************************************************/
//void AutoMove_Camera_Init(void);
/***********************************************************/



//注册自动事件表
AutoMoveEvent 	LedEvent, CatchBulletEvent, BaoEvent;//, CameraEvent;
/***************************************************************************************
* 自动控制主程序
****************************************************************************************/
void AutoMove_Configuration(void)
{
//	AutoMove_LED_Init();
	AutoMove_CatchBullet_Init();
	AutoMove_BAO_Init();
//	AutoMove_Camera_Init();
}
void AutoMove_Loop(uint32_t tick)
{
	#ifdef RC_FUNC
	//开始取弹事件
	if(GlobalMode==MODE_BULLET){
		if(RemoteSEF_REG&RemoteSEF_S1_UP) AutoMove_Start(&CatchBulletEvent);
	}
	else if(GlobalMode==MODE_LIFT){
		if(RemoteSEF_REG&RemoteSEF_S1_UP){
			Rotate_Bao_DirePos();		//正转
			AutoMove_Start(&BaoEvent);
		}
		if(RemoteSEF_REG&RemoteSEF_S1_DOWN){
			Rotate_Bao_DireRev();		//反转
			AutoMove_Start(&BaoEvent);
		}
	}
	#else
	if(GlobalMode==MODE_BULLET){
		if(Remote_KEYVal&KEY_PRESSED_OFFSET_Q) AutoMove_Start(&CatchBulletEvent);
	}
	else if(GlobalMode==MODE_LIFT){
		if(Remote_KEYVal&KEY_PRESSED_OFFSET_Q){
			Rotate_Bao_DirePos();		//正转
			AutoMove_Start(&BaoEvent);
		}
		if(Remote_KEYVal&KEY_PRESSED_OFFSET_E){
			Rotate_Bao_DireRev();		//反转
			AutoMove_Start(&BaoEvent);
		}
	}
	#endif
//	AutoMove_INNER(&LedEvent);
	AutoMove_INNER(&CatchBulletEvent);
	AutoMove_INNER(&BaoEvent);
//	AutoMove_INNER(&CameraEvent);
}
void AutoMove_Start(AutoMoveEvent* event)
{
	if(!event->enable){
		event->enable = 1;
		event->currID = 0;
	}
}
void AutoMove_INNER(AutoMoveEvent* event)
{
	static pAutoMoveUnit pNode;
	if(event->enable){
		pNode = event->entry + event->currID;
		if(pNode->cnt==0){
			if(pNode->func) (*pNode->func)();		//调用程序
		}
		if(pNode->cnt==pNode->time){
			pNode->cnt = 0;
			//事件结束
			if(event->currID == event->length - 1){
				event->enable = 0;
				event->currID = 0;
				if(event->end) (*(event->end))();	//调用程序
			}else{
				event->currID ++;
			}
		}else{
			pNode->cnt ++;
		}
	}
}

/***************************************************************************************
* 自动取弹控制
****************************************************************************************/
#ifndef RC_TEST
#define 		CATCHBULLET_LEN		10
//初始化函数数组
AutoMoveUnit	CatchBulletUnits[CATCHBULLET_LEN] = { \
			{0,500,0,Cylinder_Stretch_Open},	{1,100,0,Cylinder_Catch_Open},	{2,800,0,AutoMove_CatchBullet_MoveUp}, \
			{3,500,0,Cylinder_Stretch_Close}, 	{4,1000,0,Rotate_Fan},			{5,200,0,Rotate_Fan}, \
			{6,800,0,Cylinder_Stretch_Open},	{7,100,0,Cylinder_Catch_Close},	{8,400,0,Cylinder_Stretch_Close}, \
			{9,800,0,AutoMove_CatchBullet_MoveDown}};
/*流程:	4.7s
		伸出
		----------抓住
		------------拉高
		----------------------------缩回
		--------------------------------------翻转
		------------------------------------------------翻转
		----------------------------------------------------伸出
		--------------------------------------------------------------------放开
		----------------------------------------------------------------------缩回
		------------------------------------------------------------------------------下降
		---------------------------------------------------------------------------------------------
*/
void AutoMove_CatchBullet_Init(void)
{
	//初始化时间列表
	CatchBulletEvent.length 	= CATCHBULLET_LEN;
	CatchBulletEvent.currID 	= 0;
	CatchBulletEvent.enable 	= 0;
	CatchBulletEvent.entry 		= CatchBulletUnits;
	CatchBulletEvent.end		= NULL;
}
#else
void AutoMove_CatchBullet_Init(void){}
#endif
void AutoMove_CatchBullet_MoveUp(void)
{
	uint8_t data = 1;
	CAN1_SendMessage(1,&data);
}
void AutoMove_CatchBullet_MoveDown(void)
{
	uint8_t data = 2;
	CAN1_SendMessage(1,&data);
}
/***************************************************************************************
* 抱柱开关
****************************************************************************************/
#define 		BAO_LEN			6
AutoMoveUnit	BaoUnits[BAO_LEN] = {{0,200,0,AutoMove_BAO_MoveDown},{0,200,0,Cylinder_Bao_Open},{1,1000,0,Rotate_Bao_Start}, \
									{2,200,0,Cylinder_Bao_Close},{3,500,0,Rotate_Bao_Stop},{0,200,0,AutoMove_BAO_MoveUp}};	//初始化函数数组
//流程：打开气缸，打开电机，关闭气缸，关闭电机，（结束：反向电机转向）
void AutoMove_BAO_Init(void)
{
	//初始化时间列表
	BaoEvent.length 	= BAO_LEN;
	BaoEvent.currID 	= 0;
	BaoEvent.enable 	= 0;
	BaoEvent.entry 		= BaoUnits;
//	BaoEvent.end		= AutoMove_BAO_End;
	BaoEvent.end		= NULL;
}
void AutoMove_BAO_MoveDown(void)
{
	uint8_t data = 0x10;
	CAN1_SendMessage(1,&data);
}
void AutoMove_BAO_MoveUp(void)
{
	uint8_t data = 0x20;
	CAN1_SendMessage(1,&data);
}
///***************************************************************************************
//* 视角控制
//****************************************************************************************/
//#define 		CAMERA_LEN			3
//AutoMoveUnit	CameraUnits[CAMERA_LEN] = {{0,400,0,Camera_Pitch_Back},{1,400,0,Camera_Yaw},{2,100,0,Camera_Pitch}};	//初始化函数数组
////流程：打开气缸，打开电机，关闭气缸，关闭电机，（结束：反向电机转向）
//void AutoMove_Camera_Init(void)
//{
//	//初始化时间列表
//	BaoEvent.length 	= CAMERA_LEN;
//	BaoEvent.currID 	= 0;
//	BaoEvent.enable 	= 0;
//	BaoEvent.entry 		= CameraUnits;
//	BaoEvent.end		= NULL;
//}






///***************************************************************************************
//* 测试用LED程序
//****************************************************************************************/
//#define 		LED_LEN			4
//AutoMoveUnit	LedUnits[LED_LEN] = {{0,100,0,AutoMove_LED},{1,200,0,AutoMove_LED}, \
//									{2,300,0,AutoMove_LED},{3,400,0,AutoMove_LED}};	//初始化函数数组
//void AutoMove_LED_Init(void)
//{
//	//初始化时间列表
//	LedEvent.length 	= LED_LEN;
//	LedEvent.currID 	= 0;
//	LedEvent.enable 	= 1;
//	LedEvent.entry 		= LedUnits;
//	LedEvent.end		= AutoMove_LED_End;
//}
//void AutoMove_LED_End(void)
//{
//	AutoMove_Start(&LedEvent);
//}
//void AutoMove_LED(void)
//{
//	LED_G_TOGGLE();
//	LED_R_TOGGLE();
//}


