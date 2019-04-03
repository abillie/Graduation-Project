#include "time.h"
#include "lcd.h"
#include "delay.h"
u32 temp=0;
u8 time3_flag=0,usart_time=0;
u8 time_hours=0,time_minutes=0,time_seconds=0;
//********************************************************************//
//! 函数名:time3_init
//! 功能:定时器3初始化
//! 输入:none 
//! 输出:none
//********************************************************************// 
void time3_init(u32 rcc,u32 psc)
{
	//! 声明定时器结构体
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	 
	//! 中断优先级配置结构体
	NVIC_InitTypeDef NVIC_InitStructure;

	//! 开启定时器时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  //! 清除中断标志
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	//! 自动重装寄存器周期的值 10000*1/2000=5 秒
	TIM_TimeBaseInitStructure.TIM_Period = rcc;
  //! 预分频系数,2KHz计数频率,72*10e6/(35999+1) = 2000,每 1/2000 秒定时器计数加一
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	//! 设置时钟分割:TDTS = Tck_Tim
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0; 
	//! 计数器向上计数模式
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//! 初始化定时器
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	//! 使能定时器	
	TIM_Cmd(TIM3,ENABLE); 
	//! 使能定时器中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );	
	
	//! 配置定时器中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	//!打开TIM_IRQn通道的中断
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 
	//!抢占优先级０
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	//! 子优先级1	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1; 
	//! 使能 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);	
}
//********************************************************************//
//! 函数名:TIM3_IRQHandler
//! 功能:定时器3中断回调函数,更新LCD上的时间显示
//! 输入:none 
//! 输出:none
//********************************************************************// 
void TIM3_IRQHandler()	
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{
    USART_Cmd(USART1,DISABLE);
	  USART_Cmd(USART2,DISABLE);
		
		time3_flag=1;
		usart_time++;
		time_seconds++;
		if(time_seconds==60)
		{
			time_seconds=0;
			time_minutes++;
		}
		if(time_minutes==60)
		{
			time_hours++;
			time_minutes=0;
		}
		if(time_hours==24)
		{
			time_hours=0;
		}
		if(time_seconds<=9)
		{
			delay_ms(5);
			LCD_ShowxNum(162,10,0,1,16,0);                //154+8=162
			delay_ms(5);
			LCD_ShowxNum(170,10,time_seconds,1,16,0);     //154+8+8=170
		}
		else
		{
			delay_ms(5);
			LCD_ShowxNum(162,10,time_seconds,2,16,0);
		}
		if(time_minutes<=9)
		{
			delay_ms(5);
			LCD_ShowxNum(138,10,0,1,16,0);                //130+8=138
			delay_ms(5);
			LCD_ShowxNum(146,10,time_minutes,1,16,0);     //130+8+8=146
		}
		else
		{
			delay_ms(5);
			LCD_ShowxNum(138,10,time_minutes,2,16,0);     //130+8=138
		}
		if(time_hours<=9)
		{
			delay_ms(5);
			LCD_ShowxNum(114,10,0,1,16,0);                //98+16=114
			delay_ms(5);
			LCD_ShowxNum(122,10,time_hours,1,16,0);       //98+16+8=122
		}
		else
		{
			delay_ms(5);
			LCD_ShowxNum(114,10,time_hours,2,16,0);            //98+16=114
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		USART_Cmd(USART1,ENABLE);
	  USART_Cmd(USART2,ENABLE);
	}
}

