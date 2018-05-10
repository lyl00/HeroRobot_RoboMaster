#ifndef _CATCHCONTROL_H_
#define _CATCHCONTROL_H_

#include "rc.h"
#include "main.h"
#define Kv1 1//̧���ٶ�ϵ����Խ��̧��Խ��
#define Kv2 600//��ת�ٶ�
#define FORWARD 1//��ת�����ת
#define BACKWARD 2//��ת�����ת

// �Զ�ȡ�������������&�����жϵ�ǰ��ִ���ĸ�����
#define TASK_A			0x01      //ץ
#define TASK_B			0x02      //̧
#define TASK_C			0x04      //��
#define TASK_D			0x08      //��ת
#define TASK_E			0x10      //���ط�ת
#define TASK_F			0x20      //��
#define TASK_G			0x40      //��
#define TASK_H			0x80      //��
#define AutoCatch_FLIFT_POSLIMIT_H   8192*3.f      //�ٴ�̧���ĸ߶ȣ������ó���λֵ


void CatchControl(void);
void CatchInit(void);
void AutoCatch_tInit(void); //��ʼ������ʱ�䳣��
void AutoCatch_TaskControl(void); //�Զ�ȡ��ѭ��,����ʱ�����
void AutoCatch_TaskJudge(u8 Register); //����ȡ��ѭ���еõ��ļĴ���ֵ������Ӧ�������



#endif
