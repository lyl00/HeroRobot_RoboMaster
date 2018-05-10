/************************************************************
 *File		:	usart3.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com	@Jason , tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Usart3, for view data communication
                PD8 -> USART3_TX
								PD9 -> USART3_RX
 ************************************************************/
#include "usart3.h"

u8  USART_RX_BUF3[MAXLBUF];  

void USART3_DMA_Init(void)
{
/* -------------- Enable Module Clock Source ----------------------------*/
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA1, ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
GPIO_PinAFConfig(GPIOD,GPIO_PinSource8, GPIO_AF_USART3); 
GPIO_PinAFConfig(GPIOD,GPIO_PinSource9, GPIO_AF_USART3);
/* -------------- Configure GPIO ---------------------------------------*/
		{
				GPIO_InitTypeDef gpio;
				USART_InitTypeDef usart3;
				gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
				gpio.GPIO_Mode = GPIO_Mode_AF;
				gpio.GPIO_OType = GPIO_OType_PP;
				gpio.GPIO_Speed = GPIO_Speed_100MHz;
				gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
				GPIO_Init(GPIOD, &gpio);
			
				USART_DeInit(USART3);
				usart3.USART_BaudRate = 115200;
				usart3.USART_WordLength = USART_WordLength_8b;
				usart3.USART_StopBits = USART_StopBits_1;
				usart3.USART_Parity = USART_Parity_No ;   
				usart3.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; 
				usart3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
				USART_Init(USART3,&usart3);
				USART_Cmd(USART3,ENABLE);
				USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
		}		

		/* -------------- Configure NVIC ---------------------------------------*/
		{
				NVIC_InitTypeDef nvic;
				nvic.NVIC_IRQChannel = DMA1_Stream1_IRQn;      
				nvic.NVIC_IRQChannelPreemptionPriority = 2;
				nvic.NVIC_IRQChannelSubPriority = 3;
				nvic.NVIC_IRQChannelCmd = ENABLE;
				NVIC_Init(&nvic);
		}
		
		/* -------------- Configure DMA -----------------------------------------*/
		{
				DMA_InitTypeDef dma;
				DMA_DeInit(DMA1_Stream1);                     
				dma.DMA_Channel = DMA_Channel_4;
				dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
				dma.DMA_Memory0BaseAddr = (uint32_t)USART_RX_BUF3;   
				dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
				dma.DMA_BufferSize = MAXLBUF;       
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
				DMA_Init(DMA1_Stream1,&dma);
				DMA_ITConfig(DMA1_Stream1,DMA_IT_TC,ENABLE);
				DMA_Cmd(DMA1_Stream1,ENABLE);
		}	
}

void DMA1_Stream1_IRQHandler(void)
{
		if(DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1))
		{
			DMA_ClearFlag(DMA1_Stream1, DMA_IT_TCIF1);
			DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);	
		}
}

void UART3_Send(u8 *buffer, u8 len)
{
	u8 i=0;
	while (i < len) 
	{ 
		while((USART3->SR&0X40)==0); //循环发送,直到发送完毕   
		USART3->DR = (u8) buffer[i++];      
	} 
}
