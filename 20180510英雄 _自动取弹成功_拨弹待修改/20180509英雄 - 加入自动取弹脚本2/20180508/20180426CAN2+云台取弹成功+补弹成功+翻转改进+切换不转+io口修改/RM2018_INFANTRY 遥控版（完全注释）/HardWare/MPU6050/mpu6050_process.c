/************************************************************
 *File		:	mpu6050_process.c
 *Author	:	@YangTianhao ,490999282@qq.com
 *Version	: v0.1
 *Update	: 2016.12.11
 *Description: 	Configuration , Data Filter and Angle Calculate
 ************************************************************/
#include "mpu6050_process.h"
#include "mpu6050_driver.h"
#include <math.h>
#include "sys.h"
#include "main.h"
#include "GimbalControl.h"

//#define Angel_Pitch_Cal

ACCEL_AVERAGE_DATA   Accel_Raw_Average_Data; 
GYRO_RADIAN_DATA     Gyro_Radian_Data;
MPU6050_ANGLE        MPU6050_Angle;

//MPU6050_Configuration
//功能：陀螺仪数据初始化
//参数：无 
//返回值：void
void MPU6050_Configuration(void)
{
	int i = 0;
	while(MPU6050_Initialization() == 0xff) 
    {
        i++;     //如果一次初始化没有成功，那就再来一次                     
        if(i>20) //如果初始化一直不成功，那就没希望了，进入死循环，蜂鸣器一直叫
        {
            while(1) 
            {
                delay_ms(500);
								printf("MPU6050 ERROR\n\r");
            }
        } 
				delay_ms(200);	
    }    
}


//MPU6050_Data_Filter
//功能：陀螺仪数据滤波函数
//参数：无 
//返回值：void
void MPU6050_Data_Filter(void)  // this*0.01 + (上一次用的)last *0.99 要改
{
    unsigned int i=0;
    static unsigned int first_flag = 0;
    static unsigned int filter_cnt = 0;    //计算加速度计滤波的次数
    
		
    long temp_accel_x = 0; //用来存放加速度计X轴原生数据的累加和
    long temp_accel_y = 0; //用来存放加速度计Y轴原生数据的累加和
    long temp_accel_z = 0; //用来存放加速度计Z轴原生数据的累加和
		long temp_gyro_x = 0; 
    long temp_gyro_y = 0; 
    long temp_gyro_z = 0;	
	
    static short accel_x_buffer[10] = {0}; //用来存放加速度计X轴最近10个数据的数组
    static short accel_y_buffer[10] = {0}; //用来存放加速度计Y轴最近10个数据的数组
    static short accel_z_buffer[10] = {0}; //用来存放加速度计Z轴最近10个数据的数组
    static short gyro_x_buffer[5] = {0}; 
    static short gyro_y_buffer[5] = {0}; 
    static short gyro_z_buffer[5] = {0}; 
		
    if(first_flag == 0) //如果第一次进来该函数，则对用来做平均的数组进行初始化
    {
        first_flag = 1; //以后不再进来
        for(i=0;i<5;i++)
        {
            accel_x_buffer[i] = MPU6050_Raw_Data.Accel_X;
            accel_y_buffer[i] = MPU6050_Raw_Data.Accel_Y;
            accel_z_buffer[i] = MPU6050_Raw_Data.Accel_Z;
						gyro_x_buffer[i]  = MPU6050_Raw_Data.Gyro_X;
            gyro_y_buffer[i]  = MPU6050_Raw_Data.Gyro_Y;
            gyro_z_buffer[i]  = MPU6050_Raw_Data.Gyro_Z;
						
        }
    }
    else  //如果不是第一次了
    {
        accel_x_buffer[filter_cnt] = MPU6050_Raw_Data.Accel_X;
        accel_y_buffer[filter_cnt] = MPU6050_Raw_Data.Accel_Y;
        accel_z_buffer[filter_cnt] = MPU6050_Raw_Data.Accel_Z;   
			
				gyro_x_buffer[filter_cnt]  = MPU6050_Raw_Data.Gyro_X;
        gyro_y_buffer[filter_cnt]  = MPU6050_Raw_Data.Gyro_Y;
        gyro_z_buffer[filter_cnt]  = MPU6050_Raw_Data.Gyro_Z;	
			
        filter_cnt ++;
        if(filter_cnt == 5)
        {
            filter_cnt = 0;
        }        
    }
    
    for(i=0;i<5;i++)
    {
        temp_accel_x += accel_x_buffer[i];
        temp_accel_y += accel_y_buffer[i];
        temp_accel_z += accel_z_buffer[i];
				temp_gyro_x += gyro_x_buffer[i];
				temp_gyro_y += gyro_y_buffer[i];
				temp_gyro_z += gyro_z_buffer[i];
    }
    
    Accel_Raw_Average_Data.X = (float)temp_accel_x / 10.0f;
    Accel_Raw_Average_Data.Y = (float)temp_accel_y / 10.0f;
    Accel_Raw_Average_Data.Z = (float)temp_accel_z / 10.0f;
    
		MPU6050_Real_Data.Gyro_X = (float)temp_gyro_x / 5.0f;
		MPU6050_Real_Data.Gyro_Y = (float)temp_gyro_y / 5.0f;
		MPU6050_Real_Data.Gyro_Z = (float)temp_gyro_z / 5.0f;
		
		MPU6050_Real_Data.Gyro_X = -(float)(MPU6050_Raw_Data.Gyro_X )/32.8f;     //见datasheet 32 of 47
    MPU6050_Real_Data.Gyro_Y = -(float)(MPU6050_Raw_Data.Gyro_Y )/32.8f;     //见datasheet 32 of 47
    MPU6050_Real_Data.Gyro_Z =  (float)(MPU6050_Raw_Data.Gyro_Z )/32.8f;     //见datasheet 32 of 47
		
 
	}

