#include "main.h"
u8 pressflagl=0;
u8 lastpressflagl=0;
u8 pressflagr=0;
u8 lastpressflagr=0;
u8 IsInitialSet0=0;
int16_t liftspeed=0,rollspeed=0;

//***************************************************
//定义用于自动取弹的各任务执行时间，测试后修改，初步都定为200

u8 l_flag=0;
u8 r_flag=0;

//为防止误触，点击鼠标左键之后再点击右键触发自动取弹脚本


uint32_t tA_ms = 0 ;
uint32_t tB_ms = 1000 ;
uint32_t tC_ms = 1000 ;
uint32_t tD_ms = 8000 ;
uint32_t tE_ms = 1000 ;
uint32_t tF_ms = 1000 ;
uint32_t tG_ms = 1000 ;
uint32_t tH_ms = 1000 ;
uint32_t tI_ms = 1000 ;           //最后一步收回爪子到清空寄存器给的时间

uint32_t tA_Init_ms = 0 ;
uint32_t tB_Init_ms = 1000 ;          //大约1秒
uint32_t tC_Init_ms = 1000 ;
uint32_t tD_Init_ms = 1000 ;
uint32_t tE_Init_ms = 8000 ;          //[(8192*3-11500)/(100*0.03)]*2 安全系数
uint32_t tF_Init_ms = 1000 ;
uint32_t tG_Init_ms = 1000 ;
uint32_t tH_Init_ms = 1000 ;
uint32_t tI_Init_ms = 1000 ;

u8 CATCH_REG = 0;
u8 ID = 0;

//************************************************************************88


