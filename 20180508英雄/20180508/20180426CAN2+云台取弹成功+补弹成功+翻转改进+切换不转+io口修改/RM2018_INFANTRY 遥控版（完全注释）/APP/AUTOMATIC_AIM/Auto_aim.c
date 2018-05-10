#include "main.h"

frame frame_ex;
frame fram;
int frame_count=0;
float parameter_yaw=1;
float parameter_pitch=1;
float view_ch2;
float view_ch3;

void Auto_aim(u8 *rx_buf,int len)
{
	
	if(unpackFrame(rx_buf,len,&fram) == 0)//解包成功
	{	
		if(fram.timestamp != frame_ex.timestamp)//如果前一帧数据和当前帧时间戳一样,目标丢失,不作为
			{
					view_ch2 = parameter_yaw*fram.yaw;//比例系数parameter_yaw待检验
					view_ch3 = parameter_pitch*fram.pitch;//比例系数parameter_pitch待检验
			}
				frame_ex.timestamp = fram.timestamp;
		
	}
	else{                            //解包不成功,返回
		   view_ch2 = 0;
			 view_ch3 = 0;
		   return ;
	}
}
