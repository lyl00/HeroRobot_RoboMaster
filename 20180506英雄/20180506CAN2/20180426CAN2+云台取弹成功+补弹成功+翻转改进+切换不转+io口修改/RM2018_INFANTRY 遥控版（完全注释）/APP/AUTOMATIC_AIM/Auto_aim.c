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
	
	if(unpackFrame(rx_buf,len,&fram) == 0)//����ɹ�
	{	
		if(fram.timestamp != frame_ex.timestamp)//���ǰһ֡���ݺ͵�ǰ֡ʱ���һ��,Ŀ�궪ʧ,����Ϊ
			{
					view_ch2 = parameter_yaw*fram.yaw;//����ϵ��parameter_yaw������
					view_ch3 = parameter_pitch*fram.pitch;//����ϵ��parameter_pitch������
			}
				frame_ex.timestamp = fram.timestamp;
		
	}
	else{                            //������ɹ�,����
		   view_ch2 = 0;
			 view_ch3 = 0;
		   return ;
	}
}
