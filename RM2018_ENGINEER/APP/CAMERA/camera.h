#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "stm32f4xx.h"

#define CAMERA_TIMx					TIM8

#define CAMERA_DUTY_MIN				480			// 0.48ms
#define CAMERA_DUTY_MAX				2500		// 2.50ms
#define CAMERA_DUTY_DARG_yaw		510
#define CAMERA_DUTY_LIFT_yaw		1980
#define CAMERA_DUTY_BULLET_yaw		1180
#define CAMERA_DUTY_pitch			495
#define CAMERA_DUTY_DRAG_pitch		1100

void Camera_Configuration(void);
void Camera_Loop(void);

void Camera_Pitch_Back(void);
void Camera_Pitch(void);
void Camera_Yaw(void);

#endif
