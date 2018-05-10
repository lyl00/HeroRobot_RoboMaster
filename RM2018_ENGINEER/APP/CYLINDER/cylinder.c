/************************************************************
 *File		:	cylinder.c
 *Author	:  	@YangTao, ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.3.6
 *Description: 	Cylinder Modules��with Relay Mudules��
				PC0 PC1 PC4 PC5
 ************************************************************/

#include "main.h"

/*-------------  ���ף��̵�����������ʼ  -------------*/
void Cylinder_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;						//GPIO��ʼ���ṹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);		//GPIOʱ��ʹ��
	//GPIO��ʼ��
	GPIO_InitStructure.GPIO_Pin    |= 	GPIO_Pin_0|GPIO_Pin_1;	//PC0,PC1
	GPIO_InitStructure.GPIO_Pin    |= 	GPIO_Pin_4|GPIO_Pin_5;	//PC4,PC5
	GPIO_InitStructure.GPIO_Mode 	= 	GPIO_Mode_OUT;			//����ģʽ
	GPIO_InitStructure.GPIO_Speed 	= 	GPIO_Speed_100MHz;		//100MHsƵ��
	GPIO_InitStructure.GPIO_PuPd 	= 	GPIO_PuPd_DOWN;			//����
	GPIO_InitStructure.GPIO_OType 	= 	GPIO_OType_PP;			//����
	GPIO_Init(GPIOC, &GPIO_InitStructure);						//��ʼ��
	//���׸�λ
	Cylinder1_Off;Cylinder2_Off;
	Cylinder3_Off;Cylinder4_Off;
}


/*-------------  ���׿���ѭ��  -------------*/
/***************************************************************************************
 *Name     : Cylinder_Loop
 *Function ���ж�ң�������ض���(��������)
 *Input    ����
 *Output   ���� 
 *Description : �ж�ң�����Ƿ���й�1,3��2,3�л��Ķ���(һ������Ϊһ������)
****************************************************************************************/
void Cylinder_Loop(void)
{
	//����
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
		//�����쳣�����׸�λ
		Cylinder_Bao_Close();
		Cylinder_Catch_Close();
		Cylinder_Stretch_Close();
		break;
	}
}
///***************************************************************************************
// �ϳ������˶�����
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
 ���������˶�����
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
 ץס�����˶�����
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
 ��������˶�����
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


