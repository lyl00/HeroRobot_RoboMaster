#ifndef __USART3_H__
#define __USART3_H__

#include "main.h"

#define MAXLBUF 100

extern u8  USART_RX_BUF3[MAXLBUF];  

void USART3_DMA_Init(void);
void UART3_Send(u8 *buffer, u8 len);

#endif
