/************************************************************
 *File		:	led.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com	@Jason ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	RM2018_INFANTRY project. 
		            Led GPIO Init.
								PF14 -> LED_red
								PE7  -> LED_green
 *Input   : None
 *Output  : None
 ************************************************************/

#include "main.h"

void Led_Configuration(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF , ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化

	LED_G_OFF();
	LED_R_ON();
}

