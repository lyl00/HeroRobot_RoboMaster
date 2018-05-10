/************************************************************
 *File		:	mpu6050_i2c.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com	@YangTianhao ,490999282@qq.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Use i2c1 to read Mpu6050
								Be careful of the IIC_Delay() function. 
 ************************************************************/
/*
	iic底层程序 
	需要考虑IIC_Delay()函数中的延时值，来达到更好的通信效果
*/
#include "mpu6050_i2c.h"
#include "usart6.h"	
#include "sys.h"



/*----I2C1----SCL----PB6---PB8*/
/*----I2C1----SDA----PB7---PB9*/

#define IIC_SCL_H()      GPIO_SetBits(GPIOF,GPIO_Pin_1)
#define IIC_SCL_L()      GPIO_ResetBits(GPIOF,GPIO_Pin_1)
#define IIC_SDA_H()      GPIO_SetBits(GPIOF,GPIO_Pin_0)
#define IIC_SDA_L()      GPIO_ResetBits(GPIOF,GPIO_Pin_0)
#define IIC_SDA_Read()   GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_0)

void IIC_Delay(unsigned int t)
{
	int i;
	for( i=0;i<t;i++)
	{
		int a = 15;				//假如通信不通，需要修改IIC的延时
		while(a--);
	}
}

void HEAT_Configuration(void)
{
	GPIO_InitTypeDef gpio;   

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		
	gpio.GPIO_Pin = GPIO_Pin_4;	
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_OD;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &gpio);
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);//0
}

void IIC_GPIO_Init(void)
{
    GPIO_InitTypeDef   gpio;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
		gpio.GPIO_Pin = GPIO_Pin_1 |  GPIO_Pin_0  ;
		gpio.GPIO_Mode = GPIO_Mode_OUT;
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_Init(GPIOF, &gpio);
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
		gpio.GPIO_Pin = GPIO_Pin_8  ;
		gpio.GPIO_Mode = GPIO_Mode_IN;
		//gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOF, &gpio);
		PFout(8)=0;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
		gpio.GPIO_Pin = GPIO_Pin_6  ;
		gpio.GPIO_Mode = GPIO_Mode_IN;
    //gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOF, &gpio);
		PFout(6)=1;
}

void IIC_SDA_Out(void)
{
    GPIO_InitTypeDef   gpio;
    gpio.GPIO_Pin = GPIO_Pin_0;  
		gpio.GPIO_Mode = GPIO_Mode_OUT;
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_Init(GPIOF, &gpio);
}

void IIC_SDA_In(void)
{
    GPIO_InitTypeDef   gpio; 
		gpio.GPIO_Pin = GPIO_Pin_0;  
    gpio.GPIO_Mode = GPIO_Mode_IN; 
	
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(GPIOF, &gpio);
}

unsigned char IIC_Start(void)								  
{
	IIC_SDA_Out();
	IIC_SDA_H();
	if(!IIC_SDA_Read()){
		printf("error 1!\r\n");
		return 0xff;
	}
	IIC_SCL_H();
	IIC_Delay(1);
	IIC_SDA_L();
	if(IIC_SDA_Read()){
		printf("error 2!\r\n");
		return 0xff;
	}
	
	IIC_Delay(1);
	IIC_SCL_L();
	return 0;
}

void IIC_Stop(void)
{
	IIC_SDA_Out();
	IIC_SCL_L();	
	IIC_SDA_L();
	IIC_Delay(1);
	IIC_SCL_H();
	IIC_Delay(1);
	IIC_SDA_H();
}

void IIC_Ack(u8 re)					     
{
	IIC_SCL_L();
	IIC_SDA_Out();
	IIC_Delay(1);
	if(re)
	   IIC_SDA_H();
	else
	   IIC_SDA_L();
	IIC_Delay(1);
	IIC_SCL_H();
	IIC_Delay(1);
	IIC_SCL_L();
}

int IIC_WaitAck(void)
{
	u16 Out_Time=0;  //??2000
  IIC_SDA_In();
  IIC_SDA_H();
	IIC_Delay(1);
	IIC_SCL_H();
	IIC_Delay(1);
	while(IIC_SDA_Read())
	{
		Out_Time++;
		if((Out_Time)>50)
		{
			IIC_Stop();
			
      printf("%d %derror 2A\r\n",Out_Time,IIC_SDA_Read());
      return 0xff;
		}
		IIC_Delay(5);
	}
	IIC_SCL_L();
  return 0;
}

void IIC_WriteBit(u8 Temp)
{
	u8 i;
	IIC_SDA_Out();
	IIC_SCL_L();
	for(i=0;i<8;i++)
	{
		if(Temp&0x80)
		{
			IIC_SDA_H();
		}
		else
		{
			IIC_SDA_L();
		}
		Temp<<=1;
		IIC_Delay(1);
		IIC_SCL_H();
		IIC_Delay(1);
		IIC_SCL_L();
		IIC_Delay(1);
	}
}

u8 IIC_ReadBit(void)
{
	u8 i,Temp=0;
	IIC_SDA_In();
	IIC_Delay(1);
	for(i=0;i<8;i++)
	{
		IIC_SCL_L();
		IIC_Delay(2);
		IIC_SCL_H();
		Temp<<=1;
		if(IIC_SDA_Read())
		   Temp++;
		IIC_Delay(2);
	}
	IIC_SCL_L();
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
