#include "dht11.h"


//********************************************************************//
//! 函数名:DHT11_Init
//! 功能:初始化dht11
//! 输入:none
//! 输出:是否初始化成功;成功:0;失败:1
//********************************************************************//
u8 DHT11_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin=DHT11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);
	//! 拉高
	GPIO_SetBits(GPIO_DHT11,DHT11);	   

	DHT11_Rst();	  
	return DHT11_Check();	
}
//********************************************************************//
//! 函数名:DHT11_Rst
//! 功能:复位dht11
//! 输入:none
//! 输出:none
//********************************************************************//
void DHT11_Rst()	   
{    
	//! 输出模式             
	DHT11_IO_OUT();
	//! 拉低DQ 	
    DHT11_DQ_OUT=0; 
	//! 拉低至少18ms	
    delay_ms(20); 
	//! 拉高DQ   	
    DHT11_DQ_OUT=1; 
	//! 主机拉高20~40us	 
	delay_us(30);     	
}
//********************************************************************//
//! 函数名:DHT11_Check
//! 功能:等待dht11响应
//! 输入:none
//! 输出:dht11是否响应;1:未检测到dht11; 0:检测到dht11
//********************************************************************//
u8 DHT11_Check() 	   
{   
	u8 retry=0;
	//! 输入模式
	DHT11_IO_IN();
	//! DHT11会拉低40~50us	 
    while (DHT11_DQ_IN&&retry<100)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
	//! DHT11会拉低后会再次拉高40~50us
    while (!DHT11_DQ_IN&&retry<100)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}
//********************************************************************//
//! 函数名:DHT11_Read_Bit
//! 功能:从dht11读取一bit数据
//! 输入:none
//! 输出:一bit数据:0; 1;
//********************************************************************//
u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	 //! 等待变成低电平 12-14us 开始
	while(DHT11_DQ_IN&&retry<100)
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	//! 等待变成高电平 26-28us表示0,116-118us表示1
	while(!DHT11_DQ_IN&&retry<100)
	{
		retry++;
		delay_us(1);
	}
	//! 等待40us
	delay_us(40);
	if(DHT11_DQ_IN)
		return 1;
	else 
		return 0;		   
}
//********************************************************************//
//! 函数名:DHT11_Read_Byte
//! 功能:从dht11读取一byte数据
//! 输入:none
//! 输出:一byte数据
//********************************************************************//
u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}
//********************************************************************//
//! 函数名:DHT11_Read_Data
//! 功能:从dht11读取一次数据
//! 输入:temp:储存温度数据指针; humi:存储温度数据指针
//! 输出:是否读取成功;1:失败;0:成功
//********************************************************************//
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		//! 读取5byte数据
		for(i=0;i<5;i++)
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
		
	}
	else 
		return 1;
	return 0;	    
}
//********************************************************************//
//! 函数名:DHT11_IO_OUT
//! 功能:配置dht11为输出
//! 输入:none
//! 输出:none
//********************************************************************//
void DHT11_IO_OUT()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT11;
	//! 推挽输出
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);	
}
//********************************************************************//
//! 函数名:DHT11_IO_IN
//! 功能:配置dht11为输入
//! 输入:none
//! 输出:none
//********************************************************************//
void DHT11_IO_IN()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT11;
	//! 上拉输入模式
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	 
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);	
}





