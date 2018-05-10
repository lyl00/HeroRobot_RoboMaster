/************************************************************
 *File		:	mpu6050_driver.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com	@YangTianhao ,490999282@qq.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	MPU6050 driver fuctions.
								MPU6050_Initialization()		: Check and Init
								MPU6050_ReadData()					:	Read raw data
								MPU6050_Gyro_calibration() 	: NO USE
 ************************************************************/
/*
	陀螺仪驱动程序
	初始化、规范化、读取数据程序
*/
#include "mpu6050_driver.h"
#include "mpu6050_i2c.h"
#include "mpu6050_process.h"
#include "usart6.h"	
#include "delay.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "GimbalControl.h"

#define offset_t 5000
MPU6050_RAW_DATA    MPU6050_Raw_Data; 
MPU6050_REAL_DATA   MPU6050_Real_Data;
MPU6050_REAL_DATA   MPU6050_AVG_Data;
struct __MPU6050_REAL_Data__  MPU_OFFSET;			//零漂
float AngleGyro = 0;
int gyroADC_X_offset=0,gyroADC_Y_offset=0,gyroADC_Z_offset=0;

//MPU6050 初始化，成功返回0  失败返回 0xff
int MPU6050_Initialization(void)
{
    u8 temp_data = 0;
		s16 i;
    IIC_GPIO_Init();  //初始化IIC接口
		delay_ms(100);
		IIC_WriteData(MPU6050_DEVICE_ADDRESS,PWR_MGMT_1,0x00);   //解除休眠状态
		delay_ms(100); 
		IIC_WriteData(MPU6050_DEVICE_ADDRESS,PWR_MGMT_1,0x80);   //复位休眠状态
		delay_ms(100);
		IIC_WriteData(MPU6050_DEVICE_ADDRESS,PWR_MGMT_1,0x00);   //解除休眠状态
		delay_ms(100); 
	
	    if(IIC_ReadData(MPU6050_DEVICE_ADDRESS,WHO_AM_I ,&temp_data ,1)==0) //??IIC?????????MPU6050
    {
        if(temp_data != MPU6050_ID)
        {
            printf("error 1A,%d \r\n",temp_data);
            return 0xff; //????,??0xff
        }
				printf("OK 1A,%d \r\n",temp_data);
    }
    else
    {
        printf("error 1B\r\n");
        return 0xff; //???? ??0xff
    }
				
    IIC_WriteData(MPU6050_DEVICE_ADDRESS,SMPLRT_DIV,0x07);//cyq：07 更新频率1khz
		IIC_WriteData(MPU6050_DEVICE_ADDRESS,CONFIG,0x06); //带宽5hz
    IIC_WriteData(MPU6050_DEVICE_ADDRESS,GYRO_CONFIG,0x10); //+—1000dps
		IIC_WriteData(MPU6050_DEVICE_ADDRESS,ACCEL_CONFIG,0x00);//+-2g
		IIC_WriteData(MPU6050_DEVICE_ADDRESS,ACCEL_CONFIG2,0x0E);//+-2g
    IIC_WriteData(MPU6050_DEVICE_ADDRESS,INT_PIN_CFG,0x80);   //INT引脚低电平有效
    delay_ms(100);
		
		MPU_OFFSET.Accel_X=0;
		MPU_OFFSET.Accel_Y=0;
		MPU_OFFSET.Accel_Z=0;
		
		for(i=0;i<offset_t;i++)
		{
			MPU6050_ReadData(0);	
			if(i==6)
			{
				MPU_OFFSET.Accel_X=0;
				MPU_OFFSET.Accel_Y=0;
				MPU_OFFSET.Accel_Z=0;
			}
		}
		MPU_OFFSET.Accel_X/=offset_t;
		MPU_OFFSET.Accel_Y/=offset_t;
		MPU_OFFSET.Accel_Z=MPU_OFFSET.Accel_Z/offset_t-16384;	  //?
		MPU_OFFSET.Gyro_X/=offset_t;
		MPU_OFFSET.Gyro_Y/=offset_t;
		MPU_OFFSET.Gyro_Z/=offset_t;
		
    return 0;
}

