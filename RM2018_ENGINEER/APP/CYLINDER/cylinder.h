#ifndef __CYLINDER_H__
#define __CYLINDER_H__
#include "stm32f4xx.h"

/* PC0 */
#define Cylinder1_On		GPIOC->BSRRL|=GPIO_Pin_0;		//Set
#define Cylinder1_Off		GPIOC->BSRRH|=GPIO_Pin_0;		//Reset
#define Cylinder1_Toggle	GPIOC->ODR^=GPIO_Pin_0;			//Toogle
/* PC1 */
#define Cylinder2_On		GPIOC->BSRRL|=GPIO_Pin_1;		//Set
#define Cylinder2_Off		GPIOC->BSRRH|=GPIO_Pin_1;		//Reset
#define Cylinder2_Toggle	GPIOC->ODR^=GPIO_Pin_1;			//Toogle
/* PC4 */
#define Cylinder3_On		GPIOC->BSRRL|=GPIO_Pin_4;		//Set
#define Cylinder3_Off		GPIOC->BSRRH|=GPIO_Pin_4;		//Reset
#define Cylinder3_Toggle	GPIOC->ODR^=GPIO_Pin_4;			//Toogle
/* PC5 */
#define Cylinder4_On		GPIOC->BSRRL|=GPIO_Pin_5;		//Set
#define Cylinder4_Off		GPIOC->BSRRH|=GPIO_Pin_5;		//Reset
#define Cylinder4_Toggle	GPIOC->ODR^=GPIO_Pin_5;			//Toogle


void Cylinder_Configuration(void);
void Cylinder_Loop(void);


////拖车气缸运动处理
//void Cylinder_Drag_Open(void);
//void Cylinder_Drag_Close(void);
//void Cylinder_Drag_Toggle(void);
//抱柱气缸运动处理
void Cylinder_Bao_Open(void);
void Cylinder_Bao_Close(void);
void Cylinder_Bao_Toggle(void);
//抓住气缸
void Cylinder_Catch_Open(void);
void Cylinder_Catch_Close(void);
void Cylinder_Catch_Toggle(void);
//伸出气缸
void Cylinder_Stretch_Open(void);
void Cylinder_Stretch_Close(void);
void Cylinder_Stretch_Toggle(void);

#endif
