#ifndef __MPU6050_PROCESS_H__
#define __MPU6050_PROCESS_H__

#include "stm32f4xx.h"
//#include "sys.h"

#define dt 0.001
#define RtA 	57.324841				//弧度到角度
#define AtR    	0.0174533				//角度到弧度
#define Acc_G 	0.0011963				//加速度变成G   1/8192*9.8
#define Gyro_G 	0.0152672				//角速度变成度	1/65.5
#define Gyro_Gr	0.0002663		

typedef struct __ACCEL_AVERAGE_DATA__
{
    float X;
    float Y;
    float Z;
}ACCEL_AVERAGE_DATA;

typedef struct __GYRO_RADIAN_DATA__
{
    float X;
    float Y;
    float Z;
}GYRO_RADIAN_DATA;

typedef struct __MPU6050_ANGLE__
{
    float Pitch;
    float Roll;
    float Yaw;
}MPU6050_ANGLE;

extern ACCEL_AVERAGE_DATA   Accel_Raw_Average_Data; 
extern GYRO_RADIAN_DATA     Gyro_Radian_Data;
extern MPU6050_ANGLE        MPU6050_Angle;

void MPU6050_Configuration(void);
float CalculateAngleRateGyro(float GyroX);      //积分法求YAW角
void MPU6050_Data_Filter(void);
u8 MPU6050_Angle_Calculate( 	float gyro_x,
                              float gyro_y,
                              float gyro_z,
                              float accel_x,
                              float accel_y,
                              float accel_z);
#endif
