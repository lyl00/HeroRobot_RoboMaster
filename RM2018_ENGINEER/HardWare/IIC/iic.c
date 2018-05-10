#include "IIC.h"
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//IIC驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
 
//初始化IIC
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

//产生IIC起始信号
unsigned char IIC_Start(void)
{
	SDA_OUT();     //sda线输出
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
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
	return 0;
}	  

//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
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

//产生ACK应答
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
//不产生ACK应答		    
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

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
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

//写数据，成功返回0，失败返回0xff
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

//读数据，成功返回0，失败返回0xff
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


