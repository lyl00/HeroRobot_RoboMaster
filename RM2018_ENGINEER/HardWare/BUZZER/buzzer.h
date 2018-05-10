#ifndef __BUZZER_H__
#define __BUZZER_H__
#include "sys.h"

void Buzzer_Init(void);
u8 Start(void);
void Buzzer_toggle(void);
#define BUZZER_ON(a)     TIM_SetCompare1(TIM3,a) ;
#define BUZZER_OFF()     TIM_SetCompare1(TIM3,0) ;

#endif
