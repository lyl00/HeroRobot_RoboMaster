#include "main.h"

frame frame_ex;

int frame_count=0;
int parameter_yaw=1;
int parameter_pitch=1;

void Auto_aim(u8 *rx_buf,int len,frame *fram)
{
	if(unpackFrame(rx_buf,len,fram)==0)//解包成功
	{	
		if(frame_count<10) //前10帧数据先不要(参数10待检验)
			{
				frame_ex.timestamp=fram->timestamp;
				frame_ex.yaw=fram->yaw;
				frame_ex.pitch=fram->pitch;
				frame_count++;
				GimbalStop();
			}
		else{
			    if(fram->timestamp!=frame_ex.timestamp)//如果前一帧数据和当前帧时间戳一样,目标丢失,不作为
					{
							GimbalControl_YawAngel(parameter_yaw*fram->yaw);//比例系数parameter_yaw待检验
							GimbalControl_Pitch(MIDDLE_PITCH+parameter_pitch*fram->pitch);//比例系数parameter_pitch待检验
					}
			}
	}
	else{                            //解包不成功,返回
		   GimbalStop();
		   return ;
	}
	
}
