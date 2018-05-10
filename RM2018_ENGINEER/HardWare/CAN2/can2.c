/************************************************************
 *File		:	CAN2.c
 *Author	:  	@YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com, @YangTao,ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.04.15
 *Description: 	Use CAN2 to control motor
				bound 1Mbps
				PB13(tx) PB12(rx)
 ************************************************************/

#include "main.h"

//CAN初始化
//tsjw:重新同步跳跃时间单元. @ref CAN_synchronisation_jump_width   范围: ; CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   @ref CAN_time_quantum_in_bit_segment_2 范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   @refCAN_time_quantum_in_bit_segment_1  范围: ;	  CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//波特率=Fpclk1/((tsjw+tbs1+tbs2+3)*brp);
//mode: @ref CAN_operating_mode 范围：CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Normal_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//则波特率为:42M/((1+6+7)*6)=500Kbps
//返回值:0,初始化OK;
//其他,初始化失败;


#define CAN2_ID_MASK		0x07


float 				can2_current_angel[CAN2_MOTOR_COUNT]={0.f,0.f,0.f,0.f};
volatile int16_t 	can2_current_motor_current[CAN2_MOTOR_COUNT];
volatile int16_t 	can2_current_motor_speed[CAN2_MOTOR_COUNT];
volatile uint16_t 	can2_stuckflag[CAN2_MOTOR_COUNT];		//集成卡位检测

// Inner parameters
CanRxMsg can2_rx_message;
volatile int16_t 	can2_current_motor_angel[CAN2_MOTOR_COUNT];
volatile int16_t 	can2_last_motor_angel[CAN2_MOTOR_COUNT]={-1,-1,-1,-1};

//CAN2_Mode_Init
//功能：CAN2初始化函数
//参数：传入tsjw,tbs2,tbs1,brp,mode 决定CAN2的模式和波特率
//返回值：0
u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
  	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
  	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
   	NVIC_InitTypeDef  		NVIC_InitStructure;

    //使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//使能CAN2时钟	
	
    //初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化PA11,PA12
	
	//引脚复用映射配置
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2); //GPIOB12复用为CAN2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2); //GPIOB13复用为CAN2
	  	  
  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=ENABLE;//	DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= mode;	 //模式设置 
  	CAN_InitStructure.CAN_SJW=tsjw;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN2, &CAN_InitStructure);   // 初始化CAN2 
    
	//配置过滤器
 	CAN_FilterInitStructure.CAN_FilterNumber=14;	  //过滤器1
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO1;//过滤器0关联到FIFO1
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器1
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
	
	CAN_ITConfig(CAN2,CAN_IT_FMP1,ENABLE);//FIFO1消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 主优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
	return 0;
}   

void CAN2_RX1_IRQHandler(void)
{
	static uint8_t id;
	static float delta = 0.f;
	
    if (CAN_GetITStatus(CAN2,CAN_IT_FMP1)!= RESET) 
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);
        CAN_Receive(CAN2, CAN_FIFO1, &can2_rx_message);
		
		id = can2_rx_message.StdId&CAN2_ID_MASK;
		if(id>0 && id<5) {
			//转速（RPM）
			can2_current_motor_speed[id-1] = (can2_rx_message.Data[2]<<8) | can2_rx_message.Data[3];
			//转矩电流
			can2_current_motor_current[id-1] = (can2_rx_message.Data[4]<<8) | can2_rx_message.Data[5];
			//码盘值
			can2_current_motor_angel[id-1]	= (can2_rx_message.Data[0]<<8) | can2_rx_message.Data[1];
			//角度监视
			if(can2_last_motor_angel[id-1]!=-1){
				delta = can2_current_motor_angel[id-1]-can2_last_motor_angel[id-1];
				delta+=(delta>4096)?-8192:((delta<-4096)?8192:0);
				//203 204
				if(id==3 || id==4)
					can2_current_angel[id-1] -= delta/19.f;
				else
					can2_current_angel[id-1] += delta/19.f;
				//记录角增量是否大于一定值，否则视为没有移动
				if((delta<CAN2_STUCK_FILTER)&&(delta>-CAN2_STUCK_FILTER)){
					can2_stuckflag[id-1] += (can2_stuckflag[id-1]==(uint16_t)-1)?0:1;
				}else{
					can2_stuckflag[id-1] = 0;
				}
			}
			can2_last_motor_angel[id-1] = can2_current_motor_angel[id-1];			
		}	
    }
}



/***************************************************************************************
 *Name     : 
 *Function ：Send data for 201 202 203 204
 *Input    ：无
 *Output   ：无 
 *Description : ,int16_t candata2,int16_t candata3,int16_t candata4)
****************************************************************************************/
void CAN2_Motor(int16_t candata1, int16_t candata2, int16_t candata3, int16_t candata4)
{
	CanTxMsg tx_message;
	tx_message.StdId=0x200;	           			// 标准标识符为0x200
	tx_message.ExtId=0x00;	             		// 设置扩展标示符（29位）
	tx_message.IDE=CAN_ID_STD;					// 使用标准帧模式
	tx_message.RTR=CAN_RTR_DATA;				// 发送的是数据
	tx_message.DLC=8;							// 数据长度为2字节
	tx_message.Data[0] = (uint8_t)(candata1 >> 8);
	tx_message.Data[1] = (uint8_t)candata1;
	tx_message.Data[2] = (uint8_t)(candata2 >> 8);
	tx_message.Data[3] = (uint8_t)candata2;
	tx_message.Data[4] = (uint8_t)(candata3 >> 8);
	tx_message.Data[5] = (uint8_t)candata3;
	tx_message.Data[6] = (uint8_t)(candata4 >> 8);
	tx_message.Data[7] = (uint8_t)candata4;
    
	CAN_Transmit(CAN2,&tx_message);
}