/***************************************************************************************
 *Name     : CatchControl
 *Function ：抓弹上层函数
 *Input    ：无
 *Output   ：无 
 *Description : 控制一个抬升电机和2个翻转电机
****************************************************************************************/
void CatchControl(void)
{
	//保留手动取弹模式，在.h中#define Manul_CatchControl 即可
	#ifdef MANUL_CATCH
	
	if(remoteState == PREPARE_STATE)
	{
		
    TIM_SetCompare1(TIM8,50+100); 
		CAN2_Send_Bottom(0,0,0);
	}
	else if(remoteState == NORMAL_REMOTE_STATE )    
	{
		if(setangle1!=0&&IsInitialSet0==0)
		{
			setangle1=current_angle_201;
			setangle2=current_angle_203;       //这里有修改5_8
			IsInitialSet0=1;
		}
		
		TIM_SetCompare1(TIM8,50+250 ); 
		flift();//预抬升
		liftspeed=RC_Ex_Ctl.rc.ch3*Kv1;//速度转化
		catchbullet(liftspeed);
	
		
		
		switch(RC_Ex_Ctl.key.v & (KEY_PRESSED_OFFSET_Q | KEY_PRESSED_OFFSET_E ))//检测Q键和E键是否被按下
		{
			case ( KEY_PRESSED_OFFSET_Q)://夹子前伸
				moveforward();
				break;
			case ( KEY_PRESSED_OFFSET_E)://夹子收回
				movebackward();
				break;
			default:
				break;
		}

		switch(RC_Ex_Ctl.key.v & (KEY_PRESSED_OFFSET_SHIFT | KEY_PRESSED_OFFSET_CTRL))//检测SHIFT键和CTRL键是否被按下
		{ 
			case(KEY_PRESSED_OFFSET_SHIFT):	   //夹子夹紧
				catchstart();
				break;
			case(KEY_PRESSED_OFFSET_CTRL):	   //夹子放松
				catchfinish();
				break;
			default: 													  
				break;
		}
		
		if(RC_Ex_Ctl.mouse.press_l)            //翻转电机判断
		{
			pressflagl=1;
		}
		else 
		{
			pressflagl=0;
			if(lastpressflagl)
				RotateFlag=1;
		}              //why
		lastpressflagl=pressflagl;  
		if(RC_Ex_Ctl.mouse.press_r)
		{
			pressflagr=1;
		}
		else 
		{
			pressflagr=0;
			if(lastpressflagr)
			 	RotateFlag=-1;
		}
		lastpressflagr=pressflagr;
	}


		
	else if(remoteState == STANDBY_STATE )    //????
	{
//		  return;	
    IsInitialSet0=0;		
    TIM_SetCompare1(TIM8,50+90);
		CAN2_Send_Bottom(0,0,0);
	}
	else if(remoteState == ERROR_STATE )      //????
	{
		IsInitialSet0=0;
		TIM_SetCompare1(TIM8,50+100);
		CAN2_Send_Bottom(0,0,0);
//		  return;
	}
	else if(remoteState == KEY_REMOTE_STATE ) //????
	{
		IsInitialSet0=0;
		TIM_SetCompare1(TIM8,50+100);
		fliftdown();//夹子自动下降回原位,归原位之后发0


	}
	
/*******以下为自动取弹部分,除了取弹模式有所不同以外，其余与上手动部分相同**********************************************************/
#else   
	if(remoteState == PREPARE_STATE)
	{
		IsInitialSet0=0;
    TIM_SetCompare1(TIM8,50+100); 
		CAN2_Send_Bottom(0,0,0);
	}
	
	/***********************************主要修改在此部分***************/
	else if(remoteState == NORMAL_REMOTE_STATE )           
	{
		if(setangle1!=0&&IsInitialSet0==0)
		{
			setangle1=current_angle_201;
			setangle2=current_angle_203;       //这里有修改5_8,两个电机分别算值
			IsInitialSet0=1;
		}
		
		TIM_SetCompare1(TIM8,50+250 ); 
		flift();//预抬升
		liftspeed=RC_Ex_Ctl.rc.ch3*Kv1;//速度转化
		
		
		//用来控制是否进入自动取弹,仅当r_flag=1时自动取弹函数才生效，需要先失能原本的鼠标左右键//
		if(RC_Ex_Ctl.mouse.press_l)           
			l_flag=1;
		if(RC_Ex_Ctl.mouse.press_r&&(l_flag==1))
			r_flag=1;
		
		
		
		//自动模式下，保留此手动弹出
		switch(RC_Ex_Ctl.key.v & (KEY_PRESSED_OFFSET_Q | KEY_PRESSED_OFFSET_E ))//检测Q键和E键是否被按下
		{
			case ( KEY_PRESSED_OFFSET_Q)://夹子前伸
				moveforward();
				break;
			case ( KEY_PRESSED_OFFSET_E)://夹子收回
				movebackward();
				break;
			default:
				break;
		}
		//*************开始调用自动取弹函数**************//
      AutoCatch_TaskControl();
		  AutoCatch_TaskJudge(CATCH_REG);
		  AutoCatch_CatchBullet(liftspeed);                        //把这句话放在自动取弹循环的末尾
  }
	
	else if(remoteState == STANDBY_STATE )    //????
	{
//		  return;	
    IsInitialSet0=0;		
    TIM_SetCompare1(TIM8,50+90);
		CAN2_Send_Bottom(0,0,0);
	}
	
	
	else if(remoteState == ERROR_STATE )      //????
	{
		IsInitialSet0=0;
		TIM_SetCompare1(TIM8,50+100);
		CAN2_Send_Bottom(0,0,0);
//		  return;
	}
	
	
	else if(remoteState == KEY_REMOTE_STATE ) //
	{
		IsInitialSet0=0;
		TIM_SetCompare1(TIM8,50+100);
		fliftdown();//夹子自动下降回原位,归原位之后发0
	}
	
	
}		
#endif


/***************************************************************************************
 *Name     ： CatchInit
 *Function ：抓取初始化函数
 *Input    ：无
 *Output   ：无 
 *Description ：抓取gpio初始化
****************************************************************************************/

