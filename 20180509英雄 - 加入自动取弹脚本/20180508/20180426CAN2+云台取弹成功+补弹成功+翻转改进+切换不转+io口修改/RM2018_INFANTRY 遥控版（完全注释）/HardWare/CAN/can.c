/************************************************************
 *File		:	can1.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Use CAN1 to control CM motor
								bound 1Mbps
								
								CMcontrol and GimbalControl Use Can1 together!   2017.2.23
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

CanRxMsg rx_message;
CanRxMsg rx_message2;

volatile int16_t current_position_206;    	//pitch gimbal
volatile int16_t current_position_205;			//yaw gimbal
volatile int16_t current_cm_201;
volatile int16_t current_cm_202;
volatile int16_t current_cm_203;
volatile int16_t current_cm_204;




volatile int16_t can2_current_speed201=0;//CAN2的电机的速度返回值,201抬升电机，202翻转电机
volatile int16_t can2_current_speed202=0;
volatile int16_t can2_current_speed203=0;
volatile int16_t can2_current_speed204=0;
volatile int16_t can2_current_angle201=0;//CAN2的电机的机械角度返回值
volatile int16_t can2_current_angle202=0;
volatile int16_t can2_current_angle203=0;
volatile int16_t can2_current_angle204=0;
volatile int16_t can2_last_angle201=-1;//CAN2的电机的机械角度返回值
volatile int16_t can2_last_angle203=-1;
volatile int16_t can2_last_angle202=-1;
volatile int16_t can2_last_angle204=-1;

float current_angle_201=0,current_angle_203=0,current_angle_202=0,current_angle_204=4000;
int16_t delta;
int timell=0;





/*------------以下为新增内容-----------*/





//CAN1_Mode_Init
//功能：CAN1初始化函数
//参数：传入tsjw,tbs2,tbs1,brp,mode 决定CAN1的模式和波特率
//返回值：0
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
   	NVIC_InitTypeDef  NVIC_InitStructure;

    //使能相关时钟
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能PORTA时钟	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
	
    //初始化GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化PA11,PA12
	
	  //引脚复用映射配置
	  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1); //GPIOD0复用为CAN1
	  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1); //GPIOD1复用为CAN1
	  	  
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
  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 
    
		//配置过滤器
 	  CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
		
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
	return 0;
}   

u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
   	NVIC_InitTypeDef  NVIC_InitStructure;

    //使能相关时钟
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能PORTB时钟	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//使能CAN2时钟	
	
    //初始化GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化PB12,PB13
	
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
 	  CAN_FilterInitStructure.CAN_FilterNumber=14;	  //过滤器14//参考网上建议改成过滤器14，其他没变
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO1;//过滤器14关联到FIFO1
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器1
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
		
	  CAN_ITConfig(CAN2,CAN_IT_FMP1,ENABLE);//FIFO0消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
	return 0;
}  
	    
void CAN1_RX0_IRQHandler(void)
{   
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET) 
		{
				CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
        CAN_Receive(CAN1, CAN_FIFO0, &rx_message);       
                
        if(rx_message.StdId == 0x206)           //206 是pitch
        {             
             current_position_206 = (rx_message.Data[0]<<8) | rx_message.Data[1];	
					
        }
        else if(rx_message.StdId == 0x205)      //205 是YAW轴            
        { 
             current_position_205 = (rx_message.Data[0]<<8) | rx_message.Data[1];
        }
				else if(rx_message.StdId == 0x207)      //拨弹轮电机           
        { 	
						//current_shoot_speed_207 = (rx_message.Data[2]<<8) | rx_message.Data[3];		
					/*----------新增-----------*/
						//current_shoot_pos_207=(rx_message.Data[0]<<8)|rx_message.Data[1];//读取拨弹盘电机的机械角度
        }
				else if(rx_message.StdId == 0x201)      //左前          
        { 
             current_cm_201 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
        }
				else if(rx_message.StdId == 0x202)       //右前         
        { 
             current_cm_202 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
        }
				else if(rx_message.StdId == 0x203)       //右后        
        { 
             current_cm_203 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
        }
				else if(rx_message.StdId == 0x204)       //左后        
        { 
             current_cm_204 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
        }
				if(timell%500==1){
//			printf("PITCH码盘0:%d\r\n",current_position_206);
//	    printf("Yaw码盘:%d\r\n",current_position_205);
				}timell++;
	}
}

