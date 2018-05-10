/************************************************************
 *File		:	buzzer.c
 *Author	:	@YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Buzzer
 ************************************************************/

#include "main.h"

//Buzzer_Init
//功能：Buzzer定义初始化函数
//参数：PB4 TIM3_CH1    PWM启动蜂鸣器
//返回值：void
void Buzzer_Init(void)     //PB4
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM4时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTB时钟	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_TIM3); 	//GPIOH9复用为定时器12
	
	/* TIM3 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           //GPIOB 6 7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure);              	//初始化
	  
	TIM_TimeBaseStructure.TIM_Period=2000;   	//自动重装载值	
	TIM_TimeBaseStructure.TIM_Prescaler=72-1;  	//定时器分频     					///******这里改过
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);		//允许定时器12更新中断
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);	//初始化定时器12
	
	//初始化TIM3 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ; 		//输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  						//根据T指定的参数初始化外设TIM1 4OC1

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  	//使能TIM4在CCR1上的预装载寄存器
	TIM_ARRPreloadConfig(TIM3,ENABLE);					//ARPE使能 
	TIM_Cmd(TIM3, ENABLE);  							//使能TIM14
	BUZZER_OFF();
}

//Buzzer_toggle
//功能：蜂鸣器鸣叫1下
//参数：无 
//返回值：void
void Buzzer_toggle(void)
{
	BUZZER_ON(1500);
	delay_ms(300);
	BUZZER_OFF();
	delay_ms(300);
}
