#ifndef __MPU6050_H__
#define __MPU6050_H__

//#include <stm32f4xx.h>
#include "stm32f4xx.h"
//#include "sys.h"


void IIC_GPIO_Init(void);
int IIC_WriteData(u8 dev_addr,u8 reg_addr,u8 data);
int IIC_ReadData(u8 dev_addr,u8 reg_addr,u8 *buf,u8 len);

#endif
