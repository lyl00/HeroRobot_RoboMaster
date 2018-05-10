/************************************************************
 *File		:	rc.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com	@Jason ,tjx1024@126.com, @YTom(YangTao) ybb331082@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Use usart2 and DMA to accept Remote Data. 
				Data will be stored in the struct named "RC_Ctl" and "RC_Ex_Ctl".
				PB7				
 ************************************************************/
 
#include "main.h"

#define RC_MOUSE_X_MAX 	40 //鼠标移动限速
#define RC_MOUSE_Y_MAX 	40
#define rc_monitor_count_limit 100 //数据包计数上限

RC_Ctl_t RC_Ctl;
RC_Ctl_t RC_Ex_Ctl; 
//u8 flag_friction_switch = 0; //遥控器s1摇杆1、3档切换标志
//u8 friction_rc_last_position = 3 ;    //标记上一次摇杆位置
int16_t rc_monitor_count = 0, rc_monitor_ex_count = 0 ; //遥控器监控记录，通过比较前后两次判断是否收到新数据包
u8 flag_rc_offline = 0; //遥控器掉线标志

volatile unsigned char sbus_rx_buffer[18];
 
void RC_Init(void)
{
/* -------------- Enable Module Clock Source ----------------------------*/
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA2, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
GPIO_PinAFConfig(GPIOB,GPIO_PinSource7, GPIO_AF_USART1);     //PB7  usart1 rx
	
/* -------------- Configure GPIO ---------------------------------------*/
	{
		GPIO_InitTypeDef gpio;
		USART_InitTypeDef usart1;
		gpio.GPIO_Pin = GPIO_Pin_7;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_Speed = GPIO_Speed_100MHz;
		gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &gpio);
	
		USART_DeInit(USART1);
		usart1.USART_BaudRate = 100000;
		usart1.USART_WordLength = USART_WordLength_8b;
		usart1.USART_StopBits = USART_StopBits_1;
		usart1.USART_Parity = USART_Parity_Even;
		usart1.USART_Mode = USART_Mode_Rx;
		usart1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(USART1,&usart1);
		USART_Cmd(USART1,ENABLE);
		USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	}

	/* -------------- Configure NVIC ---------------------------------------*/
	{
		NVIC_InitTypeDef nvic;
		nvic.NVIC_IRQChannel = DMA2_Stream2_IRQn;
		nvic.NVIC_IRQChannelPreemptionPriority = 1;
		nvic.NVIC_IRQChannelSubPriority = 1;
		nvic.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvic);
	}
	/* -------------- Configure DMA -----------------------------------------*/
	{
		DMA_InitTypeDef dma;
		DMA_DeInit(DMA2_Stream2);
		dma.DMA_Channel = DMA_Channel_4;
		dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
		dma.DMA_Memory0BaseAddr = (uint32_t)sbus_rx_buffer;
		dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dma.DMA_BufferSize = 18;
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
		DMA_Init(DMA2_Stream2,&dma);
		DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);
		DMA_Cmd(DMA2_Stream2,DISABLE);							//Add a disable
		DMA_Cmd(DMA2_Stream2,ENABLE);
	}
}


void DMA2_Stream2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2))
	{
		DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
		DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
		RC_Ctl.rc.ch0 = (sbus_rx_buffer[0]| (sbus_rx_buffer[1] << 8)) & 0x07ff; //!< Channel 0
		RC_Ctl.rc.ch1 = ((sbus_rx_buffer[1] >> 3) | (sbus_rx_buffer[2] << 5)) & 0x07ff; //!< Channel 1
		RC_Ctl.rc.ch2 = ((sbus_rx_buffer[2] >> 6) | (sbus_rx_buffer[3] << 2) | //!< Channel 2
		(sbus_rx_buffer[4] << 10)) & 0x07ff;
		RC_Ctl.rc.ch3 = ((sbus_rx_buffer[4] >> 1) | (sbus_rx_buffer[5] << 7)) & 0x07ff; //!< Channel 3
		RC_Ctl.rc.s1 = ((sbus_rx_buffer[5] >> 4)& 0x000C) >> 2; //!< Switch left
		RC_Ctl.rc.s2 = ((sbus_rx_buffer[5] >> 4)& 0x0003); //!< Switch right
		RC_Ctl.mouse.x = sbus_rx_buffer[6] | (sbus_rx_buffer[7] << 8); //!< Mouse X axis
		RC_Ctl.mouse.y = sbus_rx_buffer[8] | (sbus_rx_buffer[9] << 8); //!< Mouse Y axis
		RC_Ctl.mouse.z = sbus_rx_buffer[10] | (sbus_rx_buffer[11] << 8); //!< Mouse Z axis
		RC_Ctl.mouse.press_l = sbus_rx_buffer[12]; //!< Mouse Left Is Press ?
		RC_Ctl.mouse.press_r = sbus_rx_buffer[13]; //!< Mouse Right Is Press ?
		RC_Ctl.key.v = sbus_rx_buffer[14]| (sbus_rx_buffer[15] << 8); //!< KeyBoard value
		RC_Ctl.resv  = sbus_rx_buffer[16] | (sbus_rx_buffer[17] << 8); //NULL          

		if(rc_monitor_count >= rc_monitor_count_limit)  //防止计数过多，占用内存
			rc_monitor_count = 0;

		rc_monitor_count++;    //数据包计数
	}
}


