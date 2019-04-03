#ifndef __SYS_H
#define __SYS_H	
#include "stm32f10x.h"
#include "misc.h"															    
//! 位带操作,实现51类似的GPIO控制功能
//! 具体实现思想,参考<CM3权威指南>第五章87~92页
//! IO口宏操作定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//! IO地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//! IO口操作,只针对单一IO口操作
//! 确保n值小于16
//! 输出
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)
//! 输入  
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  

//! 输出
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n) 
//! 输入 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n) 

//! 输出
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n) 
//! 输入 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  

//! 输出
#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n) 
//! 输入 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)

//! 输出
#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n) 
//! 输入 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  

//! 输出
#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n) 
//! 输入
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n) 

//! 输出
#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  
//! 输入 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  



void NVIC_Configuration(void);



#endif
