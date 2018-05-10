#ifndef __PID_H__
#define __PID_H__

#include "stm32f4xx.h"
//#include "sys.h"
//#define u32 unsigned long
//#define u8 unsigned char

//PIDCR�Ĵ���������SetPIDCR��������ֵ��
#define PEN 0		  //����ʹ�ܣ�0-�رգ�1-�򿪣����Ʊ�������״̬
#define IEN 1		  //����ʹ�ܣ�0-�رգ�1-�򿪣����ƻ�������״̬
#define DEN 2		  //΢��ʹ�ܣ�0-�رգ�1-�򿪣�����΢������״̬
#define PIDEN 3		  //PIDʹ�ܣ�0-�رգ�1-�򿪣�����PID������ܿ���
#define IHEN 4		  //���ֱ���ʹ�ܣ�0-�رգ�1-�򿪣������������ɴ򿪱�Ϊ�رգ�����λ��λ����ᱣ��ԭ�����ֵ�ֵ�����򽫻���ֵ��0
#define INTERVAL 5	  //����PID_Calc�����ĵڶ�������Ϊϵͳʱ�仹�ǹ̶�ʱ�䣬0-ϵͳʱ�䣬����ϵͳʱ�䣬PID���㺯�����Զ�����ʱ��������λΪus��1-�̶�ʱ��������ʱPID���㺯���Ը�ֵ��ΪPID����ʱ��������λΪus
#define TOR 6		  //��ʱ��λ��0-�رգ�1-�򿪡�����λ��λʱ��ÿ�μ���������ʱ�����Ƿ����ĳ���趨ֵ��InternalTimeMax���������ڣ�˵����������ʱ��PID���������ᱻ��λ��

//PIDSR�Ĵ�����ֻ��)
#define CHANGE 0	  //PIDCRֵ�����ı�״̬λ������PIDCR�Ĵ������и���ʱ����λ����λ������PID����Ԥ����
#define FC 1		  //��һ��PID����״̬λ����PID��ʼ����λ�󣬸�λ����λ��ֱ����һ��PID������ɣ���λ�����������PID����Ԥ����
typedef struct 
{
	float  SetPoint;           //  �趨Ŀ�� Desired Value
	float  NowPoint;			//	��ǰʵ��ֵ 
	
	float  Proportion;         //  �������� Proportional Const 
	float  Integral;           //  ���ֳ��� Integral Const 
	float  Derivative;         //  ΢�ֳ��� Derivative Const 
		
	float  Integral_Min;		//�������޷�
	float  Integral_Max;		//�������޷�

	float  InternalTimeMax;//����PIDʱ�������ޣ���������������Ϊ������󣬲�����PID

	float  Out;			//PID�������ֵ

//��Ҫ��PID������ֱ�Ӹ������в���
	float  LastError;          //  Error[-1] 
	float  PrevError;          //  Error[-2] 
	float  IntegralError;           //  Integral of Errors

	u32    LastTime;//���ڼ�¼���һ�μ���PID��ʱ��

	u8 PIDCR;
	u8 PIDCR_DUMMY;
	u8 PIDSR;
}PID_Struct;

void PID_Init(PID_Struct* pid,float Proportion,float Integral,float Derivative,float Integral_Min,float Integral_Max,float InternalTimeMax);
void SetPIDCR(PID_Struct* pid,u8 value);
void PID_Calc(PID_Struct *pid,u32 IntervalTime);
float PID_Driver(float SetSpeed, float NowSpeed , PID_Struct* pid,u32 IntervalTime);//PID����Ԥ����
void PidValControl(PID_Struct* pid);          //���ڵ���PID����ʱ����̬�ظı�PIDֵ
#endif
