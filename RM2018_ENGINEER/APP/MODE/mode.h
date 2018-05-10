#ifndef __MODE_H__
#define __MODE_H__
#include "stm32f4xx.h"

typedef enum 
{
	MODE_STANBY=0,			//����/��ʼģʽ���������κ�ָ��
	MODE_DRAG,				//�ϳ�ģʽ��
	MODE_LIFT,				//̧��ģʽ��
	MODE_BULLET				//ȡ��ģʽ��
} Mode_WorkingMode;

#define MODE_DEFAULT	MODE_STANBY

extern Mode_WorkingMode GlobalMode;


void Mode_Configuration(void);
void Mode_Loop(void);

#endif
