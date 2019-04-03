#include "LED.h"

//********************************************************************//
//! 函数名:LED_GPIO_Config
//! 功能:LEDGPIO初始化
//! 输入:none 
//! 输出:none
//********************************************************************// 
void LED_GPIO_Config(void)
{		
	 //! 申明GPIO结构体
	 GPIO_InitTypeDef GPIO_InitStructure;
	 //! 开启GPIOE外设时钟
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	 //! GPIO5和6															   
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_5;	
	 //! 推挽输出模式
  	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	 //! GPIO速率50MHz 
  	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	 //! 初始化
     GPIO_Init(GPIOE, &GPIO_InitStructure);
	 //! 拉高GPIO5,6
     GPIO_SetBits(GPIOE,GPIO_Pin_5);   
	 GPIO_SetBits(GPIOE,GPIO_Pin_6);   
}



