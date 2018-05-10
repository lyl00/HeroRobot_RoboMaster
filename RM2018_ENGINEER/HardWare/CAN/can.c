/************************************************************
 *File		:	can1.c
 *Author	:  	@YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com
 *Version	: 	V1.0
 *Update	: 	2017.12.11
 *Description: 	Use CAN1 to control CM motor
				bound 1Mbps
				CMcontrol and GimbalControl Use Can1 together!   2017.2.23
 ************************************************************/

#include "main.h"

//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ. @ref CAN_synchronisation_jump_width   ��Χ: ; CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   @ref CAN_time_quantum_in_bit_segment_2 ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   @refCAN_time_quantum_in_bit_segment_1  ��Χ: ;	  CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;(ʵ��Ҫ��1,Ҳ����1~1024) tq=(brp)*tpclk1
//������=Fpclk1/((tsjw+tbs1+tbs2+3)*brp);
//mode: @ref CAN_operating_mode ��Χ��CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Normal_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//������Ϊ:42M/((1+6+7)*6)=500Kbps
//����ֵ:0,��ʼ��OK;
//����,��ʼ��ʧ��;

volatile int16_t 	current_cm_speed[4]={0};
volatile int16_t 	current_cm_angle[4]={0};
float 				can1_current_angel205,can1_current_angel206;
volatile int16_t 	can1_current_motor_current205,can1_current_motor_current206;
volatile int16_t 	can1_current_motor_speed205,can1_current_motor_speed206;

// Inner parameters
volatile int16_t 	can1_current_motor_angel205,can1_current_motor_angel206;
volatile int16_t 	can1_last_motor_angel205=-1,can1_last_motor_angel206=-1;

CanRxMsg rx_message;
#define CAN_ID_MASK		0x07


//CAN1_Mode_Init
//���ܣ�CAN1��ʼ������
//����������tsjw,tbs2,tbs1,brp,mode ����CAN1��ģʽ�Ͳ�����
//����ֵ��0
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
  	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
  	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
   	NVIC_InitTypeDef  		NVIC_InitStructure;

    //ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��PORTAʱ��	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
	
    //��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��PA11,PA12
	
	//���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1); //GPIOD0����ΪCAN1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1); //GPIOD1����ΪCAN1
	  	  
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=ENABLE;//	DISABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= mode;	 //ģʽ���� 
  	CAN_InitStructure.CAN_SJW=tsjw;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 
    
	//���ù�����
 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
		
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
	return 0;
}
	    
void CAN1_RX0_IRQHandler(void)
{   
	static uint8_t id;
	static float delta;
	
	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET) 
	{
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
		CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
		
		if(rx_message.StdId==0x205){
			//ת�٣�RPM��
			can1_current_motor_speed205 = (rx_message.Data[2]<<8) | rx_message.Data[3];
			//ת�ص���
			can1_current_motor_current205 = (rx_message.Data[4]<<8) | rx_message.Data[5];
			//����ֵ
			can1_current_motor_angel205	= (rx_message.Data[0]<<8) | rx_message.Data[1];
			//�Ƕȼ���
			if(can1_last_motor_angel205!=-1){
				delta = can1_current_motor_angel205-can1_last_motor_angel205;
				delta += (delta>4096)?-8192:((delta<-4096)?8192:0);
				can1_current_angel205 += delta/19.f;
			}
			can1_last_motor_angel205 = can1_current_motor_angel205;
		}
		else if(rx_message.StdId==0x206){
			//ת�٣�RPM��
			can1_current_motor_speed206 = (rx_message.Data[2]<<8) | rx_message.Data[3];
			//ת�ص���
			can1_current_motor_current206 = (rx_message.Data[4]<<8) | rx_message.Data[5];
			//����ֵ
			can1_current_motor_angel206	= (rx_message.Data[0]<<8) | rx_message.Data[1];
			//�Ƕȼ���
			if(can1_last_motor_angel206!=-1){
				delta = can1_current_motor_angel206-can1_last_motor_angel206;
				delta += (delta>4096)?-8192:((delta<-4096)?8192:0);
				can1_current_angel206 += delta/19.f;
			}
			can1_last_motor_angel206 = can1_current_motor_angel206;
		}else if(rx_message.StdId==CAN1_MESSAGE_ID_BOTTOM){
			//���յײ������ź�
			if(((rx_message.Data[0])&0x03)==rx_message.Data[0]){
				for(id=0;id<rx_message.DLC;id++){
					Remote_FromBottomData[id] = rx_message.Data[id];
				}
			}else{
				disp1[7]++;
			}
		}
	}
}


/***************************************************************************************
 *Name     : CAN1_Cmd_Bottom
 *Function ��CAN1���̿��Ƴ���
 *Input    ��Speed A B C D ������ĸ������ֵ
 *Output   ���� 
 *Description : �����ĸ����̵���˶�
****************************************************************************************/
void CAN1_Send_Bottom(int16_t candata1,int16_t candata2,int16_t candata3,int16_t candata4)
{
	CanTxMsg TxMessage;
	TxMessage.StdId=0x200;	           			// ��׼��ʶ��Ϊ0x200
	TxMessage.ExtId=0x00;	             		// ������չ��ʾ����29λ��
	TxMessage.IDE=CAN_ID_STD;					// ʹ�ñ�׼֡ģʽ
	TxMessage.RTR=CAN_RTR_DATA;				 	// ���͵�������
	TxMessage.DLC=8;							// ���ݳ���Ϊ2�ֽ�
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
 *Name     : 
 *Function ��
 *Input    ��
 *Output   ���� 
 *Description : 
****************************************************************************************/
void CAN1_Rotate(int16_t candata1, int16_t candata2)
{
	CanTxMsg tx_message;
	tx_message.StdId=0x1ff;	           			// ��׼��ʶ��Ϊ0x20 0
	tx_message.IDE=CAN_ID_STD;					// ʹ�ñ�׼֡ģʽ
	tx_message.RTR=CAN_RTR_DATA;				// ���͵�������
	tx_message.DLC=8;							// ���ݳ���Ϊ2�ֽ�
	tx_message.Data[0] = (uint8_t)(candata1 >> 8);
	tx_message.Data[1] = (uint8_t)candata1;
	tx_message.Data[2] = (uint8_t)(candata2 >> 8);
	tx_message.Data[3] = (uint8_t)candata2;
	tx_message.Data[4] = 0;
	tx_message.Data[5] = 0;
	tx_message.Data[6] = 0;
	tx_message.Data[7] = 0;
    
	CAN_Transmit(CAN1,&tx_message);
}

/***************************************************************************************
 *Name     : 
 *Function ��
 *Input    ��
 *Output   ���� 
 *Description : ֻ��8���ֽ�
****************************************************************************************/
void CAN1_SendMessage(uint8_t len, uint8_t *data)
{
	CanTxMsg tx_message;
	tx_message.StdId=CAN1_MESSAGE_ID_UPPER;	   	// ��׼��ʶ��
	tx_message.IDE=CAN_ID_STD;					// ʹ�ñ�׼֡ģʽ
	tx_message.RTR=CAN_RTR_DATA;				// ���͵�������
	tx_message.DLC=(len>8)?8:len;				// ���ݳ���
	for(uint8_t i=0;i<len;i++){
		tx_message.Data[i] = data[i];
	}    
	CAN_Transmit(CAN1,&tx_message);
}