void CAN2_RX1_IRQHandler(void)
{
	 if (CAN_GetITStatus(CAN2,CAN_IT_FMP1)!= RESET) 
		{
				CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);
        CAN_Receive(CAN2, CAN_FIFO1, &rx_message2);       
               
				if(rx_message2.StdId == 0x201)      //抬升电机          
        { 
          can2_current_speed201 = (rx_message2.Data[2]<<8) | rx_message2.Data[3];	
					can2_current_angle201 = (rx_message2.Data[0]<<8) | rx_message2.Data[1];
					if(can2_last_angle201!=-1)
						{
							delta = can2_current_angle201-can2_last_angle201;
							delta+=(delta>4096)?-8192:((delta<-4096)?8192:0);
							current_angle_201 = current_angle_201 + delta*1.0/19;
						}
						can2_last_angle201 = can2_current_angle201;
//						printf("ok\n");//加一个输出来看这一段能否被执行
				}
				else if(rx_message2.StdId == 0x202)       //旋转电机1         
				{ 
					can2_current_speed202 = (rx_message2.Data[2]<<8) | rx_message2.Data[3];	
					can2_current_angle202 = (rx_message2.Data[0]<<8) | rx_message2.Data[1];
					if(can2_last_angle202!=-1)
						{
							delta = can2_current_angle202-can2_last_angle202;
							delta+=(delta>4096)?-8192:((delta<-4096)?8192:0);
							current_angle_202 = current_angle_202 + delta*1.0/19;
						}
						can2_last_angle202 = can2_current_angle202;
					
				}
				else if(rx_message2.StdId == 0x203)       //抬升电机2        
				{ 
					can2_current_speed203 = (rx_message2.Data[2]<<8) | rx_message2.Data[3];	
					can2_current_angle203 = (rx_message2.Data[0]<<8) | rx_message2.Data[1];
					if(can2_last_angle203!=-1)
						{
							delta = can2_current_angle203-can2_last_angle203;
							delta+=(delta>4096)?-8192:((delta<-4096)?8192:0);
							current_angle_203 = current_angle_203 + delta*1.0/19;
						}
						can2_last_angle203 = can2_current_angle203;
				}
				else if(rx_message2.StdId == 0x204)       //左后       
        { 
          can2_current_speed204 = (rx_message2.Data[2]<<8) | rx_message2.Data[3];	
					can2_current_angle204 = (rx_message2.Data[0]<<8) | rx_message2.Data[1];
					if(can2_last_angle204!=-1)
						{
							delta = can2_current_angle204-can2_last_angle204;
							delta+=(delta>4096)?-8192:((delta<-4096)?8192:0);
							current_angle_204 = current_angle_204 + delta*1.0/19;
						}
						can2_last_angle204 = can2_current_angle204;
					
    }
			
    }

}

/***************************************************************************************
 *Name     : CAN1_Cmd_Bottom
 *Function ：CAN1底盘控制程序
 *Input    ：Speed A B C D ，输出四个电机的值
 *Output   ：无 
 *Description : 控制四个底盘电机运动
****************************************************************************************/
void CAN1_Send_Bottom(int16_t candata1,int16_t candata2,int16_t candata3,int16_t candata4)
{	
	CanTxMsg TxMessage;
  TxMessage.StdId=0x200;	           // 标准标识符为0x200
  TxMessage.ExtId=0x00;	             // 设置扩展标示符（29位）
  TxMessage.IDE=CAN_ID_STD;					 // 使用标准帧模式
  TxMessage.RTR=CAN_RTR_DATA;				 // 发送的是数据
  TxMessage.DLC=8;							     // 数据长度为2字节
  TxMessage.Data[0]= (uint8_t)(candata1 >> 8);
  TxMessage.Data[1]= (uint8_t) candata1;	
	TxMessage.Data[2]= (uint8_t)(candata2 >> 8);
  TxMessage.Data[3]= (uint8_t) candata2;	
	TxMessage.Data[4]= (uint8_t)(candata3 >> 8);
  TxMessage.Data[5]= (uint8_t) candata3;	
	TxMessage.Data[6]= (uint8_t)(candata4 >> 8);
  TxMessage.Data[7]= (uint8_t) candata4;	
	
	CAN_Transmit(CAN1, &TxMessage);
}



