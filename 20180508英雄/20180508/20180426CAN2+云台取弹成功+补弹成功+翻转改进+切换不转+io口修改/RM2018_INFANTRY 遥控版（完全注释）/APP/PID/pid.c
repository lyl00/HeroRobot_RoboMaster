/************************************************************
 *File		:	pid.c
 *Author	:  @YangTianhao ,490999282@qq.com��@TangJiaxin ,tjx1024@126.com	
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Pid Struct
								Pid output calculation
 ************************************************************/
#include "pid.h"
#include "sys.h"
#include "usart6.h"
#include "main.h"

float test_p = 0,test_i = 0,test_d = 0.00,test_limit=120;   //��PID��

u8 PID_BitIsSet(u8 Register,u8 Position)
{
	if(Register & 1<<Position)
		return 1;
	return 0;
}
//PID_Init
//���ܣ�PID���庯��
//������pidָ��, p, i ,d ,�������ޣ��������ޣ�����ʱ��ֵ
//����ֵ��void
void PID_Init(PID_Struct* pid,float Proportion,float Integral,float Derivative,float Integral_Min,float Integral_Max,float InternalTimeMax)
{
	pid->Proportion=Proportion;
	pid->Integral=Integral;
	pid->Derivative=Derivative;
	pid->Integral_Min=Integral_Min;
	pid->Integral_Max=Integral_Max;
	pid->PIDCR=0x00;
	pid->PIDCR_DUMMY=0x00;
	pid->PIDSR=0x00;
}

void SetPIDCR(PID_Struct* pid,u8 value)
{
	pid->PIDCR_DUMMY=value;
	pid->PIDSR|=1<<CHANGE;
}
//PID_PreCalc
//���ܣ�PID����Ԥ����
//������pidָ��
//����ֵ��void
void PID_PreCalc(PID_Struct* pid)//PID����Ԥ����
{
	u8 ChangeFlag,EdgeTypeFlag;//ChangeFlag��ӦλΪ1��ʾ��־λ�����ı䣬�������ı䣬EdgeTypeFlag��ӦλΪ0��ʾ�����أ�Ϊ1��ʾ�½���
	ChangeFlag=pid->PIDCR^pid->PIDCR_DUMMY;
	EdgeTypeFlag=ChangeFlag&pid->PIDCR;

	if(PID_BitIsSet(ChangeFlag,PIDEN))//PIDENλ�����κα仯���������΢�ֺͻ�������¼ֵ
	{
		pid->IntegralError=pid->PrevError=pid->LastError=0;
		if(PID_BitIsSet(EdgeTypeFlag,PIDEN))//���PIDENλ�ɹرձ�Ϊ��������PIDSR��FC��λ
			pid->PIDSR|=1<<FC;

	}
	else//��PIDENλδ�����仯���ٴ���PEN,IEN,DENλ�ı仯
	{
		if(PID_BitIsSet(ChangeFlag,IEN) && PID_BitIsSet(EdgeTypeFlag,IEN) && !PID_BitIsSet(pid->PIDCR,IHEN))//�������رգ����PIDCR��IHENλʹ�ܣ����������ֵ���������
			pid->IntegralError=0;

		if(PID_BitIsSet(ChangeFlag,DEN) && PID_BitIsSet(EdgeTypeFlag,DEN))//΢�����رգ������΢�ּ�¼ֵ
			pid->PrevError=pid->LastError=0;
	}

	pid->PIDCR=pid->PIDCR_DUMMY;//�޸�PIDCR��ֵ
	pid->PIDSR&=~(1<<CHANGE);//�Ѿ�������PIDCR�仯�����PIDSR��CHANGEλ
}

