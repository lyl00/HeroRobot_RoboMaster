/************************************************************
 *File		:	cylinder.c
 *Author	:  	@YangTao, ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.3.6
 *Description: 	Cylinder Modules（with Relay Mudules）
				PC0 PC1 PC4 PC5
 ************************************************************/

#include "main.h"

/*-------------  气缸（继电器）驱动初始  -------------*/
void Cylinder_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;						//GPIO初始化结构体
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);		//GPIO时钟使能
	//GPIO初始化
	GPIO_InitStructure.GPIO_Pin    |= 	GPIO_Pin_0|GPIO_Pin_1;	//PC0,PC1
	GPIO_InitStructure.GPIO_Pin    |= 	GPIO_Pin_4|GPIO_Pin_5;	//PC4,PC5
	GPIO_InitStructure.GPIO_Mode 	= 	GPIO_Mode_OUT;			//输入模式
	GPIO_InitStructure.GPIO_Speed 	= 	GPIO_Speed_100MHz;		//100MHs频率
	GPIO_InitStructure.GPIO_PuPd 	= 	GPIO_PuPd_DOWN;			//推挽
	GPIO_InitStructure.GPIO_OType 	= 	GPIO_OType_PP;			//下拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);						//初始化
	//气缸复位
	Cylinder1_Off;Cylinder2_Off;
	Cylinder3_Off;Cylinder4_Off;
}


/*-------------  气缸控制循环  -------------*/
/***************************************************************************************
 *Name     : Cylinder_Loop
 *Function ：判断遥控器开关动作(控制气缸)
 *Input    ：无
 *Output   ：无 
 *Description : 判断遥控器是否进行过1,3或2,3切换的动作(一次来回为一个动作)
****************************************************************************************/
void Cylinder_Loop(void)
{
	//气缸
	switch(GlobalMode){
	case MODE_DRAG: 
		break;
	case MODE_LIFT:
		#ifdef RC_TEST
		if(RemoteSEF_REG&RemoteSEF_S1_UP) Cylinder_Bao_Toggle();
		#endif
		break;
	case MODE_BULLET:
		#ifdef RC_TEST
		if(RemoteSEF_REG&RemoteSEF_S1_UP){
			Cylinder_Catch_Toggle();
			Cylinder_Stretch_Toggle();
		}
		#endif
		break;
	default:
		//控制异常，气缸复位
		Cylinder_Bao_Close();
		Cylinder_Catch_Close();
		Cylinder_Stretch_Close();
		break;
	}
}
///***************************************************************************************
// 拖车气缸运动处理
//****************************************************************************************/
//void Cylinder_Drag_Open(void)
//{
//	Cylinder1_On;
//}
//void Cylinder_Drag_Close(void)
//{
//	Cylinder1_Off;
//}
//void Cylinder_Drag_Toggle(void)
//{
//	Cylinder1_Toggle;
//}
/***************************************************************************************
 抱柱气缸运动处理
****************************************************************************************/
void Cylinder_Bao_Open(void)
{
	Cylinder2_On;
}
void Cylinder_Bao_Close(void)
{
	Cylinder2_Off;
}
void Cylinder_Bao_Toggle(void)
{
	Cylinder2_Toggle;
}
/***************************************************************************************
 抓住气缸运动处理
****************************************************************************************/
void Cylinder_Catch_Open(void)
{
	Cylinder3_On;
}
void Cylinder_Catch_Close(void)
{
	Cylinder3_Off;
}
void Cylinder_Catch_Toggle(void)
{
	Cylinder3_Toggle;
}
/***************************************************************************************
 伸出气缸运动处理
****************************************************************************************/
void Cylinder_Stretch_Open(void)
{
	Cylinder4_On;
}
void Cylinder_Stretch_Close(void)
{
	Cylinder4_Off;
}
void Cylinder_Stretch_Toggle(void)
{
	Cylinder4_Toggle;
}


