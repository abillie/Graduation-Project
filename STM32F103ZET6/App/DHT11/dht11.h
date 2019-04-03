#ifndef _dht11_H
#define _dht11_H

#include "sys.h"
#include "delay.h"
//! Port C 9
#define DHT11 (GPIO_Pin_9) 
#define GPIO_DHT11 GPIOC

//! 输入
#define DHT11_DQ_IN PCin(9)
//! 输出	  
#define DHT11_DQ_OUT PCout(9)  

void DHT11_IO_OUT(void);
void DHT11_IO_IN(void);
u8 DHT11_Init(void);
void DHT11_Rst(void);
u8 DHT11_Check(void);
u8 DHT11_Read_Bit(void);
u8 DHT11_Read_Byte(void);
u8 DHT11_Read_Data(u8 *temp,u8 *humi);


#endif
