#include "IIC.h"
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//IIC���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
 
//��ʼ��IIC
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 |  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
  GPIO_Init(GPIOF, &GPIO_InitStructure);
}
void SDA_OUT(void)
{
    GPIO_InitTypeDef   gpio;
    gpio.GPIO_Pin = GPIO_Pin_9;  
		gpio.GPIO_Mode = GPIO_Mode_OUT;
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_Init(GPIOF, &gpio);
}

void SDA_IN(void)
{
    GPIO_InitTypeDef   gpio; 
		gpio.GPIO_Pin = GPIO_Pin_9;  
    gpio.GPIO_Mode = GPIO_Mode_IN; 
	
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(GPIOF, &gpio);
}

//����IIC��ʼ�ź�
unsigned char IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;
   if(!READ_SDA){
		printf("error 1!\r\n");
		return 0xff;
	}	
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	if(READ_SDA){
		printf("error 2!\r\n");
		return 0xff;
	}
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
	return 0;
}	  

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
int IIC_WaitAck(void)
{
	u16 Out_Time=0;  //??2000
  SDA_IN();
  IIC_SCL=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	while(READ_SDA)
	{
		Out_Time++;
		if((Out_Time)>50)
		{
			IIC_Stop();
			
      printf("%d %derror 2A\r\n",Out_Time,(int)READ_SDA);
      return 0xff;
		}
		delay_us(2);
	}
	IIC_SCL=0;
  return 0;
}

//����ACKӦ��
void IIC_Ack(u8 re)					     
{
	IIC_SCL=0;
	SDA_OUT();
	delay_us(2);
	if(re)
	   IIC_SDA=1;
	else
	   IIC_SCL=0;
	delay_us(2);
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}	

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_WriteBit(u8 Temp)
{
	u8 i;
	SDA_OUT();
	IIC_SCL = 0;
	for(i=0;i<8;i++)
	{
		if(Temp&0x80)
		{
			IIC_SCL = 1;
		}
		else
		{
			IIC_SCL = 0;
		}
		Temp<<=1;
		delay_us(2);
		IIC_SCL = 1;
		delay_us(2);
		IIC_SCL = 0;
		delay_us(2);
	}
}

u8 IIC_ReadBit(void)
{
	u8 i,Temp=0;
	SDA_IN();
	delay_us(2);
	for(i=0;i<8;i++)
	{
		IIC_SCL = 0;
		delay_us(2);
		IIC_SCL = 1;
		Temp<<=1;
		if(READ_SDA)
		   Temp++;
		delay_us(2);
	}
	IIC_SCL = 0;
	return Temp;
}

//д���ݣ��ɹ�����0��ʧ�ܷ���0xff
int IIC_WriteData(u8 dev_addr,u8 reg_addr,u8 data)
{
	if(IIC_Start()==0xff){
		printf("error 3!");
		return 0xff;
	}
    
	IIC_WriteBit(dev_addr);
	if(IIC_WaitAck() == 0xff)
    {
        printf("error 4\r\n");
        return 0xff;
    }
    
	IIC_WriteBit(reg_addr);
	if(IIC_WaitAck() == 0xff)
    {
        printf("error 5\r\n");
        return 0xff;
    }

    IIC_WriteBit(data);
    if(IIC_WaitAck() == 0xff)
    {
        printf("error 6\r\n");
        return 0xff;
    }

	IIC_Stop();
    return 0;
}

//�����ݣ��ɹ�����0��ʧ�ܷ���0xff
int IIC_ReadData(u8 dev_addr,u8 reg_addr,u8 *pdata,u8 count)
{
	u8 i;

  if(IIC_Start()==0xff){
		printf("error 7\r\n");
    return 0xff;
	}
	
  IIC_WriteBit(dev_addr);
	if(IIC_WaitAck() == 0xff)
    {
        printf("error 8\r\n");
        return 0xff;
    }
    
  IIC_WriteBit(reg_addr);
	if(IIC_WaitAck() == 0xff)
    {
        printf("error 9\r\n");
        return 0xff;
    }

		
  IIC_Start();
	
  IIC_WriteBit(dev_addr+1);
	if(IIC_WaitAck() == 0xff)
    {
        printf("error 2H\r\n");
        return 0xff;
    }
    
    for(i=0;i<(count-1);i++)
    {
			  
        *pdata=IIC_ReadBit();
        IIC_Ack(0);
        pdata++;
    }

    *pdata=IIC_ReadBit();
    IIC_Ack(1); 
    
    IIC_Stop(); 
    
    return 0;    
}


