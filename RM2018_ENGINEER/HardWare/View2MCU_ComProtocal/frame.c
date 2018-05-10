#include <stdio.h>
#include <string.h>
#include "define.h"
#include "frame.h"
#include "crc_table.h"
//created by sui qi
//modified by twb 2017.7.18 add send module
u8 sumCheck(u8 *buff,u16 len)
{
    u16 i=0;
    u32 sum=0;
    for(i=0;i<len;i++)
        sum+=buff[i];
    return (u8)sum&0x000000ff;
}

u8 crc8Check(u8 *buff,u16 len)
{
	u8 ucIndex,ucCRC8=(u8)CRC8_INIT;
	while (len--)
	{
		ucIndex = ucCRC8^(*buff++);
		ucCRC8 = CRC8_Table[ucIndex];
	}
	return(ucCRC8);
}

u16 crc16Check(u8 *buff,u32 len)
{
	u8 chData;
	u16 wCRC=(u16)CRC16_INIT;
	if(buff == NULL)
	{
		return 0xFFFF;
	}
	while(len--)
	{
		chData = *buff++;
		(wCRC) = ((u16)(wCRC) >> 8) ^ CRC16_Table[((u16)(wCRC) ^ (u16)(chData)) &0x00ff];
	}
	return wCRC;
}

int unpackFrame(u8 *buff,u16 len,frame *fram)
{	
	u16 i=0;
	while(buff[i]!=0xff||buff[i+1]!=0xff)	
	{
		i++;
		if(i>(len-sizeof(frame)))
			return 1;
	}	
	memcpy(fram,(buff+i),sizeof(frame));
	if(fram->crc8check==crc8Check((u8 *)fram,sizeof(frame)-1))
		return 0;
	else
		return 2;
}

void packFrame(u8 *buff,frame *fram)
{	
	memcpy(buff,fram,sizeof(frame));
	buff[sizeof(frame)-1] = crc8Check((u8 *)fram,sizeof(frame)-1);
}