#define Kp 0.3f                        // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.008f                          // integral gain governs rate of convergence of gyroscope biases
#define halfT 0.0005f                   // half the sample period采样周期的一半


float q0 = 1, q1 = 0, q2 = 0, q3 = 0;    // quaternion elements representing the estimated orientation
float exInt = 0, eyInt = 0, ezInt = 0;    // scaled integral error
u8 MPU6050_Angle_Calculate( float gx,float gy,float gz,float ax,float ay,float az)
{
		float gz_input;
	
  float norm;
  float hx, hy, hz, bx, bz;
  float vx, vy, vz;// wx, wy, wz;
  float ex, ey, ez;


  float q0q0 = q0*q0;
  float q0q1 = q0*q1;
  float q0q2 = q0*q2;
  float q0q3 = q0*q3;
  float q1q1 = q1*q1;
  float q1q2 = q1*q2;
  float q1q3 = q1*q3;
  float q2q2 = q2*q2;
  float q2q3 = q2*q3;
  float q3q3 = q3*q3;
	
	gz_input=gz/Gyro_Gr*AtR;
	if(ax*ay*az==0)
 		return 0;
		
  norm = sqrt(ax*ax + ay*ay + az*az);       //acc?????
  ax = ax / norm;
  ay = ay / norm;
  az = az / norm;

  // estimated direction of gravity and flux (v and w)              ?????????/??
  vx = 2*(q1q3 - q0q2);												//????xyz???
  vy = 2*(q0q1 + q2q3);
  vz = q0q0 - q1q1 - q2q2 + q3q3 ;

  // error is sum of cross product between reference direction of fields and direction measured by sensors
  ex = (ay*vz - az*vy) ;                           					 //???????????????
  ey = (az*vx - ax*vz) ;
  ez = (ax*vy - ay*vx) ;

  exInt = exInt + ex * Ki;								          //???????
  eyInt = eyInt + ey * Ki;
  ezInt = ezInt + ez * Ki;

  // adjusted gyroscope measurements
  gx = gx + Kp*ex + exInt;					   							//???PI???????,???????
  gy = gy + Kp*ey + eyInt;
  gz = gz + Kp*ez + ezInt;				   							//???gz????????????????,??????????????

  // integrate quaternion rate and normalise						   //????????
  q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
  q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
  q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
  q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

  // normalise quaternion
  norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  q0 = q0 / norm;
  q1 = q1 / norm;
  q2 = q2 / norm;
  q3 = q3 / norm;
  //????????
  MPU6050_Angle.Pitch  = asin(-2 * q1q3 + 2 * q0q2)* 57.3; // pitch

  MPU6050_Angle.Roll = atan2(2 * q2q3 + 2 * q0q1, -2 * q1q1 - 2 * q2q2 + 1)* 57.3; // roll

  MPU6050_Angle.Yaw += 2*gz_input*halfT; // yaw
	return 0;
}
