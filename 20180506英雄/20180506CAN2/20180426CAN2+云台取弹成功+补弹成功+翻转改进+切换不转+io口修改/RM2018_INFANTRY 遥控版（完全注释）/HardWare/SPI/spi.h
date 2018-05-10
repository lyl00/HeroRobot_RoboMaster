#ifndef SPI_H
#define SPI_H
#include "sys.h"

#define SPI5_NS PFout(6)

void SPI5Init(void);
unsigned char SPI5ReadWriteByte(unsigned char TxData);
unsigned char MPU6500ReadSPISingle(unsigned char reg);
unsigned char MPU6500ReadSPI(unsigned char reg,unsigned char *buf,unsigned char len);
unsigned char MPU6500WriteSPI(unsigned char reg,unsigned char data);
	
#endif
