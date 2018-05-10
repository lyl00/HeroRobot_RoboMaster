#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "define.h"
#include "led.h"
//#include "adc.h"
#include "delay.h"
#include "ControlTask.h"
//#include "sys.h"
#include "bsp.h"
#include "timer.h"
#include "buzzer.h"
//#include "MonitorControl.h"
#include "mpu6500.h"
#include "spi.h"
//#include "iic.h"
#include "euler.h"
#include "usart6.h"	
#include "usart3.h"

#include "rc.h"

//#include "frame.h"
#include "can.h"
//#include "laser.h"
#include "pid.h"
//#include "CMControl.h"
#include "RemoteControl.h"
//#include "GimbalControl.h"
//#include "Auto_aim.h"
//#include "ShootControl.h"
//#include "mpu6050_interrupt.h"
//#include "mpu6050_driver.h"
//#include "mpu6050_i2c.h"
//#include "mpu6050_process.h"

#include "iwdg.h"
#include "rotate.h"
#include "mode.h"
#include "cylinder.h"
#include "duct.h"
#include "servo.h"
#include "automove.h"
#include "camera.h"

//Õ”¬›“« ¡„∆Ø ˝æ›
#define GYRO_X_OFFSET 0.750f  //1∫≈∞Â¡„∆Æ0.07500f
#define GYRO_Y_OFFSET 0.005012f
#define GYRO_Z_OFFSET -0.022880f
	

//#define RC_TEST
//#define RC_FUNC

#endif