/***************************************************************************************
 *Name     : TestRc
 *Function ：显示遥控器值
 *Input    ：无
 *Output   ：无 
 *Description : 测试用，串口打印接收机接收到的遥控器值
****************************************************************************************/
void TestRc(void)
{	  
	printf("ch0:%d\r\n",RC_Ctl.rc.ch0);
	printf("ch1:%d\r\n",RC_Ctl.rc.ch1);
	printf("ch2:%d\r\n",RC_Ctl.rc.ch2);
	printf("ch3:%d\r\n",RC_Ctl.rc.ch3);
	printf("s1 :%d\r\n",RC_Ctl.rc.s1);     //遥杆打最上 1 中间 3 最下 2
	printf("s2 :%d\r\n",RC_Ctl.rc.s2);
	printf("m_x:%d\r\n",RC_Ctl.mouse.x);
	printf("m_y:%d\r\n",RC_Ctl.mouse.y);
	printf("m_l:%d\r\n",RC_Ctl.mouse.press_l);
	printf("m_r:%d\r\n",RC_Ctl.mouse.press_r);
	printf("key:%d\r\n",RC_Ctl.key.v);
	printf("res:%d\r\n",RC_Ctl.resv);
}


/***************************************************************************************
 *Name     : ItoE
 *Function ：处理遥控器键值
 *Input    ：无
 *Output   ：无 
 *Description : 将接收到的遥控器信号标准化，即中位为0，满量程为-660~660，
								s1、s2在上中下对应分别为：1、3、2
								将标准化后的值在串口打印
								并判断开关s1是否1、3位置切换过
****************************************************************************************/
void ItoE(void)
{
	RemoteSEF_REG = 0;
	RemoteSDF_REG = 0;
	
	RC_Ex_Ctl.rc.ch0 = RC_Ctl.rc.ch0 - 1024;
	RC_Ex_Ctl.rc.ch1 = RC_Ctl.rc.ch1 - 1024;
	RC_Ex_Ctl.rc.ch2 = RC_Ctl.rc.ch2 - 1024;
	RC_Ex_Ctl.rc.ch3 = RC_Ctl.rc.ch3 - 1024;
	RC_Ex_Ctl.rc.s1  = RC_Ctl.rc.s1;
	RC_Ex_Ctl.rc.s2  = RC_Ctl.rc.s2;

	RC_Ex_Ctl.mouse.x = RC_Ctl.mouse.x>RC_MOUSE_X_MAX? RC_MOUSE_X_MAX : (RC_Ctl.mouse.x< -RC_MOUSE_X_MAX ? -RC_MOUSE_X_MAX : RC_Ctl.mouse.x);
	RC_Ex_Ctl.mouse.y = RC_Ctl.mouse.y>RC_MOUSE_Y_MAX? RC_MOUSE_Y_MAX : (RC_Ctl.mouse.y< -RC_MOUSE_Y_MAX ? -RC_MOUSE_Y_MAX : RC_Ctl.mouse.y);
	RC_Ex_Ctl.mouse.press_l = RC_Ctl.mouse.press_l;
	RC_Ex_Ctl.mouse.press_r = RC_Ctl.mouse.press_r;
	RC_Ex_Ctl.key.v = RC_Ctl.key.v;

//	if(	friction_rc_last_position == 1 && RC_Ex_Ctl.rc.s1 == 3)   //判断遥控器s1是否1、3位置切换过
//		flag_friction_switch = 1;
//	friction_rc_last_position = RC_Ex_Ctl.rc.s1;	
	
}


	
