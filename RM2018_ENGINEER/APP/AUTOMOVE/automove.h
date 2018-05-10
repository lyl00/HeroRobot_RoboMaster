#ifndef __AUTOMOVE_H__
#define __AUTOMOVE_H__
#include "main.h"

typedef void(*pAutoMoveFunc)(void);

typedef struct AutoMoveUnit{
	uint8_t					id;			//节点ID
	uint16_t 				time;		//动作时长(ms)
	uint16_t				cnt;		//计数器，time匹配
	pAutoMoveFunc			func;		//执行函数的函数指针
} AutoMoveUnit,*pAutoMoveUnit;	//用链表的形式

typedef struct {
	uint16_t 		length:8;	//动作长度(个数)
	uint16_t 		enable:1;	//使能
	pAutoMoveUnit 	entry;		//AutoMoveUnit数组首地址
	pAutoMoveFunc	end;		//结束时执行
	uint8_t			currID;		//当前ID
} AutoMoveEvent;


void AutoMove_Configuration(void);
void AutoMove_Loop(uint32_t tick);
void AutoMove_Start(AutoMoveEvent* event);


//注册自动事件表
//extern AutoMoveEvent 	LedEvent;				//测试用LED事件
extern AutoMoveEvent	CatchBulletEvent;		//取弹事件
extern AutoMoveEvent	BaoEvent;				//抱柱事件
//extern AutoMoveEvent	CameraEvent;			//视角控制事件


#endif
