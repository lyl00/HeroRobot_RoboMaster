#include "define.h"
#ifndef __FRAME
#define __FRAME	 
//modified by sui qi
//modified by twb 2017
#pragma  pack(1) 
typedef struct _frame				
{
	u8 head[2];		//0xff 0xff
	u32 timestamp;
	float yaw;	
	float pitch;
	u32 extra[2]; //additional imformation	
	u8 crc8check;							
}frame;											
#pragma  pack() 
/*
#pragma  pack(1) 
typedef struct _frame_send				
{
	u8 head[2];	
	u32 timestamp;

	s16 v_x;
	s16 v_y;
	s16 v_w;											  									
	s16 target_x;//undermined
	s16 target_y;//undermined
	s16 target_w;//undermined		
	float platform_delta_yaw;
	float platform_delta_pitch;
	u8 cmd0;
	u8 cmd1;
	u8 cmd2;
	u8 cmd3;	
	u8 crc8check;							
}frame_send;											
#pragma  pack() 
*/
u8 sumCheck(u8 *buff,u16 len);
u8 crc8Check(u8 *buff,u16 len);
u16 crc16Check(u8 *buff,u32 len);
/*
*return 0: success
*return 1: no frame in buff
*return 2: crc8 error
*/
int unpackFrame(u8 *buff,u16 len,frame *fram);
void packFrame(u8 *buff,frame *fram);

#endif
