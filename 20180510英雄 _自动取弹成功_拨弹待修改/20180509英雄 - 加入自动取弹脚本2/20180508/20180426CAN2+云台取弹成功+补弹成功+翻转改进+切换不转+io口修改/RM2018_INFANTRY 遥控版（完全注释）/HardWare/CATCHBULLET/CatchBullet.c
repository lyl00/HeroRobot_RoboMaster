#include "main.h"

static uint32_t count1=0;
//static uint32_t count_rotate=Cycle_rotate;
//static int flag_rotate=0;

float rotate_speed=0;
float liftspeed1=0,liftspeed2=0;
float setangle1=0,setangle2=0,setrotate=0,lastrotate=0;
int RotateFlag=0;
u8 RotateDone=0;

PID_Struct CatchRotatePid;
PID_Struct CatchRotateSPEEDPid;
PID_Struct CatchLiftPid1;
PID_Struct CatchLiftPid2;
PID_Struct CatchLiftPOSPid1;
PID_Struct CatchLiftPOSPid2;

/***************************************************************************************
 *Name     : catchbullet
 *Function ：抓弹底层函数
 *Input    ：抬升速度liftspeed和翻转速度rollspeed
 *Output   ：无 
 *Description : 控制一个抬升电机和2个翻转电机
****************************************************************************************/

void catchbullet(int16_t liftspeed)
{
		count1++;
	//确定旋转速度
	if(RotateFlag==1&&RotateDone==0)
	{
//		count_rotate=0;
		RotateFlag=0;
		setrotate=6*440;
		RotateDone=1;
	}
	if(RotateFlag==-1&&RotateDone==1)
	{
		RotateFlag=0;
		setrotate=0;
		RotateDone=0;
	}

	RotateSpeedcal();
	lastrotate=current_angle_202;
	//确定抬升速度
	setangle1 += liftspeed*0.01f;
	setangle1 = (setangle1>LIFT_POSLIMIT)?LIFT_POSLIMIT:((setangle1<-LIFT_POSLIMIT)?-LIFT_POSLIMIT:setangle1);
	setangle2 = -setangle1;
	
	LiftPosSpeedcal();
	
	CAN2_Send_Bottom(liftspeed1,rotate_speed,liftspeed2);
	
}


/*******************************************************
  *Name     : AutoCatch_CatchBullet
 *Function ：自动抓弹底层函数
 *Input    ：抬升速度liftspeed和翻转速度rollspeed
 *Output   ：无 
 *Description : 控制一个抬升电机和2个翻转电机    
*********************************************************/
void AutoCatch_CatchBullet(int16_t liftspeed) 
{
	
	if(RotateFlag==1) setrotate=6*440;
	if(RotateFlag==0) setrotate=0;
	
	RotateSpeedcal();
	lastrotate=current_angle_202;
	//确定抬升速度
	setangle1 += liftspeed*0.01f;
	setangle1 = (setangle1>LIFT_POSLIMIT)?LIFT_POSLIMIT:((setangle1<-LIFT_POSLIMIT)?-LIFT_POSLIMIT:setangle1);
	setangle2 = -setangle1;
	
	LiftPosSpeedcal();
	
	CAN2_Send_Bottom(liftspeed1,rotate_speed,liftspeed2);
	
	
}


/***************************************************************************************
 *Name     ： moveinit
 *Function ：夹子前进过程初始化函数
 *Input    ：无
 *Output   ：无 
 *Description ：夹子前进过程初始化
****************************************************************************************/
void moveinit()
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//PD13
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD,GPIO_Pin_14);
	
}

/***************************************************************************************
 *Name     ： catchinit
 *Function ：夹子夹紧过程初始化函数
 *Input    ：无
 *Output   ：无 
 *Description ：夹子夹紧过程初始化
****************************************************************************************/
void catchinit()
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);//PD14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOH, &GPIO_InitStructure);
	GPIO_SetBits(GPIOH,GPIO_Pin_10);
	
	PID_Init(&CatchRotatePid, Rotate_POS_p, Rotate_POS_i, Rotate_POS_d, -Rotate_POS_limit, Rotate_POS_limit,0);
	SetPIDCR(&CatchRotatePid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&CatchRotateSPEEDPid, Rotate_SPEED_p, Rotate_SPEED_i, Rotate_SPEED_d, -Rotate_SPEED_limit, Rotate_SPEED_limit,0);
	SetPIDCR(&CatchRotateSPEEDPid,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
}

