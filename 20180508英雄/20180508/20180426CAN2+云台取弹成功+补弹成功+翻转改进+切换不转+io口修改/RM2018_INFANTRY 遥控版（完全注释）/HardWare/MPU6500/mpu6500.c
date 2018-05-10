#include "mpu6500.h"
#include "spi.h"
#include "main.h"
#define ACCEL_SEN 16384.0f
#define GYRO_SEN 1880.0f 
#define MAG_SEN 0.3f
MPU6500_RAW_DATA mpu6500_raw_data;
MPU6500_REAL_DATA mpu6500_real_data;


float offset_x=GYRO_X_OFFSET,offset_y=GYRO_Y_OFFSET,offset_z=GYRO_Z_OFFSET;

unsigned char getDeviceID(void){
		unsigned char ID;
	ID=MPU6500ReadSPISingle(WHO_AM_I);
	
	if(ID==MPU6500_ID){
		return 1;
	}else{
		
		return 0;
	}
	
}
void MPU6500_Init(void)
{
	delay_ms(1000);
	if(getDeviceID())
		{
			//2g 1000度 1.3 
			delay_ms(10);
			MPU6500WriteSPI(MPU6500_PWR_MGMT_1,0x80);       //复位MPU9250
			delay_ms(100);
			MPU6500WriteSPI(MPU6500_PWR_MGMT_1, 0x03);       //唤醒MPU9250
			delay_ms(10);
			MPU6500WriteSPI(MPU6500_PWR_MGMT_2, 0x00);   
			delay_ms(10);
			MPU6500WriteSPI(MPU6500_SMPLRT_DIV, 0x00);       //陀螺仪采样率，0x07(1kHz) 
			delay_ms(10);
			MPU6500WriteSPI(MPU6500_CONFIG, 0x02);
			delay_ms(10);
			MPU6500WriteSPI(MPU6500_INT_PIN_CFG,0x02);
			delay_ms(10);
			MPU6500WriteSPI(MPU6500_GYRO_CONFIG,0x10);   //设置陀螺仪满量程	±1000dps	
			delay_ms(10);
			MPU6500WriteSPI(MPU6500_ACCEL_CONFIG,0x02); //设置加速度计满量程	±2g
			delay_ms(10);
			MPU6500WriteSPI(MPU6500_ACCEL_CONFIG_2,0x08);	
			delay_ms(10);
			gyro_offset();
	}
		
}
void MPU6500_Read(void){
	unsigned char buf[14];

	MPU6500ReadSPI(ACCEL_XOUT_H,buf,14);
	
	mpu6500_raw_data.Accel_X=(short)((buf[0])<<8) | buf[1];
	mpu6500_real_data.Accel_X=mpu6500_raw_data.Accel_X/ACCEL_SEN;
	
	mpu6500_raw_data.Accel_Y=(short)((buf[2])<<8 )| buf[3];
	mpu6500_real_data.Accel_Y=mpu6500_raw_data.Accel_Y/ACCEL_SEN;
	
	mpu6500_raw_data.Accel_Z=(short)((buf[4])<<8 )| buf[5];
	mpu6500_real_data.Accel_Z=mpu6500_raw_data.Accel_Z/ACCEL_SEN;
	
	mpu6500_raw_data.Temp=(short)((buf[6])<<8 )| buf[7];
	mpu6500_real_data.Temp=mpu6500_raw_data.Temp*1.0;
	
	mpu6500_raw_data.Gyro_X=(short)((buf[8])<<8 )| buf[9];
	mpu6500_real_data.Gyro_X=mpu6500_raw_data.Gyro_X/GYRO_SEN -offset_x;
	
	mpu6500_raw_data.Gyro_Y=(short)((buf[10])<<8 )| buf[11];
	mpu6500_real_data.Gyro_Y=mpu6500_raw_data.Gyro_Y/GYRO_SEN -offset_y;
	
	mpu6500_raw_data.Gyro_Z=(short)((buf[12])<<8 )| buf[13];
	mpu6500_real_data.Gyro_Z=mpu6500_raw_data.Gyro_Z/GYRO_SEN -offset_z;
	
}
void MPU6500_ReadACCEL(void){
	unsigned char buf[6];

	MPU6500ReadSPI(ACCEL_XOUT_H,buf,6);
	
	mpu6500_raw_data.Accel_X=(short)((buf[0])<<8) | buf[1];
	mpu6500_real_data.Accel_X=mpu6500_raw_data.Accel_X/ACCEL_SEN;
	
	mpu6500_raw_data.Accel_Y=(short)((buf[2])<<8 )| buf[3];
	mpu6500_real_data.Accel_Y=mpu6500_raw_data.Accel_Y/ACCEL_SEN;
	
	mpu6500_raw_data.Accel_Z=(short)((buf[4])<<8 )| buf[5];
	mpu6500_real_data.Accel_Z=mpu6500_raw_data.Accel_Z/ACCEL_SEN;

}
void MPU6500ReadGYRO(void){
	unsigned char buf[6];

	MPU6500ReadSPI(GYRO_XOUT_H,buf,6);
	
	mpu6500_raw_data.Gyro_X=(short)((buf[0])<<8 )| buf[1];
	mpu6500_real_data.Gyro_X=mpu6500_raw_data.Gyro_X/GYRO_SEN - offset_x;
	
	
	mpu6500_raw_data.Gyro_Y=(short)((buf[2])<<8 )| buf[3];
	mpu6500_real_data.Gyro_Y=mpu6500_raw_data.Gyro_Y/GYRO_SEN - offset_y;
	
	
	mpu6500_raw_data.Gyro_Z=(short)((buf[4])<<8 )| buf[5];
	mpu6500_real_data.Gyro_Z=mpu6500_raw_data.Gyro_Z/GYRO_SEN - offset_z;
	
	
}

