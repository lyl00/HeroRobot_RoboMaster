#include "main.h"

frame frame_ex;

int frame_count=0;
int parameter_yaw=1;
int parameter_pitch=1;

void Auto_aim(u8 *rx_buf,int len,frame *fram)
{
	if(unpackFrame(rx_buf,len,fram)==0)//����ɹ�
	{	
		if(frame_count<10) //ǰ10֡�����Ȳ�Ҫ(����10������)
			{
				frame_ex.timestamp=fram->timestamp;
				frame_ex.yaw=fram->yaw;
				frame_ex.pitch=fram->pitch;
				frame_count++;
				GimbalStop();
			}
		else{
			    if(fram->timestamp!=frame_ex.timestamp)//���ǰһ֡���ݺ͵�ǰ֡ʱ���һ��,Ŀ�궪ʧ,����Ϊ
					{
							GimbalControl_YawAngel(parameter_yaw*fram->yaw);//����ϵ��parameter_yaw������
							GimbalControl_Pitch(MIDDLE_PITCH+parameter_pitch*fram->pitch);//����ϵ��parameter_pitch������
					}
			}
	}
	else{                            //������ɹ�,����
		   GimbalStop();
		   return ;
	}
	
}