void CatchInit()
{
	moveinit();
	catchinit();
	liftinit();
	CAN2_Send_Bottom(0,0,0);
}


/***********自动取弹函数定义***************/

/***************************************************************************************
 *Name     ：AutoCatch_TaskControl
 *Function ：自动取弹任务管理
 *Input    ：无
 *Output   ：无 
 *Description ：通过对各任务的管理给自动取弹寄存器赋不同值，以代入下一步函数进行任务执行
****************************************************************************************/
void AutoCatch_TaskControl(void){
	if(r_flag==1){
	/*********时钟任务层*********/
	  if(tA_ms==0&&tB_ms!=0){			//实际上此处tB的时间决定了A动作的执行时间:重要!
		  ID = 0;
		  CATCH_REG = 1<<ID;
      tB_ms--;
		}
		if(tB_ms==0&&tC_ms!=0){
			ID = 1;	
		  CATCH_REG = 1<<ID;
			tC_ms--;
		}
	  if(tC_ms==0&&tD_ms!=0){
			ID = 2;
		  CATCH_REG = 1<<ID;
			tD_ms--;
		}
	  if(tD_ms==0&&tE_ms!=0){
			ID = 3;
		  CATCH_REG = 1<<ID;
			tE_ms--;
		}
	  if(tE_ms==0&&tF_ms!=0){
			ID = 4;	
		  CATCH_REG = 1<<ID;
			tF_ms--;
		}
	  if(tF_ms==0&&tG_ms!=0){
			ID = 5;	
		  CATCH_REG = 1<<ID;
			tG_ms--;
		}
	  if(tG_ms==0&&tH_ms!=0){
			ID = 6;
		  CATCH_REG = 1<<ID;
			tH_ms--;
		}
	  if(tH_ms==0&&tI_ms!=0){
			ID = 7;	
		  CATCH_REG = 1<<ID;
			tI_ms--;
		}
		//系列动作结束,写结束动作并重置参数//
		if(tI_ms==0){
			CATCH_REG=0;       //清零寄存器
			AutoCatch_tInit(); //再重置时间参数和flag
		  l_flag=0;
		  r_flag=0;
		}
	
	}
}	

/***************************************************************************************
 *Name     ：AutoCatch_tInit
 *Function ：时间常数的初始化
 *Input    ：无
 *Output   ：无 
 *Description ：在一组任务结束后，对所有时间常数进行初始化
****************************************************************************************/
void AutoCatch_tInit(void){

tA_ms = tA_Init_ms ;
tB_ms = tB_Init_ms ;
tC_ms = tC_Init_ms ;
tD_ms = tD_Init_ms ;
tE_ms = tE_Init_ms ;
tF_ms = tF_Init_ms ;
tG_ms = tG_Init_ms ;
tH_ms = tH_Init_ms ;
tI_ms = tI_Init_ms ;
	
}




/***************************************************************************************
 *Name     ：AutoCatch_TaskJudge
 *Function ：时间常数的初始化
 *Input    ：无
 *Output   ：无 
 *Description ：在一组任务结束后，对所有时间常数进行初始化
****************************************************************************************/
void AutoCatch_TaskJudge(u8 Register){
   switch(Register){
	    case(TASK_A):
				catchstart();
				break;
			case(TASK_B):
				if(setangle1<=AutoCatch_FLIFT_POSLIMIT_H)
					{
						setangle1 += FLIFT_LIFTSPEED*0.035f;
						setangle2 -= FLIFT_LIFTSPEED*0.035f;            //可能会太快? 
					}
				break;
			case(TASK_C):
				movebackward();
				break;
			case(TASK_D):
				RotateFlag=1;
			  break;
			case(TASK_E):
				RotateFlag=0;
				break;
			case(TASK_F):
				moveforward();
				break;
			case(TASK_G):
				catchfinish();
			break;
			case(TASK_H):
				movebackward();
			break;
			default:
			break;
		}
}