short myabs(short a){
	if(a>0){
		return a;
	}else{
		return -a;
	}
	
}
void gyro_offset(){
		unsigned char buf[6];
		short last_x=0,last_y=0,last_z=0;
		short gyro_x,gyro_y,gyro_z;
		long x_add=0,y_add=0,z_add=0;
		unsigned short i;
		
		for(i=0;i<1000;i++){
			MPU6500ReadSPI(GYRO_XOUT_H,buf,6);
			gyro_x=(short)((buf[0])<<8 )| buf[1];
			gyro_y=(short)((buf[2])<<8 )| buf[3];
			gyro_z=(short)((buf[4])<<8 )| buf[5];
			if(myabs(gyro_x-last_x)>100||myabs(gyro_y-last_y)>100||myabs(gyro_z-last_z)>100){
				static unsigned int num=0;
				num+=i;
				i=0;
				last_x=last_y=last_z=0;
				x_add=y_add=z_add=0;
				
				if(num>500){
					
					return ;
					
				}
				continue;
			}
			
			x_add+=gyro_x;
			y_add+=gyro_y;
			z_add+=gyro_z;
			last_x=gyro_x;
			last_y=gyro_y;
			last_z=gyro_z;
			delay_ms(1);
		}
		
		offset_x=x_add/1000.f/GYRO_SEN;
		offset_y=y_add/1000.f/GYRO_SEN;
		offset_z=z_add/1000.f/GYRO_SEN;
		
}



unsigned char MPU6500ReadSPISingle(unsigned char reg){
	unsigned char res;
	SPI5_NS=0;
	SPI5ReadWriteByte(reg|0x80);
	res=SPI5ReadWriteByte(0xff);
	SPI5_NS=1;
	return res;
}
unsigned char MPU6500ReadSPI(unsigned char reg,unsigned char *buf,unsigned char len){
	unsigned char i;
	SPI5_NS=0;
	SPI5ReadWriteByte(reg|0x80);
	for(i=0;i<len;i++){
		*buf=SPI5ReadWriteByte(0xff);
		buf++;
	}
	SPI5_NS=1;
	return 1;
	
}
unsigned char MPU6500WriteSPI(unsigned char reg,unsigned char data){
	unsigned char status;
	SPI5_NS= 0;
	status=SPI5ReadWriteByte(reg);
	SPI5ReadWriteByte(data);
	SPI5_NS= 1;
	return status;
}



