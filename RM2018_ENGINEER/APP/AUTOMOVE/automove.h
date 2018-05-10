#ifndef __AUTOMOVE_H__
#define __AUTOMOVE_H__
#include "main.h"

typedef void(*pAutoMoveFunc)(void);

typedef struct AutoMoveUnit{
	uint8_t					id;			//�ڵ�ID
	uint16_t 				time;		//����ʱ��(ms)
	uint16_t				cnt;		//��������timeƥ��
	pAutoMoveFunc			func;		//ִ�к����ĺ���ָ��
} AutoMoveUnit,*pAutoMoveUnit;	//���������ʽ

typedef struct {
	uint16_t 		length:8;	//��������(����)
	uint16_t 		enable:1;	//ʹ��
	pAutoMoveUnit 	entry;		//AutoMoveUnit�����׵�ַ
	pAutoMoveFunc	end;		//����ʱִ��
	uint8_t			currID;		//��ǰID
} AutoMoveEvent;


void AutoMove_Configuration(void);
void AutoMove_Loop(uint32_t tick);
void AutoMove_Start(AutoMoveEvent* event);


//ע���Զ��¼���
//extern AutoMoveEvent 	LedEvent;				//������LED�¼�
extern AutoMoveEvent	CatchBulletEvent;		//ȡ���¼�
extern AutoMoveEvent	BaoEvent;				//�����¼�
//extern AutoMoveEvent	CameraEvent;			//�ӽǿ����¼�


#endif
