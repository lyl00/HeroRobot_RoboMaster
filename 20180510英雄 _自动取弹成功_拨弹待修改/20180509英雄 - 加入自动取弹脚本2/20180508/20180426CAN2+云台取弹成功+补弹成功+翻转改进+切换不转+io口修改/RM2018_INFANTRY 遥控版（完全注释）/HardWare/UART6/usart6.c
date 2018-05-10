/************************************************************
 *File		:	usart1.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com	@YangTianhao ,490999282@qq.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Usart1 for common use
 ************************************************************/
 
#include "stm32f4xx.h"
#include "usart6.h"	
#include "string.h"
#include "main.h"

//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif
char usart6_rxbuf;
u8 usart6_flag=0;

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
	USART6->DR = (u8) ch;      
	return ch;
}
#endif

#if EN_USART6_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	

//��ʼ��IO ����6 
//bound:������
void USART6_Configuration(u32 bound){
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); 	//GPIOG9����ΪUSART6
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); 	
	
	//USART6�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; // C6  TX  �� C7    RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOG,&GPIO_InitStructure); //

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART6, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART6, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART6, USART_FLAG_TC);
	
#if EN_USART6_RX	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
	
}

#endif	
//}

void UART6_Send(u8 *buffer, u8 len)
{
	u8 i=0;
	while (i < len) 
	{ 
		while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
		USART6->DR = (u8) buffer[i++];      
	} 
}


u8 USART6_dma[USART6_dma_len];
/*USART6 DMA test*/
void USART6_DMA_Init(void)
{
		/* -------------- Enable Module Clock Source ----------------------------*/
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG , ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//??USART1??
	
GPIO_PinAFConfig(GPIOG,GPIO_PinSource14, GPIO_AF_USART6);  //PD8 TX  PD9 RX
GPIO_PinAFConfig(GPIOG,GPIO_PinSource9, GPIO_AF_USART6);  
/* -------------- Configure GPIO ---------------------------------------*/
		{
				GPIO_InitTypeDef gpio;
				USART_InitTypeDef usart6;
				gpio.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_9;
				gpio.GPIO_Mode = GPIO_Mode_AF;
				gpio.GPIO_OType = GPIO_OType_PP;
				gpio.GPIO_Speed = GPIO_Speed_100MHz;
				gpio.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOG, &gpio);
			
				USART_DeInit(USART6);
				usart6.USART_BaudRate = 115200;
				usart6.USART_WordLength = USART_WordLength_8b;
				usart6.USART_StopBits = USART_StopBits_1;
				usart6.USART_Parity = USART_Parity_No ;    //USART_Parity_Even;
				usart6.USART_Mode = USART_Mode_Rx|USART_Mode_Tx; //USART_Mode_Rx;
				usart6.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
				USART_Init(USART6,&usart6);
				USART_Cmd(USART6,ENABLE);
				USART_ClearFlag(USART6, USART_FLAG_TC);
			
				USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);
				USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);

			
		}		


		/* -------------- Configure NVIC ---------------------------------------*/
		{
				NVIC_InitTypeDef nvic;
			
				nvic.NVIC_IRQChannel = DMA2_Stream1_IRQn;      //Stream 1 
				nvic.NVIC_IRQChannelPreemptionPriority = 1;
				nvic.NVIC_IRQChannelSubPriority = 1;
				nvic.NVIC_IRQChannelCmd = ENABLE;
				NVIC_Init(&nvic);
			
				nvic.NVIC_IRQChannel = DMA2_Stream6_IRQn;      //Stream 1 			
				nvic.NVIC_IRQChannelPreemptionPriority = 1;
				nvic.NVIC_IRQChannelSubPriority = 1;
				nvic.NVIC_IRQChannelCmd = ENABLE;

				NVIC_Init(&nvic);
			
		}
		
		/* -------------- Configure DMA -----------------------------------------*/
		{
				DMA_InitTypeDef dma;
			
				DMA_DeInit(DMA2_Stream1);                      //DMA2_stream2 channel 5
				dma.DMA_Channel = DMA_Channel_5;
				dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART6->DR);
				dma.DMA_Memory0BaseAddr = (uint32_t)USART6_dma;    //??
				dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
				dma.DMA_BufferSize = USART6_dma_len;       //???
				dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
				dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
				dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
				dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
				dma.DMA_Mode = DMA_Mode_Circular;
				dma.DMA_Priority = DMA_Priority_VeryHigh;
				dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
				dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
				dma.DMA_MemoryBurst = DMA_Mode_Normal;
				dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
				DMA_Init(DMA2_Stream1,&dma);
				DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);
				DMA_Cmd(DMA2_Stream1,ENABLE);
				
//					DMA_InitTypeDef dma;
				DMA_DeInit(DMA2_Stream6);                      //DMA1_stream1 channel 4
				dma.DMA_Channel = DMA_Channel_5;
				dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART6->DR);
				dma.DMA_Memory0BaseAddr = (uint32_t)Personal_Data;    //����
				dma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
				dma.DMA_BufferSize = 21;       //Ҫ�ģ�
				
				dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
				dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
				dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
				dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
				dma.DMA_Mode = DMA_Mode_Normal;
				dma.DMA_Priority = DMA_Priority_Medium;
				dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
				dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
				dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
				dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
				DMA_Init(DMA2_Stream6,&dma);			
		}

}

u8 Personal_Data[22];

void RefereeSend(void)
{
	
	DMA_Cmd(DMA2_Stream6, DISABLE);                      //??DMA?? 
	
	while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE){}	//??DMA?????  
	DMA_ClearFlag(DMA2_Stream6,DMA_FLAG_TCIF6);
	DMA_SetCurrDataCounter(DMA2_Stream6,21);          //?????  
	DMA_Cmd(DMA2_Stream6, ENABLE);                      //??DMA?? 
}


void DMA2_Stream1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1))
	{
		DMA_ClearFlag(DMA2_Stream1, DMA_IT_TCIF1);
		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);	
	}
}

void DMA2_Stream6_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream6, DMA_IT_TCIF6))
	{
		DMA_ClearFlag(DMA2_Stream6, DMA_IT_TCIF6);
		DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6);	
	}
}
