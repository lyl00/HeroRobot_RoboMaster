#include "main.h"
u8 pressflagl=0;
u8 lastpressflagl=0;
u8 pressflagr=0;
u8 lastpressflagr=0;
u8 IsInitialSet0=0;
int16_t liftspeed=0,rollspeed=0;

//***************************************************
//���������Զ�ȡ���ĸ�����ִ��ʱ�䣬���Ժ��޸ģ���������Ϊ200

u8 l_flag=0;
u8 r_flag=0;

//Ϊ��ֹ�󴥣����������֮���ٵ���Ҽ������Զ�ȡ���ű�


uint32_t tA_ms = 0 ;
uint32_t tB_ms = 1000 ;
uint32_t tC_ms = 1000 ;
uint32_t tD_ms = 8000 ;
uint32_t tE_ms = 1000 ;
uint32_t tF_ms = 1000 ;
uint32_t tG_ms = 1000 ;
uint32_t tH_ms = 1000 ;
uint32_t tI_ms = 1000 ;           //���һ���ջ�צ�ӵ���ռĴ�������ʱ��

uint32_t tA_Init_ms = 0 ;
uint32_t tB_Init_ms = 1000 ;          //��Լ1��
uint32_t tC_Init_ms = 1000 ;
uint32_t tD_Init_ms = 1000 ;
uint32_t tE_Init_ms = 8000 ;          //[(8192*3-11500)/(100*0.03)]*2 ��ȫϵ��
uint32_t tF_Init_ms = 1000 ;
uint32_t tG_Init_ms = 1000 ;
uint32_t tH_Init_ms = 1000 ;
uint32_t tI_Init_ms = 1000 ;

u8 CATCH_REG = 0;
u8 ID = 0;

//************************************************************************88


/***************************************************************************************
 *Name     : CatchControl
 *Function ��ץ���ϲ㺯��
 *Input    ����
 *Output   ���� 
 *Description : ����һ��̧�������2����ת���
****************************************************************************************/
void CatchControl(void)
{
	//�����ֶ�ȡ��ģʽ����.h��#define Manul_CatchControl ����
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
			setangle2=current_angle_203;       //�������޸�5_8
			IsInitialSet0=1;
		}
		
		TIM_SetCompare1(TIM8,50+250 ); 
		flift();//Ԥ̧��
		liftspeed=RC_Ex_Ctl.rc.ch3*Kv1;//�ٶ�ת��
		catchbullet(liftspeed);
	
		
		
		switch(RC_Ex_Ctl.key.v & (KEY_PRESSED_OFFSET_Q | KEY_PRESSED_OFFSET_E ))//���Q����E���Ƿ񱻰���
		{
			case ( KEY_PRESSED_OFFSET_Q)://����ǰ��
				moveforward();
				break;
			case ( KEY_PRESSED_OFFSET_E)://�����ջ�
				movebackward();
				break;
			default:
				break;
		}

		switch(RC_Ex_Ctl.key.v & (KEY_PRESSED_OFFSET_SHIFT | KEY_PRESSED_OFFSET_CTRL))//���SHIFT����CTRL���Ƿ񱻰���
		{ 
			case(KEY_PRESSED_OFFSET_SHIFT):	   //���Ӽн�
				catchstart();
				break;
			case(KEY_PRESSED_OFFSET_CTRL):	   //���ӷ���
				catchfinish();
				break;
			default: 													  
				break;
		}
		
		if(RC_Ex_Ctl.mouse.press_l)            //��ת����ж�
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
		fliftdown();//�����Զ��½���ԭλ,��ԭλ֮��0


	}
	
