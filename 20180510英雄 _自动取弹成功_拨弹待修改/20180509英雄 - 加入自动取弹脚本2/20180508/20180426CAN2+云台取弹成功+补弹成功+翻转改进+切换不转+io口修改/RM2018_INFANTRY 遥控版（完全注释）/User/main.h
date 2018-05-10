#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx.h"
#include "led.h"

#include "math.h"
#include "adc.h"
#include "delay.h"
#include "ControlTask.h"
#include "sys.h"
#include "bsp.h"
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "buzzer.h"
#include "MonitorControl.h"
#include "mpu6500.h"
#include "spi.h"
#include "iic.h"
#include "euler.h"
#include "usart6.h"	
#include "usart3.h"
#include "rc.h"
#include "frame.h"
#include "define.h"
#include "can.h"
#include "laser.h"
#include "pid.h"
#include "CMControl.h"
#include "RemoteControl.h"
#include "GimbalControl.h"
#include "Auto_aim.h"
#include "ShootControl.h"
#include "mpu6050_interrupt.h"
#include "mpu6050_driver.h"
#include "mpu6050_i2c.h"
#include "mpu6050_process.h"
#include "cylinder.h"
#include "infrared.h"
#include "CatchControl.h"
#include "CatchBullet.h"
#include "pwm.h"
#include "RefereeInfo.h"

//Õ”¬›“« ¡„∆Ø ˝æ›
#define GYRO_X_OFFSET 0.0750f  //1∫≈∞Â¡„∆Æ0.07500f
#define GYRO_Y_OFFSET 0.005012f
#define GYRO_Z_OFFSET -0.022880f


	
#endif