//PID_Calc
//���ܣ�PID����
//������pidָ�� ,����ʱ��ֵ
//����ֵ��void �ı�pid���outputֵ
void PID_Calc(PID_Struct* pid,u32 IntervalTime) 
{ 
	float  	dError,//���΢��
					Error, //���
					Out=0;//PID���
	float 	Interval;//PID����ʱ����
	
	if(PID_BitIsSet(pid->PIDSR,CHANGE))//PIDCR�Ĵ����������ģ��������Ӧ����
		PID_PreCalc(pid);

	if(PID_BitIsSet(pid->PIDCR,INTERVAL)==0)//�������ϵͳʱ��
	{		
		Interval=0.000001f*(IntervalTime-pid->LastTime);
		pid->LastTime=IntervalTime;
		if(PID_BitIsSet(pid->PIDSR,FC))//�������ϵͳʱ�䣬�����ǵ�һ�ε��ü��㺯������ôֻ��¼��ǰϵͳʱ�䣬������PID����
		{
			pid->PIDSR&=~(1<<FC);
			return;
		}
		if(PID_BitIsSet(pid->PIDCR,TOR) && Interval>pid->InternalTimeMax)//�������ϵͳʱ�䣬���PIDCR��TOR��λ������ʱ�������м�飬�������InternalTimeMax������Ϊ�������������PID����
		{
			pid->IntegralError=pid->PrevError=pid->LastError=0;
			return;
		}
	}
	else
		Interval=0.000001f*IntervalTime;//��usת����s


	if(PID_BitIsSet(pid->PIDCR,PIDEN))//PIDENλʹ��
	{		
		Error = pid->SetPoint - pid->NowPoint;          // ���

		if(PID_BitIsSet(pid->PIDCR,PEN))//PENλʹ�ܣ�����������
		{
			Out+= pid->Proportion * Error;
		}

		if(PID_BitIsSet(pid->PIDCR,IEN))//IENλʹ�ܣ�����������
		{
			pid->IntegralError += Error*Interval;                      // ���� 
			if(pid->IntegralError>pid->Integral_Max)	pid->IntegralError=pid->Integral_Max;	//�������޷�
			if(pid->IntegralError<pid->Integral_Min)	pid->IntegralError=pid->Integral_Min;	//�������޷�
			Out+= pid->Integral * pid->IntegralError;
		}
		
		if(PID_BitIsSet(pid->PIDCR,DEN))//DENλʹ�ܣ�����΢�����
		{
			dError = (pid->LastError - pid->PrevError)/Interval;     // ��ǰ΢��
			pid->PrevError = pid->LastError; 
			pid->LastError = Error;
			Out+= pid->Derivative * dError;
		}
	}

	pid->Out=Out;										//������һ��out
}

//PID_Driver
//���ܣ�PID����Ԥ����
//������setspeed, nowspeed , pidstruct, intervalTime
//����ֵ��float
float PID_Driver(float SetSpeed, float NowSpeed , PID_Struct* pid,u32 IntervalTime)//PID����Ԥ����
{
	float pidout;
	(*pid).SetPoint = SetSpeed ;//MotorPIDOutYaw;//�����趨ֵ�Ͳⶨֵ
	(*pid).NowPoint = NowSpeed;
	PID_Calc(pid,IntervalTime);
	pidout=(*pid).Out ;
	return pidout;
}

//PidValControl
//���ܣ�PID�����ú���
//������ pidstruct,  
//����ֵ��void
void PidValControl(PID_Struct* pid)          //���ڵ���PID����ʱ����̬�ظı�PIDֵ
{
	u8 temp_uartbuf;
//	printf("Receive: %c \r\n",usart1_rxbuf);
	temp_uartbuf = usart6_rxbuf;
	usart6_rxbuf = 0;
	switch(temp_uartbuf)
	{
		case 'q':test_p += 2;break;
		case 'w':test_i += 0.5f;break;
		case 'e':test_d += 0.001f;break;
		case 'r':test_limit += 5;break;
		case 'a':test_p -= 1;break;
		case 's':test_i -= 0.1f;break;
		case 'd':test_d -= 0.001f;break;
		case 'f':test_limit -= 5;break;
		case 'z':test_p *= 10;break;
		default:break;
	}
	printf("p:%f\t i:%f\t d:%f\t limit:%f\t \r\n",test_p,test_i,test_d,test_limit);
	PID_Init(pid,test_p,test_i,test_d,test_limit*(-1),test_limit,0);    //
	SetPIDCR(pid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
}