//MPU6050  数据读取，成功返回0  失败返回 0xff
int MPU6050_ReadData(u8 status)     //两种模式   初始化0和正式读取1
{
		u8 buf[14];     //用来储存数据
    IIC_ReadData(MPU6050_DEVICE_ADDRESS,MPU6050_DATA_START,buf,14);
       

		if(status == 0)        //初始化
	{
		MPU_OFFSET.Accel_X += ((s16)(buf[0]<<8)) | (buf)[1];
		MPU_OFFSET.Accel_Y += ((s16)(buf[2]<<8)) | (buf)[3];
		MPU_OFFSET.Accel_Z += ((s16)(buf[4]<<8)) | (buf)[5];
		MPU_OFFSET.Gyro_X += ((s16)(buf[8]<<8))	 | (buf)[9];
		MPU_OFFSET.Gyro_Y += ((s16)(buf[10]<<8)) | (buf)[11];
		MPU_OFFSET.Gyro_Z += ((s16)(buf[12]<<8)) | (buf)[13];
	}
	//measure status
	else if(status == 1)          //正式开始测量
	{
//		//加速度计
		MPU6050_Raw_Data.Accel_X = (buf[0]<<8 | buf[1]) - (s16)	MPU_OFFSET.Accel_X;
    MPU6050_Raw_Data.Accel_Y = (buf[2]<<8 | buf[3]) - (s16)	MPU_OFFSET.Accel_Y;
    MPU6050_Raw_Data.Accel_Z = (buf[4]<<8 | buf[5]) - (s16)	MPU_OFFSET.Accel_Z; 
    MPU6050_Raw_Data.Temp =    (buf[6]<<8 | buf[7]);  
    MPU6050_Raw_Data.Gyro_X = (buf[8]<<8 | buf[9])  - (s16)	MPU_OFFSET.Gyro_X;
    MPU6050_Raw_Data.Gyro_Y = (buf[10]<<8 | buf[11])- (s16)	MPU_OFFSET.Gyro_Y;
    MPU6050_Raw_Data.Gyro_Z = (buf[12]<<8 | buf[13])- (s16)	MPU_OFFSET.Gyro_Z;
	  

	        //将原生数据转换为实际值，计算公式跟寄存器的配置有关
    MPU6050_Real_Data.Accel_X = -(float)(MPU6050_Raw_Data.Accel_X)/16384.0f; //见datasheet 30 of 47
    MPU6050_Real_Data.Accel_Y = -(float)(MPU6050_Raw_Data.Accel_Y)/16384.0f; //见datasheet 30 of 47
		MPU6050_Real_Data.Accel_Z =  (float)(MPU6050_Raw_Data.Accel_Z)/16384.0f; //见datasheet 30 of 47

		MPU6050_Real_Data.Gyro_X = -(float)(MPU6050_Raw_Data.Gyro_X )/32.8f;     //见datasheet 32 of 47
    MPU6050_Real_Data.Gyro_Y = -(float)(MPU6050_Raw_Data.Gyro_Y )/32.8f;     //见datasheet 32 of 47
    MPU6050_Real_Data.Gyro_Z =  (float)(MPU6050_Raw_Data.Gyro_Z )/32.8f;     //见datasheet 32 of 47

	}   
    return 0;
}


//void MPU6050_Gyro_calibration(void)      //规范化
//{
//	u16 i;
//	float x_temp=0,y_temp=0,z_temp=0;
//	
//	for(i=0; i<1000; i++)
//	{
//		MPU6050_ReadData(0);
//		delay_ms(1);
//		x_temp=x_temp+MPU6050_Raw_Data.Gyro_X;
//		y_temp=y_temp+MPU6050_Raw_Data.Gyro_Y;
//		z_temp=z_temp+MPU6050_Raw_Data.Gyro_Z;
//	}			
//	
//	x_temp=x_temp/1000.00f;
//	y_temp=y_temp/1000.00f;	
//	z_temp=z_temp/1000.00f;

//	gyroADC_X_offset=(int)x_temp;
//	gyroADC_Y_offset=(int)y_temp;
//	gyroADC_Z_offset=(int)z_temp;
//}


