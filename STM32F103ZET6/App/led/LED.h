#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"

//! PE5拉高
#define LED1_OFF GPIO_SetBits(GPIOE,GPIO_Pin_5);
//! PE5拉低    
#define LED1_ON GPIO_ResetBits(GPIOE,GPIO_Pin_5); 
//! PE6拉高  
#define LED2_OFF GPIO_SetBits(GPIOE,GPIO_Pin_6);
//! PE6拉低    
#define LED2_ON GPIO_ResetBits(GPIOE,GPIO_Pin_6);   

//********************************************************************//
//! 函数名:LED_GPIO_Config
//! 功能:LEDGPIO初始化
//! 输入:none 
//! 输出:none
//********************************************************************// 
void LED_GPIO_Config(void);

#endif /* __LED_H */
