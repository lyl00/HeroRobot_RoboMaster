#ifndef __MPU6050_DRIVER_H__
#define __MPU6050_DRIVER_H__

#include "stm32f4xx.h"
//#include "sys.h"

#define	SMPLRT_DIV		0x19	//陀螺仪采样率 典型值 0X07 125Hz
#define	CONFIG			0x1A	//低通滤波频率 典型值 0x00 
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围                 典型值 0x18 不自检 2000deg/s
#define	ACCEL_CONFIG	0x1C	//加速度计自检及测量范围及高通滤波频率 典型值 0x01 不自检 2G 5Hz
#define	ACCEL_CONFIG2 0x1D

#define INT_PIN_CFG     0x37
#define INT_ENABLE      0x38
#define INT_STATUS      0x3A    //只读


#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C

#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E

#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40

#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	

#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46

#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48

#define MPU_USER_CTRL_REG 	0x6A
#define MPU_FIFO_CNTL_REG		0x73
#define	PWR_MGMT_1		0x6B	//电源管理 典型值 0x00 正常启用
#define	WHO_AM_I			0x75	//只读  默认读出应该是 MPU6050_ID = 0x68


#define MPU6050_ID              0x68
//#define MPU6050_DEVICE_ADDRESS  0xD0
#define MPU6050_DEVICE_ADDRESS  0xD0	
#define MPU6050_DATA_START      ACCEL_XOUT_H   //由于数据存放地址是连续的，所以一并读出

typedef struct __MPU6050_RAW_Data__
{
    short Accel_X;  //寄存器原生X轴加速度表示值
    short Accel_Y;  //寄存器原生Y轴加速度表示值
    short Accel_Z;  //寄存器原生Z轴加速度表示值
    short Temp;     //寄存器原生温度表示值
    short Gyro_X;   //寄存器原生X轴陀螺仪表示值
    short Gyro_Y;   //寄存器原生Y轴陀螺仪表示值
    short Gyro_Z;   //寄存器原生Z轴陀螺仪表示值
}MPU6050_RAW_DATA;

typedef struct __MPU6050_REAL_Data__
{
    float Accel_X;  //转换成实际的X轴加速度，
    float Accel_Y;  //转换成实际的Y轴加速度，
    float Accel_Z;  //转换成实际的Z轴加速度，
    float Temp;     //转换成实际的温度，单位为摄氏度
    float Gyro_X;   //转换成实际的X轴角加速度，
    float Gyro_Y;   //转换成实际的Y轴角加速度，
    float Gyro_Z;   //转换成实际的Z轴角加速度
}MPU6050_REAL_DATA;

extern MPU6050_RAW_DATA    MPU6050_Raw_Data; 
extern MPU6050_REAL_DATA   MPU6050_Real_Data;
extern float AngleGyro;
int MPU6050_Initialization(void);
int MPU6050_ReadData(u8 status);
void MPU6050_Gyro_calibration(void);

#endif
