/*************************************************
Function		: BSP_Init
Description	: Init NVIC, Delay, GPIO, USART, CAN , RC, PWM ,TIMER
Input				: None
Output			: None
Return			: None
*************************************************/

#include "main.h"

void BSP_Init(void)	//ע���ʼ����˳��
{
	//��ʼ����ʱ����	
	delay_init(168);  
	//��ʼ��led�ײ�	
	Led_Configuration(); 
	//��ʼ��������
	Buzzer_Init();
//	//��ʼ������
//	Laser_Configuration();
//	//��ʼ���������ģ��
//	Adc_Init();
	//��ʼ������6�ײ�
	USART6_Configuration(115200); 
	//��ʼ������3��DMA1�ײ�
	USART3_DMA_Init();
	//��ʼ��SPI5,���ڶ�ȡ��������������
	SPI5Init();
	
//	//��ʼ��usart2���ڡ�DMA2������ң�������ݶ�ȡ		
//	RC_Init();
	
	//CAN1��ʼ����������1Mbps
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_9tq,3,CAN_Mode_Normal);
//	//CAN2��ʼ����������1Mbps
//	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_9tq,3,CAN_Mode_Normal);
	//��ʼ����ʱ��4�����ڿ��ƺ����Ͷ��
	TIM4_Init();
	
	//��ʼ����ʱ��4�����ڿ���������ӽ�
	TIM8_Init();
	
	
  /* ���������ʼ��*/
	WholeInitTask();	
	
	/*��ʱ����ʼ��*/
	TIM2_Init();   //��ʼ����ʱ��2�����������Ƕ�ֵ���£���ʱ�����ǻ�û�г�ʼ����������
//	delay_ms(500); //�ȴ�0.5s������̨�ȶ�
	TIM5_Init();   //��ʼ����ʱ��5�����ڿ�������ִ�У���ʱ�����ǿ�ʼ������λ�ü�Ϊ��ʼλ��
}