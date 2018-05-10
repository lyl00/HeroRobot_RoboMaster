/************************************************************
 *File		:	CAN2.c
 *Author	:  	@YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com, @YangTao,ybb331082@126.com
 *Version	: 	V1.0
 *Update	: 	2018.04.15
 *Description: 	Use CAN2 to control motor
				bound 1Mbps
				PB13(tx) PB12(rx)
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


#define CAN2_ID_MASK		0x07


float 				can2_current_angel[CAN2_MOTOR_COUNT]={0.f,0.f,0.f,0.f};
volatile int16_t 	can2_current_motor_current[CAN2_MOTOR_COUNT];
volatile int16_t 	can2_current_motor_speed[CAN2_MOTOR_COUNT];
volatile uint16_t 	can2_stuckflag[CAN2_MOTOR_COUNT];		//���ɿ�λ���

// Inner parameters
CanRxMsg can2_rx_message;
volatile int16_t 	can2_current_motor_angel[CAN2_MOTOR_COUNT];
volatile int16_t 	can2_last_motor_angel[CAN2_MOTOR_COUNT]={-1,-1,-1,-1};

//CAN2_Mode_Init
//���ܣ�CAN2��ʼ������
//����������tsjw,tbs2,tbs1,brp,mode ����CAN2��ģʽ�Ͳ�����
//����ֵ��0
u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
  	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
  	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
   	NVIC_InitTypeDef  		NVIC_InitStructure;

    //ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//ʹ��CAN2ʱ��	
	
    //��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��PA11,PA12
	
	//���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2); //GPIOB12����ΪCAN2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2); //GPIOB13����ΪCAN2
	  	  
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
  	CAN_Init(CAN2, &CAN_InitStructure);   // ��ʼ��CAN2 
    
	//���ù�����
 	CAN_FilterInitStructure.CAN_FilterNumber=14;	  //������1
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO1;//������0������FIFO1
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������1
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	
	CAN_ITConfig(CAN2,CAN_IT_FMP1,ENABLE);//FIFO1��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // �����ȼ�Ϊ2
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
			//ת�٣�RPM��
			can2_current_motor_speed[id-1] = (can2_rx_message.Data[2]<<8) | can2_rx_message.Data[3];
			//ת�ص���
			can2_current_motor_current[id-1] = (can2_rx_message.Data[4]<<8) | can2_rx_message.Data[5];
			//����ֵ
			can2_current_motor_angel[id-1]	= (can2_rx_message.Data[0]<<8) | can2_rx_message.Data[1];
			//�Ƕȼ���
			if(can2_last_motor_angel[id-1]!=-1){
				delta = can2_current_motor_angel[id-1]-can2_last_motor_angel[id-1];
				delta+=(delta>4096)?-8192:((delta<-4096)?8192:0);
				//203 204
				if(id==3 || id==4)
					can2_current_angel[id-1] -= delta/19.f;
				else
					can2_current_angel[id-1] += delta/19.f;
				//��¼�������Ƿ����һ��ֵ��������Ϊû���ƶ�
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
 *Function ��Send data for 201 202 203 204
 *Input    ����
 *Output   ���� 
 *Description : ,int16_t candata2,int16_t candata3,int16_t candata4)
****************************************************************************************/
void CAN2_Motor(int16_t candata1, int16_t candata2, int16_t candata3, int16_t candata4)
{
	CanTxMsg tx_message;
	tx_message.StdId=0x200;	           			// ��׼��ʶ��Ϊ0x200
	tx_message.ExtId=0x00;	             		// ������չ��ʾ����29λ��
	tx_message.IDE=CAN_ID_STD;					// ʹ�ñ�׼֡ģʽ
	tx_message.RTR=CAN_RTR_DATA;				// ���͵�������
	tx_message.DLC=8;							// ���ݳ���Ϊ2�ֽ�
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

