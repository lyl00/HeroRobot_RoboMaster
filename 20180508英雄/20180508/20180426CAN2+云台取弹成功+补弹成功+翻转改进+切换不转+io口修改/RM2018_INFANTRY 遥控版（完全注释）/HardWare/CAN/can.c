/************************************************************
 *File		:	can1.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
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

CanRxMsg rx_message;
CanRxMsg rx_message2;

volatile int16_t current_position_206;    	//pitch gimbal
volatile int16_t current_position_205;			//yaw gimbal
volatile int16_t current_cm_201;
volatile int16_t current_cm_202;
volatile int16_t current_cm_203;
volatile int16_t current_cm_204;




volatile int16_t can2_current_speed201=0;//CAN2�ĵ�����ٶȷ���ֵ,201̧�������202��ת���
volatile int16_t can2_current_speed202=0;
volatile int16_t can2_current_speed203=0;
volatile int16_t can2_current_speed204=0;
volatile int16_t can2_current_angle201=0;//CAN2�ĵ���Ļ�е�Ƕȷ���ֵ
volatile int16_t can2_current_angle202=0;
volatile int16_t can2_current_angle203=0;
volatile int16_t can2_current_angle204=0;
volatile int16_t can2_last_angle201=-1;//CAN2�ĵ���Ļ�е�Ƕȷ���ֵ
volatile int16_t can2_last_angle203=-1;
volatile int16_t can2_last_angle202=-1;
volatile int16_t can2_last_angle204=-1;

float current_angle_201=0,current_angle_203=0,current_angle_202=0,current_angle_204=4000;
int16_t delta;
int timell=0;





/*------------����Ϊ��������-----------*/





//CAN1_Mode_Init
//���ܣ�CAN1��ʼ������
//����������tsjw,tbs2,tbs1,brp,mode ����CAN1��ģʽ�Ͳ�����
//����ֵ��0
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
   	NVIC_InitTypeDef  NVIC_InitStructure;

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

u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
   	NVIC_InitTypeDef  NVIC_InitStructure;

    //ʹ�����ʱ��
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��PORTBʱ��	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//ʹ��CAN2ʱ��	
	
    //��ʼ��GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��PB12,PB13
	
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
 	  CAN_FilterInitStructure.CAN_FilterNumber=14;	  //������14//�ο����Ͻ���ĳɹ�����14������û��
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO1;//������14������FIFO1
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������1
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
		
	  CAN_ITConfig(CAN2,CAN_IT_FMP1,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;            // �����ȼ�Ϊ0
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
                
        if(rx_message.StdId == 0x206)           //206 ��pitch
        {             
             current_position_206 = (rx_message.Data[0]<<8) | rx_message.Data[1];	
					
        }
        else if(rx_message.StdId == 0x205)      //205 ��YAW��            
        { 
             current_position_205 = (rx_message.Data[0]<<8) | rx_message.Data[1];
        }
				else if(rx_message.StdId == 0x207)      //�����ֵ��           
        { 	
						//current_shoot_speed_207 = (rx_message.Data[2]<<8) | rx_message.Data[3];		
					/*----------����-----------*/
						//current_shoot_pos_207=(rx_message.Data[0]<<8)|rx_message.Data[1];//��ȡ�����̵���Ļ�е�Ƕ�
        }
				else if(rx_message.StdId == 0x201)      //��ǰ          
        { 
             current_cm_201 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
        }
				else if(rx_message.StdId == 0x202)       //��ǰ         
        { 
             current_cm_202 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
        }
				else if(rx_message.StdId == 0x203)       //�Һ�        
        { 
             current_cm_203 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
        }
				else if(rx_message.StdId == 0x204)       //���        
        { 
             current_cm_204 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
        }
				if(timell%500==1){
//			printf("PITCH����0:%d\r\n",current_position_206);
//	    printf("Yaw����:%d\r\n",current_position_205);
				}timell++;
	}
}

void CAN2_RX1_IRQHandler(void)
{
	 if (CAN_GetITStatus(CAN2,CAN_IT_FMP1)!= RESET) 
		{
				CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);
        CAN_Receive(CAN2, CAN_FIFO1, &rx_message2);       
               
				if(rx_message2.StdId == 0x201)      //̧�����          
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
//						printf("ok\n");//��һ�����������һ���ܷ�ִ��
				}
				else if(rx_message2.StdId == 0x202)       //��ת���1         
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
				else if(rx_message2.StdId == 0x203)       //̧�����2        
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
				else if(rx_message2.StdId == 0x204)       //���       
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
 *Function ��CAN1���̿��Ƴ���
 *Input    ��Speed A B C D ������ĸ������ֵ
 *Output   ���� 
 *Description : �����ĸ����̵���˶�
****************************************************************************************/
void CAN1_Send_Bottom(int16_t candata1,int16_t candata2,int16_t candata3,int16_t candata4)
{	
	CanTxMsg TxMessage;
  TxMessage.StdId=0x200;	           // ��׼��ʶ��Ϊ0x200
  TxMessage.ExtId=0x00;	             // ������չ��ʾ����29λ��
  TxMessage.IDE=CAN_ID_STD;					 // ʹ�ñ�׼֡ģʽ
  TxMessage.RTR=CAN_RTR_DATA;				 // ���͵�������
  TxMessage.DLC=8;							     // ���ݳ���Ϊ2�ֽ�
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
 *Function ��CAN1�����������
 *Input    ����
 *Output   ���� 
 *Description : �������Ʋ����ֵ��
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
  TxMessage.StdId=0x200;	           // ��׼��ʶ��Ϊ0x200
  TxMessage.ExtId=0x00;	             // ������չ��ʾ����29λ��
  TxMessage.IDE=CAN_ID_STD;					 // ʹ�ñ�׼֡ģʽ
  TxMessage.RTR=CAN_RTR_DATA;				 // ���͵�������
  TxMessage.DLC=8;							     // ���ݳ���Ϊ2�ֽ�
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
 *Function ����̨���У׼
 *Input    ����
 *Output   ���� 
 *Description : �µ�����ν���ʱ��У׼
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
 *Function ��CAN1��̨������Ƴ���
 *Input    ������pitch��yaw�������ֵ
 *Output   ���� 
 *Description : ������̨�˶�������pitch�ᡢyaw��Ͳ�����
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