/***************************************************************************************
 *Name     ：liftinit
 *Function ：夹子夹紧过程初始化函数
 *Input    ：无
 *Output   ：无 
 *Description ：夹子夹紧过程初始化
****************************************************************************************/

void liftinit()
{
	
	PID_Init(&CatchLiftPid1, Lift_SPEED_p, Lift_SPEED_p, Lift_SPEED_p, -Lift_SPEED_limit, Lift_SPEED_limit,0);
	SetPIDCR(&CatchLiftPid1,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&CatchLiftPid2, Lift_SPEED_p, Lift_SPEED_p, Lift_SPEED_p, -Lift_SPEED_limit, Lift_SPEED_limit,0);
	SetPIDCR(&CatchLiftPid2,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&CatchLiftPOSPid1, Lift_POS_p, Lift_POS_i, Lift_POS_d, -Lift_POS_limit, Lift_POS_limit,0);
	SetPIDCR(&CatchLiftPOSPid1,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	PID_Init(&CatchLiftPOSPid2, Lift_POS_p, Lift_POS_i, Lift_POS_d, -Lift_POS_limit, Lift_POS_limit,0);
	SetPIDCR(&CatchLiftPOSPid2,(1<<PEN)+(1<<IEN)+(1<<PIDEN)+(1<<INTERVAL)+(1<<DEN));
	CAN2_Send_Bottom(0,0,0);
}



/***************************************************************************************
 *Name     ： moveforward
 *Function ：夹子前进函数
 *Input    ：无
 *Output   ：无 
 *Description ：驱动夹子前进
****************************************************************************************/

void moveforward()
{
	//GPIO_SetBits(GPIOD,GPIO_Pin_13);
	GPIO_ResetBits(GPIOH,GPIO_Pin_10);
}

/***************************************************************************************
 *Name     ： movebackward
 *Function ：夹子后退函数
 *Input    ：无
 *Output   ：无 
 *Description ：驱动夹子后退
****************************************************************************************/

void movebackward()
{


//	GPIO_ResetBits(GPIOD,GPIO_Pin_13);
	GPIO_SetBits(GPIOH,GPIO_Pin_10);
	
}

/***************************************************************************************
 *Name     ： catchstart
 *Function ：夹子夹紧函数
 *Input    ：无
 *Output   ：无 
 *Description ：驱动夹子夹紧
****************************************************************************************/

void catchstart()
{
	GPIO_SetBits(GPIOD,GPIO_Pin_14);
}

/***************************************************************************************
 *Name     ： catchfinish
 *Function ：夹子放松函数
 *Input    ：无
 *Output   ：无 
 *Description ：驱动夹子夹紧
****************************************************************************************/

void catchfinish()
{
	GPIO_ResetBits(GPIOD,GPIO_Pin_14);
}

/***************************************************************************************
 *Name     ： catchtest
 *Function ：抓弹位置测试函数函数
 *Input    ：无
 *Output   ：无 
 *Description ：测试用，返回角度值和速度值
****************************************************************************************/

void catchtest()
{
	printf("201currentspeed:%d\r\n",can2_current_speed201);
	printf("201currentangle:%d\r\n",can2_current_angle201);
	printf("202currentspeed:%d\r\n",can2_current_speed202);
	printf("202currentangle:%d\r\n",can2_current_angle202);
	printf("203currentspeed:%d\r\n",can2_current_speed203);
	printf("203currentangle:%d\r\n",can2_current_angle203);
	
}

/***************************************************************************************
 *Name     ：RotateSpeedcal
 *Function ：取弹翻转速度计算函数
 *Input    ：无
 *Output   ：无 
 *Description ：发送速度值
****************************************************************************************/

void RotateSpeedcal()
{
	rotate_speed= PID_Driver(setrotate, current_angle_202, &CatchRotatePid, 10000);
	rotate_speed = Lift_Legalize(rotate_speed,3000);
	rotate_speed= PID_Driver(rotate_speed, can2_current_speed202, &CatchRotateSPEEDPid, 10000);
	rotate_speed = Lift_Legalize(rotate_speed,3000);
}

/***************************************************************************************
 *Name     ：LiftSpeedcal
 *Function ：取弹翻转速度计算函数
 *Input    ：无
 *Output   ：无 
 *Description ：发送速度值
****************************************************************************************/

void LiftSpeedcal(float speed1,float speed2)
{
	liftspeed1 = Lift_Legalize(speed1*LIFT_SCALE,Lift_SPEED_Max);
	liftspeed2 = Lift_Legalize(speed2*LIFT_SCALE,Lift_SPEED_Max);
	
	liftspeed1 = PID_Driver(liftspeed1, can2_current_speed201, &CatchLiftPid1, 10000);
	liftspeed2 = PID_Driver(liftspeed2, can2_current_speed203, &CatchLiftPid2, 10000);
	
	liftspeed1 = Lift_Legalize(liftspeed1,Lift_SPEED_Max);
	liftspeed2 = Lift_Legalize(liftspeed2,Lift_SPEED_Max);
	
}

/***************************************************************************************
 *Name     ：LiftPosSpeedcal
 *Function ：取弹翻转速度计算函数(位置环)
 *Input    ：无
 *Output   ：无 
 *Description ：发送速度值
****************************************************************************************/

void LiftPosSpeedcal(void)
{
	float delta;
	delta = setangle1 - current_angle_201;
		if((delta>LIFT_FILTER)||(delta<-LIFT_FILTER))
		{
			liftspeed1 = PID_Driver(setangle1, current_angle_201, &CatchLiftPOSPid1, 2000); 
		} 
		else 
		{
			liftspeed1 = 0;
		}
		delta = setangle2 - current_angle_203;
		if((delta>LIFT_FILTER)||(delta<-LIFT_FILTER))
		{
			liftspeed2 = PID_Driver(setangle2, current_angle_203, &CatchLiftPOSPid2, 2000); 
		} 
		else 
		{
			liftspeed2 = 0;
		}
	

		liftspeed1 = Lift_Legalize(liftspeed1*LIFT_SCALE,Lift_SPEED_Max);
		liftspeed2 = Lift_Legalize(liftspeed2*LIFT_SCALE,Lift_SPEED_Max);
		
}

/***************************************************************************************
 *Name     ：Lift_Legalize
 *Function ：抬升过程速度限制函数
 *Input    ：无
 *Output   ：无 
 *Description ：幅值限制函数
****************************************************************************************/

float Lift_Legalize(float MotorCurrent , float limit)
{
	return MotorCurrent<-limit?-limit:(MotorCurrent>limit?limit:MotorCurrent);
}

/***************************************************************************************
 *Name     ：flift
 *Function ：换抬升模式之后预提升到某一高度
 *Input    ：无
 *Output   ：无 
 *Description ：无
****************************************************************************************/

void flift()
{
	if(setangle1<=FLIFT_POSLIMIT_H)
	{
		setangle1 += FLIFT_LIFTSPEED*0.04f;
		setangle2 -= FLIFT_LIFTSPEED*0.04f;            //这里有修改
		
	}
	
}

/***************************************************************************************
 *Name     ：fliftdown
 *Function ：换其他之后下降到某一高度
 *Input    ：无
 *Output   ：无 
 *Description ：无
****************************************************************************************/

  void fliftdown()
{
	if(setangle1>FLIFT_POSLIMIT_L)
	{
		setangle1 -= FLIFT_LIFTSPEED*0.04f;
		setangle2 += FLIFT_LIFTSPEED*0.04f;
	}
	if(setangle1<=FLIFT_POSLIMIT_L)
	{
		setangle1 = 0;
		setangle2 = -setangle1;
	}
	LiftPosSpeedcal();
	//CAN2_Send_Bottom(liftspeed1,0,liftspeed2);
}

