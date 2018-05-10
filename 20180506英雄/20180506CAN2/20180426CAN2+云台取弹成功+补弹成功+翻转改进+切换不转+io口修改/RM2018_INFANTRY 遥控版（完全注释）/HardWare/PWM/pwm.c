#include "main.h"

//����TIM5��channel 1  ��Ӧ�˿�PA0
void TIM8_PWM_Init(u32 arr,u32 psc)
{	
	GPIO_InitTypeDef 					GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  				TIM_OCInitStructure;
//	NVIC_InitTypeDef 					NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 ,ENABLE);  	//TIM4ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE); 	//ʹ��PORTBʱ��	
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource5,GPIO_AF_TIM8); //GPIOD13����Ϊ��ʱ��4
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource6,GPIO_AF_TIM8); //GPIOD13����Ϊ��ʱ��4
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource7,GPIO_AF_TIM8); //GPIOD13����Ϊ��ʱ��4
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource2,GPIO_AF_TIM8); //GPIOD13����Ϊ��ʱ��4
	
	/* TIM8 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7 | GPIO_Pin_2 ;           //GPIOB 6 7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOI,&GPIO_InitStructure);              //��ʼ��
	  
	TIM_TimeBaseStructure.TIM_Period=2000;   //�Զ���װ��ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler=840-1;  //��ʱ����Ƶ               ///******����Ĺ�
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	
	
	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE); //����ʱ��12�����ж�
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseStructure);//��ʼ����ʱ��12
	
	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ; //�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	//��ʼ��TIM14 Channel2 PWMģʽ	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High ; //�������:TIM����Ƚϼ��Ե�
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //ʹ��TIM4��CCR1�ϵ�Ԥװ�ؼĴ���
  TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  //ʹ��TIM4��CCR2�ϵ�Ԥװ�ؼĴ���
  
	TIM_ARRPreloadConfig(TIM8,ENABLE);//ARPEʹ�� 
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
	TIM_Cmd(TIM8, ENABLE);  //ʹ��TIM14				 
	//�˲������ֶ��޸�IO������
	
}  



