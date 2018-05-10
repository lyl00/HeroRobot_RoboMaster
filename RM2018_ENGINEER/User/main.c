/************************************************************
 *File		:	main.c
  *Author	:  @YangTianhao ,490999282@qq.com£¬@TangJiaxin ,tjx1024@126.com, @YangTao ,ybb331082@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	
                RM2018_INFANTRY project. 
							
							- Control the Chassis motors , Gimbal motors , Shoot motor BY Remote controller.
							-	Using CAN bus to connect all 7 motors(Yaw,pitch,motorA,motorB,motorC,motorD,ShootMotor) made by DJI.
							-	Manage the power of chassis by electricity module.
							- Control the friction wheel motors by electronic governors.
								
								Instruction:
								
              - Turn on the controller,and switch on the power,you will hear the buzzer ring a few,that means the mechine is initializing.						
							- When the buzzer is silent again,which means the mechine is ready to move,you have the permission to control it.
							-	Channel 0 1 2 3 corresponds to Translation crosswise,Translation lengthways,Pitching and Rotate respectively.
							- Switching s1 from middle position to up position then switch back will turn on the friction wheels,and do that again will 
							  turn off them.When switch to down position,the gun shoots.
							- Three positions of s2 corresponds to Remote pattern,Key pattern and Standby pattern respectively from up to down.
                							
								
						                                                            
 					       ___	  ______                             					     	______    ___ 
					      ||_||	 | __   |______                               _____|   __ |  ||_||
					       |_|___|______|------          *********            -----|______|___| |   
                _|_|_  TongJi  _\_ _				                        _ _/_  TongJi  _|_|_	
				       / \ / \___RM___/ \ / \                              / \ / \___RM___/ \ / \
				      ||- + -||      ||- + -||                            ||- + -||      ||- + -|| 
				       \_/_\_/	      \_/_\_/                              \_/_\_/	      \_/_\_/ 
					   
					   
			- Buzzer Reference
				long 1 beep -> totally initialized 
				short 1 beeps -> Darg Mode
				short 2 beeps -> Lift Mode
				short 3 beeps -> (Take) Bullet Mode
			- System Time unit 1ms
			- UART PG9 RX  PG14 TX
			- Gyro 180d = pi
			- Motor: 	CAN1 for CM, 205 for catch pillar, 206 for catch bullet
						CAN2 for lift
			- PWM:		TIM4 CH1 CH2 for duct, CH3 CH4 for servo
						TIM8 CH3 CH4 for camera (PI7 PI2)
			- PC0 PC1 PC4 PC5 for cylinder
			
			- Control:
				Right 1-drag 2-lift 3-bullet
				Left 1-enable something 2-disable something
				DRAG:
					Q: disable something
					E: enable something
					W: up pitch
					S: low pitch
				LIFT:
					(×óÓÒ = Duct)
					Q: Roate bao positive -- auto
					E: Roate bao reverse -- auto
					A: duct right
					D: duct left
					
				BULLET:
					Q: take bullet -- auto
					S: open cover
					W: close cover
					A: move left
					D: move right
			
			THIS IS THE PROGRAM FOR UPPER CONTROLLER
				
  --- All rights reserved by TongJi RoboMaster Club(TJRC).
	
	School of Mechanical Engineering Tongji University,
	No.4800,CaoYang Road,JiaDing District,
	Shanghai City,P.R.China,201804
 ************************************************************/
 
#include "main.h" 

int main(void)
{ 
	//ÅäÖÃµ×²ãÇý¶¯
	BSP_Init();					
	while(1)
	{}
}
