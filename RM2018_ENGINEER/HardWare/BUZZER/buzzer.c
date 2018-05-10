/************************************************************
 *File		:	buzzer.c
 *Author	:	@YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Buzzer
 ************************************************************/

#include "main.h"

//Buzzer_Init
//���ܣ�Buzzer�����ʼ������
//������PB4 TIM3_CH1    PWM����������
//����ֵ��void
void Buzzer_Init(void)     //PB4
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM4ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//ʹ��PORTBʱ��	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_TIM3); 	//GPIOH9����Ϊ��ʱ��12
	
	/* TIM3 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           //GPIOB 6 7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOB,&GPIO_InitStructure);              	//��ʼ��
	  
	TIM_TimeBaseStructure.TIM_Period=2000;   	//�Զ���װ��ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler=72-1;  	//��ʱ����Ƶ     					///******����Ĺ�
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);		//����ʱ��12�����ж�
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);	//��ʼ����ʱ��12
	
	//��ʼ��TIM3 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ; 		//�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  						//����Tָ���Ĳ�����ʼ������TIM1 4OC1

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  	//ʹ��TIM4��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM3,ENABLE);					//ARPEʹ�� 
	TIM_Cmd(TIM3, ENABLE);  							//ʹ��TIM14
	BUZZER_OFF();
}

//Buzzer_toggle
//���ܣ�����������1��
//�������� 
//����ֵ��void
void Buzzer_toggle(void)
{
	BUZZER_ON(1500);
	delay_ms(300);
	BUZZER_OFF();
	delay_ms(300);
}
