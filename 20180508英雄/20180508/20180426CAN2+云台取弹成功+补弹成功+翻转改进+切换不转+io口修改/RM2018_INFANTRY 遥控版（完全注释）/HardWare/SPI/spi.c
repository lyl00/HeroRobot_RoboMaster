#include "main.h"

void SPI5Init(void){
	
	unsigned short temp=0;
	GPIO_InitTypeDef gpio;	//从开启时钟下面挪上来这里，不然会报错
	RCC->AHB1ENR|=1<<5;
	RCC->APB2ENR|=1<<20;
		

	gpio.GPIO_Pin = GPIO_Pin_7 |  GPIO_Pin_8|  GPIO_Pin_9  ;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOF, &gpio);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource7,5); 
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,5);
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,5);

	gpio.GPIO_Pin = GPIO_Pin_6  ;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF, &gpio);
	PFout(6)=1;
	
	RCC->APB2RSTR|=1<<20;	//复位SPI5
	RCC->APB2RSTR&=~(1<<20); //停止复位SPI5
	temp|=0<<10;	//全双工模式	
	temp|=1<<2;	  //SPI主机  
	temp|=1<<8;
	temp|=0<<11;  //8位数据格式	
	temp|=1<<1;   //空闲模式下SCK为1 CPOL=1 
	temp|=1<<0;   //数据采样从第2个时间边沿开始,CPHA=1  
	temp|=1<<9;   //软件nss管理
	temp|=7<<3;   //Fsck=Fpclk/256
	temp|=0<<7;   //MSB First  
	temp|=1<<6;   //SPI启动 
	SPI5->CR1=temp;
	SPI5->I2SCFGR&=~(1<<11);//选择SPI模式
}

unsigned char SPI5ReadWriteByte(unsigned char TxData){
	unsigned short retry=0;
	while((SPI5->SR&1<<1)==0){
		retry++;
		if(retry>600){
			return 0;
		}
	}
	SPI5->DR=TxData;
	retry=0;
	while((SPI5->SR&1<<0)==0){
		retry++;
		if(retry>600){
			return 0;
		}
	}
	return SPI5->DR;
}

