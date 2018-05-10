/************************************************************
 *File		:	laser.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com	@YangTianhao ,490999282@qq.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Laser output  
 ************************************************************/

#include "main.h" 

void Laser_Configuration(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG , ENABLE);//
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;    //���ڼ���		//���ڿ��Ƽ���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��	
	
	LASER_OFF();   //д�ڶ�����ʼ����
}