/*******����Ϊ�Զ�ȡ������,����ȡ��ģʽ������ͬ���⣬���������ֶ�������ͬ**********************************************************/
#else   
	if(remoteState == PREPARE_STATE)
	{
		IsInitialSet0=0;
    TIM_SetCompare1(TIM8,50+100); 
		CAN2_Send_Bottom(0,0,0);
	}
	
	/***********************************��Ҫ�޸��ڴ˲���***************/
	else if(remoteState == NORMAL_REMOTE_STATE )           
	{
		if(setangle1!=0&&IsInitialSet0==0)
		{
			setangle1=current_angle_201;
			setangle2=current_angle_203;       //�������޸�5_8,��������ֱ���ֵ
			IsInitialSet0=1;
		}
		
		TIM_SetCompare1(TIM8,50+250 ); 
		flift();//Ԥ̧��
		liftspeed=RC_Ex_Ctl.rc.ch3*Kv1;//�ٶ�ת��
		
		
		//���������Ƿ�����Զ�ȡ��,����r_flag=1ʱ�Զ�ȡ����������Ч����Ҫ��ʧ��ԭ����������Ҽ�//
		if(RC_Ex_Ctl.mouse.press_l)           
			l_flag=1;
		if(RC_Ex_Ctl.mouse.press_r&&(l_flag==1))
			r_flag=1;
		
		
		
		//�Զ�ģʽ�£��������ֶ�����
		switch(RC_Ex_Ctl.key.v & (KEY_PRESSED_OFFSET_Q | KEY_PRESSED_OFFSET_E ))//���Q����E���Ƿ񱻰���
		{
			case ( KEY_PRESSED_OFFSET_Q)://����ǰ��
				moveforward();
				break;
			case ( KEY_PRESSED_OFFSET_E)://�����ջ�
				movebackward();
				break;
			default:
				break;
		}
		//*************��ʼ�����Զ�ȡ������**************//
      AutoCatch_TaskControl();
		  AutoCatch_TaskJudge(CATCH_REG);
		  AutoCatch_CatchBullet(liftspeed);                        //����仰�����Զ�ȡ��ѭ����ĩβ
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
		fliftdown();//�����Զ��½���ԭλ,��ԭλ֮��0
	}
	
	
}		
#endif


/***************************************************************************************
 *Name     �� CatchInit
 *Function ��ץȡ��ʼ������
 *Input    ����
 *Output   ���� 
 *Description ��ץȡgpio��ʼ��
****************************************************************************************/

void CatchInit()
{
	moveinit();
	catchinit();
	liftinit();
	CAN2_Send_Bottom(0,0,0);
}


/***********�Զ�ȡ����������***************/

/***************************************************************************************
 *Name     ��AutoCatch_TaskControl
 *Function ���Զ�ȡ���������
 *Input    ����
 *Output   ���� 
 *Description ��ͨ���Ը�����Ĺ�����Զ�ȡ���Ĵ�������ֵͬ���Դ�����һ��������������ִ��
****************************************************************************************/
void AutoCatch_TaskControl(void){
	if(r_flag==1){
	/*********ʱ�������*********/
	  if(tA_ms==0&&tB_ms!=0){			//ʵ���ϴ˴�tB��ʱ�������A������ִ��ʱ��:��Ҫ!
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
		//ϵ�ж�������,д�������������ò���//
		if(tI_ms==0){
			CATCH_REG=0;       //����Ĵ���
			AutoCatch_tInit(); //������ʱ�������flag
		  l_flag=0;
		  r_flag=0;
		}
	
	}
}	

/***************************************************************************************
 *Name     ��AutoCatch_tInit
 *Function ��ʱ�䳣���ĳ�ʼ��
 *Input    ����
 *Output   ���� 
 *Description ����һ����������󣬶�����ʱ�䳣�����г�ʼ��
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
 *Name     ��AutoCatch_TaskJudge
 *Function ��ʱ�䳣���ĳ�ʼ��
 *Input    ����
 *Output   ���� 
 *Description ����һ����������󣬶�����ʱ�䳣�����г�ʼ��
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
						setangle2 -= FLIFT_LIFTSPEED*0.035f;            //���ܻ�̫��? 
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




