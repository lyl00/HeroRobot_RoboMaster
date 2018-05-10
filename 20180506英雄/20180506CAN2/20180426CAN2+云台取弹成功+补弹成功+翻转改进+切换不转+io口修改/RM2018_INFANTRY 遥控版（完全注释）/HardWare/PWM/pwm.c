#include "main.h"

//配置TIM5的channel 1  对应端口PA0
void TIM8_PWM_Init(u32 arr,u32 psc)
{	
	GPIO_InitTypeDef 					GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  				TIM_OCInitStructure;
//	NVIC_InitTypeDef 					NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 ,ENABLE);  	//TIM4时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE); 	//使能PORTB时钟	
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource5,GPIO_AF_TIM8); //GPIOD13复用为定时器4
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource6,GPIO_AF_TIM8); //GPIOD13复用为定时器4
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource7,GPIO_AF_TIM8); //GPIOD13复用为定时器4
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource2,GPIO_AF_TIM8); //GPIOD13复用为定时器4
	
	/* TIM8 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7 | GPIO_Pin_2 ;           //GPIOB 6 7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOI,&GPIO_InitStructure);              //初始化
	  
	TIM_TimeBaseStructure.TIM_Period=2000;   //自动重装载值	
	TIM_TimeBaseStructure.TIM_Prescaler=840-1;  //定时器分频               ///******这里改过
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	
	
	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE); //允许定时器12更新中断
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseStructure);//初始化定时器12
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ; //输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
	//初始化TIM14 Channel2 PWM模式	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ; //输出极性:TIM输出比较极性低
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
	
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //使能TIM4在CCR1上的预装载寄存器
  TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  //使能TIM4在CCR2上的预装载寄存器
  
	TIM_ARRPreloadConfig(TIM8,ENABLE);//ARPE使能 
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
	TIM_Cmd(TIM8, ENABLE);  //使能TIM14				 
	//此部分需手动修改IO口设置
	
}  



