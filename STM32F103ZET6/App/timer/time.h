#ifndef _time_H
#define _time_H
#include "stm32f10x.h"
extern u8 time3_flag,usart_time,time_hours,time_minutes,time_seconds;
//********************************************************************//
//! 函数名:TIM3_IRQHandler
//! 功能:定时器3中断回调函数,更新LCD上的时间显示
//! 输入:none 
//! 输出:none
//********************************************************************// 
void time3_init(u32 rcc,u32 psc);
#endif
