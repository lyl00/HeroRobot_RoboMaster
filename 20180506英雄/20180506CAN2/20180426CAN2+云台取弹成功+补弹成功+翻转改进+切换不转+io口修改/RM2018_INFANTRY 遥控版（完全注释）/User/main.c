/************************************************************
 *File		:	main.c
  *Author	:  @YangTianhao ,490999282@qq.com£¬@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	
                RM2018_INFANTRY project. 
							
							- Control the Chassis motors , Gimbal motors , Shoot motor BY Remote controller.
							-	Using CAN bus to connect all 7 motors(Yaw,pitch,motorA,motorB,motorC,motorD,ShootMotor) made by DJI.
							-	Manage the power of chassis by electricity module.
							- Control the friction wheel motors by electronic governors.
								
								Instruction:
								
              - Turn on the controller,and switch on the power,you will hear the buzzer ring a few,that means the mechine is initializing.						
							- When the buzzer is silent again,which means the mechine is ready to move,you have the permission to control it.
							-	Channel 0 1 2 3 corresponds to Translation crosswise,Translation lengthways,Pitching and Rotate respectively.
							- Switching s1 from middle position to up position then switch back will turn on the friction wheels,and do that again will 
							  turn off them.When switch to down position,the gun shoots.
							- Three positions of s2 corresponds to Remote pattern,Key pattern and Standby pattern respectively from up to down.
                							
								
						                                                            
 					       ___	  ______                             					     	______    ___ 
					      ||_||	 | __   |______                               _____|   __ |  ||_||
					       |_|___|______|------          *********            -----|______|___| |   
                _|_|_  TongJi  _\_ _				                        _ _/_  TongJi  _|_|_	
				       / \ / \___RM___/ \ / \                              / \ / \___RM___/ \ / \
				      ||- + -||      ||- + -||                            ||- + -||      ||- + -|| 
				       \_/_\_/	      \_/_\_/                              \_/_\_/	      \_/_\_/ 
					   
						 
        
  --- All rights reserved by TongJi RoboMaster Club(TJRM).
	
	School of Mechanical Engineering Tongji University,
	No.4800,CaoYang Road,JiaDing District,
	Shanghai City,P.R.China,201804
 ************************************************************/
 
#include "main.h" 

int main(void)
{ 
	  //ÅäÖÃµ×²ãÇý¶¯
 		BSP_Init();	
//	frame frame1;
//	//frame unpack_fream;
//	
//	unsigned char buff[100];
//	frame1.head[0]=0xff;
//	frame1.head[1]=0xff;
//	frame1.yaw = 100;
//	frame1.pitch = 200;
//	
//	int flag=0;
//	int a=0,b=0;
    while(1)
	  {	
			//flag++;
//			
//			if(unpackFrame(USART_RX_BUF3,100,&unpack_fream)==0)
//				a++;
//			else
//				b++;
//			
//			if(a+b>100){
//				printf("rate:%f\r\n",(float)b/(a+b));
//				a=0;
//				b=0;
//			}
			
			//printf("Gimbal_control.angle_yaw_current:%f\r\n",	Gimbal_control.angle_yaw_current);
			//printf("Gimbal_control.angle_pitch_current:%f\r\n",	Gimbal_control.angle_pitch_current);
			
		//	printf("current_position_206:%d\r\n",	current_position_206);
			//printf("current_position_205:%d\r\n",	current_position_205);
 
//		  printf("unpack_fream:%f\r\n",unpack_fream.yaw);
//			printf("unpack_fream:%f\r\n",unpack_fream.yaw);
			//printf("%s\r\n",USART_RX_BUF3);
//				printf("view_ch2:%f\r\n",view_ch2);
//				printf("view_ch3:%f\r\n",view_ch3);
//			  printf("Gimbal_control.angle_yaw_set:%f\r\n",Gimbal_control.angle_yaw_set);
//				printf("Gimbal_control.angle_pitch_set:%f\r\n",Gimbal_control.angle_pitch_set);
//			if(flag%3==0)
//			{
//			packFrame(buff,&frame1);
//			UART3_Send(buff,100);
//			}
				delay_ms(10);
					
		}
}
