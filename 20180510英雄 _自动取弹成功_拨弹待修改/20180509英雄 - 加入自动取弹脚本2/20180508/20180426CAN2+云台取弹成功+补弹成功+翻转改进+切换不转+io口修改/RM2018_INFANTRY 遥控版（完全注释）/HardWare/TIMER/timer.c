/************************************************************
 *File		:	timer.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com	
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Timer5: 1ms, for Control_Task()
								Timer2: 1ms, for IMU Calculation
								Timer12: 50HZ, for Friction Wheel control
 ************************************************************/

#include "main.h"

#define Init_time 2000 

u16 TIM5_time_count = 0;  //TIM5计时
u8 flag_Ready = 0; //初始化完成标志

/*-------------  定时器5初始化  -------------*/
void TIM5_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = 1000; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=84-1;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);//初始化TIM3
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM5,ENABLE); //使能定时器5
	
}

/*-------------  定时器5中断服务  -------------*/
void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //溢出中断
	{			
		if(!flag_Ready)
		{
			if (TIM5_time_count < Init_time)   //等2秒，使初始化完成，并使云台以较慢速度归中，此时运行PREPARE_STATE模式，不接收遥控器数据，见函数Remote_State_FSM()
			{
				TIM5_time_count++;
			}
			else 
			{			 
				 TIM5_time_count = 0;
				 flag_Ready = 1;
				 GimbalPidInit();       //初始化完成后，以新的pid控制云台
			}
		}
		Control_Task();             //主任务循环
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //清除中断标志位
}

/*-------------  定时器2初始化  -------------*/
void TIM2_Init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///使能TIM2时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = 1000; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=84-1;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化TIM2
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //允许定时器2更新中断
	TIM_Cmd(TIM2,ENABLE); //使能定时器2
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM2_IRQHandler(void)      //1ms产生一次中断
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
		  update_euler();        //陀螺仪角度更新
			Read_Infrared_State();
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
	
}

/************************************************************
*定时器12初始化，用于控制摩擦轮转动
*************************************************************/
void TIM12_Init(void)
{
	GPIO_InitTypeDef 					GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  				TIM_OCInitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);  ///TIM3时钟使能	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE); 	//使能PORTH时钟	

	GPIO_PinAFConfig(GPIOH,GPIO_PinSource9,GPIO_AF_TIM12); //GPIOH9复用为定时器12
	GPIO_PinAFConfig(GPIOH,GPIO_PinSource6,GPIO_AF_TIM12); //GPIOH6复用为定时器12
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_6 ;   //GPIOB 6 9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOH,&GPIO_InitStructure);              //初始化
	  
	TIM_TimeBaseStructure.TIM_Period=2000-1;   //自动重装载值	
	TIM_TimeBaseStructure.TIM_Prescaler=720-1;  //定时器分频            
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	
	TIM_ITConfig(TIM12,TIM_IT_Update,ENABLE); //允许定时器12更新中断
	TIM_TimeBaseInit(TIM12,&TIM_TimeBaseStructure);//初始化定时器12
	
	//初始化TIM12 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ; //输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM12, &TIM_OCInitStructure); 
	//初始化TIM12 Channel2 PWM模式	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ; //输出极性:TIM输出比较极性低
	TIM_OC2Init(TIM12, &TIM_OCInitStructure);  
	
	TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);  //使能TIM12在CCR1上的预装载寄存器
  TIM_OC2PreloadConfig(TIM12, TIM_OCPreload_Enable);  //使能TIM12在CCR2上的预装载寄存器
  
	TIM_ARRPreloadConfig(TIM12,ENABLE);//ARPE使能 
	
	TIM_Cmd(TIM12, ENABLE);  //使能TIM12
	
}
