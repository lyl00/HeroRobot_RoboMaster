/************************************************************
 *File		:	main.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	RM2018_INFANTRY project. 
								Control the CM (Chassis motor) , Gimbal motor , Shoot motor BY Remote controller.
								USB CAN to connect motor made by DJI.
 ************************************************************/
 
#include "main.h"

#define _automove

float disp0[8];
int16_t disp1[8];

void WholeInitTask()
{
//	//�����ǳ�ʼ��
//	MPU6500_Init();				
//	//ŷ���任��ʼ��	
//	init_euler();
	
	//���׿���(�̵���)��ʼ��
	Cylinder_Configuration();
	//�������Ƴ�ʼ��
	Duct_Configuration();
	//��������ͷ��������Ƴ�ʼ��
	Rotate_Configuration();
	//������Ƴ�ʼ��
	Servo_Configuration();
	//������ӽǳ�ʼ��
	Camera_Configuration();
	//���Ź���ʼ��
	IWDG_Configuration();
	
	//ģʽ���Ƴ�ʼ��
	Mode_Configuration();
#ifdef _automove
	//�Զ������Ƴ�ʼ��
	AutoMove_Configuration();	
#endif

	//��ʾ��
	printf("Hello world!\r\n");
	BUZZER_ON(1500);delay_ms(500);BUZZER_OFF();
}

/*-------------  ��ʼ�����̿��Ƴ���  -------------*/
void Init_Task()
{
	static uint32_t time_tick_init = 0; //0.2����������ԡ���ʱ���������ӿ��Ƴ���
	
	time_tick_init++;
	
	INITREADY_FLAG = 1;
	
	//ÿԼ11msιһ�ι�
	if(time_tick_init%11==0){
		IWDG_FeedDog();
	}
}


/*-------------  �����Ƴ���  -------------*/
void Control_Task()
{
	static uint32_t time_tick_1ms = 0; //0.1����������ԡ���ʱ���������ӿ��Ƴ���
	time_tick_1ms++;
	
	//��ȡ����ѭ��
	RemoteControlLoop();
//	//��س���ѭ��
//	monitorControlLoop();    //��س���Ҫ����̨����ǰ��ִ�У�����ң���������������ת
	
	//�������ƿ���ѭ��
	Duct_Loop();
	//���׿���ѭ��
	Cylinder_Loop();
	//��������ͷ���������ѭ��
	Rotate_Loop();
	//������ƿ���ѭ��
	Servo_Loop();
	//������ӽǿ���ѭ�� 10Hz
	Camera_Loop();
	

#ifdef _automove
	//�Զ������Ƴ���
	AutoMove_Loop(time_tick_1ms);
#endif
	
	//ÿԼ11msιһ�ι�
	if(time_tick_1ms%11==0){
		IWDG_FeedDog();
	}
	
//	if(time_tick_1ms%999==0){
//		printf("%d, %d\r\n",disp1[0],disp1[1]);
//		printf("\r\n");
//	}
	
	//�źŵ�
	switch(GlobalMode){
	case MODE_DRAG:
		if(time_tick_1ms%999 == 0) {
			LED_G_TOGGLE(); LED_R_TOGGLE();
		}
		break;
	case MODE_LIFT:
		if(time_tick_1ms%499 == 0) {
			LED_G_TOGGLE(); LED_R_TOGGLE();
		}
		break;
	case MODE_BULLET:
		if(time_tick_1ms%333 == 0) {
			LED_G_TOGGLE(); LED_R_TOGGLE();
		}
		break;
	default:
		LED_G_OFF(); LED_R_ON();
	}
	
}