/***************************************************************************************
 *Name     : CAN1_Cmd_SHOOT
 *Function ：CAN1拨弹电机控制
 *Input    ：无
 *Output   ：无 
 *Description : 单独控制拨弹轮电机
****************************************************************************************/
void CAN2_Cmd_SHOOT(int16_t current_204)
{
	CanTxMsg tx_message;
  tx_message.StdId = 0x200;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_Data;
  tx_message.DLC = 0x08;
        
	tx_message.Data[0]= ((uint8_t) liftspeed1 >> 8);
  tx_message.Data[1]= (uint8_t) liftspeed1;	
	tx_message.Data[2]= 0x00;
  tx_message.Data[3]= 0x00;	
	tx_message.Data[4]= ((uint8_t) liftspeed2>> 8);
  tx_message.Data[5]= (uint8_t) liftspeed2;	
  tx_message.Data[6] = (unsigned char)(current_204 >> 8);
  tx_message.Data[7] = (unsigned char)current_204;
    
	CAN_Transmit(CAN2,&tx_message);
}

void CAN2_Send_Bottom(int16_t candata1,int16_t candata2,int16_t candata3/*,int16_t candata4*/)
{	
	CanTxMsg TxMessage;
  TxMessage.StdId=0x200;	           // 标准标识符为0x200
  TxMessage.ExtId=0x00;	             // 设置扩展标示符（29位）
  TxMessage.IDE=CAN_ID_STD;					 // 使用标准帧模式
  TxMessage.RTR=CAN_RTR_DATA;				 // 发送的是数据
  TxMessage.DLC=8;							     // 数据长度为2字节
  TxMessage.Data[0]= (uint8_t)(candata1 >> 8);
  TxMessage.Data[1]= (uint8_t) candata1;	
	TxMessage.Data[2]= (uint8_t)(candata2 >> 8);
  TxMessage.Data[3]= (uint8_t) candata2;	
	TxMessage.Data[4]= (uint8_t)(candata3 >> 8);
  TxMessage.Data[5]= (uint8_t) candata3;	
	TxMessage.Data[6]= 0x00;
  TxMessage.Data[7]= 0x00;	
	
	CAN_Transmit(CAN2, &TxMessage);
}


/***************************************************************************************
 *Name     : CAN1_Cmd_standard
 *Function ：云台电机校准
 *Input    ：无
 *Output   ：无 
 *Description : 新电机初次接入时需校准
****************************************************************************************/
void CAN1_Cmd_standard(void)
{
		CanTxMsg tx_message;
		tx_message.StdId = 0x3f0;
		tx_message.IDE = CAN_Id_Standard;
		tx_message.RTR = CAN_RTR_Data;
		tx_message.DLC = 0x08;
    
		tx_message.Data[0] = 'c';
		tx_message.Data[1] = 0x00;
    tx_message.Data[2] = 0x00;
		tx_message.Data[3] = 0x00;
    tx_message.Data[4] = 0x00;
		tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
		
		CAN_Transmit(CAN1,&tx_message);
}


/***************************************************************************************
 *Name     : CAN1_Cmd_All
 *Function ：CAN1云台电机控制程序
 *Input    ：传入pitch、yaw电机控制值
 *Output   ：无 
 *Description : 控制云台运动，包括pitch轴、yaw轴和拨弹轮
****************************************************************************************/
void CAN1_Cmd_All(int16_t current_205,int16_t current_206)
{
    CanTxMsg tx_message;
    
    tx_message.StdId = 0x1ff;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
		tx_message.Data[0] = (unsigned char)(current_205 >> 8);
    tx_message.Data[1] = (unsigned char) current_205;
    tx_message.Data[2] = (unsigned char)(current_206 >> 8);
    tx_message.Data[3] = (unsigned char) current_206;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
    
    CAN_Transmit(CAN1,&tx_message);
}





