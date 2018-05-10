/************************************************************
 *File		:	euler.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.03.02
 *Description: 	euler caluation functions
 ************************************************************/

#include "euler.h"
#include "math.h"
#include "mpu6500.h"
#define euler_dt 0.001f
#define PI 3.1415f
float Yaw_gyro,Roll_gyro,Pitch_gyro;
float Yaw_mag,Roll_accel,Pitch_accel;
float Yaw,Roll,Pitch,Yaw_Offset,Pitch_Offset,Roll_Offset;
int count = 0;

//磁力计未校准计算不准确 暂时用陀螺仪

float invSqrt(float num);

void init_euler(void){
	
	unsigned char i;
	float ax,ay,az,a;
	//float mx,my,m;
	ax=ay=az=0.0f;
	//mx=my=0.0f;
	for(i=0;i<5;i++){
		MPU6500_Read();
		//MPU6500_ReadMAG();
		ax+=mpu6500_real_data.Accel_X;
		ay+=mpu6500_real_data.Accel_Y;
		az+=mpu6500_real_data.Accel_Z;
//		mx+=ist8310_real_data.Mag_X;
//		my+=ist8310_real_data.Mag_Y;
		
		delay_ms(5);
	}
	
	
	a=1.0f/sqrt(ax*ax+ay*ay+az*az);
	ax*=a;
	ay*=a;
	az*=a;
//	m=1.0f/sqrt(my*my+mx*mx);
//	my*=m;
//	mx*=m;

	Roll=atan2(-ay,az);
	Pitch=asin(ax);
	//Yaw=atan2(my,mx);
	Yaw=0;
	
}
/*


*/

void update_euler(void){
	
	float Roll_sin,Roll_cos,Pitch_sin,Pitch_cos;
	float a,gx,gy,gz,dx,dy,dz;
	static const float K=1.0f;  //磁力计的互补比例系数 越大 磁力计的影响越小
	static const float KP=0.5f; //加速度计的P调节器越大 加速度计的影响越大
	Roll_sin=sin(Roll);
	Roll_cos=cos(Roll);
	Pitch_sin=sin(Pitch);
	Pitch_cos=cos(Pitch);
	
	MPU6500_Read();

	a=invSqrt(mpu6500_real_data.Accel_X*mpu6500_real_data.Accel_X
		+mpu6500_real_data.Accel_Y*mpu6500_real_data.Accel_Y
	+mpu6500_real_data.Accel_Z*mpu6500_real_data.Accel_Z);
	mpu6500_real_data.Accel_X*=a;
	mpu6500_real_data.Accel_Y*=a;
	mpu6500_real_data.Accel_Z*=a;

	dx=mpu6500_real_data.Accel_Y*Pitch_cos*Roll_cos-mpu6500_real_data.Accel_Z*Pitch_cos*Roll_sin;
	dy=mpu6500_real_data.Accel_Z*(-Pitch_sin)-mpu6500_real_data.Accel_X*Pitch_cos*Roll_cos;
	dz=mpu6500_real_data.Accel_X*Pitch_cos*Roll_sin+mpu6500_real_data.Accel_Y*Pitch_sin;

	gx=mpu6500_real_data.Gyro_X+KP*dx;
	gy=mpu6500_real_data.Gyro_Y+KP*dy;
	gz=mpu6500_real_data.Gyro_Z+KP*dz;
	
	
	Roll=Roll+
	(Pitch_cos*gx+Pitch_sin*Roll_sin*gy+Pitch_sin*Roll_cos*gz)/Pitch_cos*euler_dt;
	
	Pitch=Pitch+(Roll_cos*gy-Roll_sin*gz)*euler_dt;
	
	Yaw_gyro=Yaw+
	(Roll_sin*gy+Roll_cos*gz)/Pitch_cos*euler_dt;
	
	if(Yaw_gyro>PI){	
		Yaw_gyro-=2*PI;
	}else if(Yaw_gyro<-PI){
		Yaw_gyro+=2*PI;
	}
	if(Roll>PI){	
		Roll-=2.0f*PI;
	}else if(Roll<-PI){
		Roll+=2.0f*PI;
	}
	
	if(Pitch>PI/2.0f){	
		Pitch-=PI;
	}else if(Pitch<-PI/2.0f){
		Pitch+=PI;
	}
	
	//MPU6500_ReadMAG();
	//Yaw_mag=atan2(ist8310_real_data.Mag_Y,ist8310_real_data.Mag_X);
	Yaw=K*Yaw_gyro+(1.0f-K)*Yaw_mag;
	
	if(count <= 10)    //记录offset值
	{	
		Yaw_Offset = Yaw ;
		Pitch_Offset = Pitch;
		Roll_Offset = Roll;
		
		count++;
	}
	
}


float invSqrt(float num) {
	float halfnum = 0.5f * num;
	float y = num;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfnum * y * y));
	return y;
}


