#ifndef __MONITORCONTROL_H
#define __MONITORCONTROL_H

#include "main.h"

extern float ele_cal;

void monitorControlLoop(void);		
int16_t CMWatt_Cal(int16_t speedX , float ele_now);

#endif
