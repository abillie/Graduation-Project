#include "sys.h"
#include "usart.h"
#include "led.h"


//! 加入以下代码,支持prfintf函数,而不需要选择 use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//! 标准库需要的支持函数               
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//! 定义 _sys_exit() 避免使用半主机模式
_sys_exit(int x) 
{ 
	x = x; 
} 
//! 重定义fputc函数
//! 循环发送,直到发送完毕
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0); 
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLibd的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
//! 串口2,3数据接收和缓冲buffer
u8 USART2_RX_BUF[10]={0},USART3_RX_BUF[75]={0},number_USART2=0,number_USART3=0;  
//! 串口2,3数据接收完成标志
u16 USART2_RX_STA=0,USART3_RX_STA=0;                         

//! 串口数据接收buffer 	
u8 USART1_RX_BUF[7],number_USART1=0; 
//! 串口1接收状态标志
u16 USART1_RX_STA=0;      
         
//********************************************************************//
//! 函数名:uart1_init
//! 功能:串口1初始化，接收噪音传感器数据
//! 输入:bound:串口波特率
//! 输出:none
//********************************************************************//  
void uart1_init(u32 bound){
  //! 串口GPIO初始化
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//! 使能串口1,GPIO1时钟 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
  
	//! USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//! 复用推挽输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//! 初始化PA.9	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //! USART1_RX	  GPIOA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	//! 浮空输入
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//! 初始化PA.10
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //! Usart1 NVIC 中断优先级配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	//! 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	//! 子优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	//! 使能IRQ通道		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//! 初始化VIC寄存器			
	NVIC_Init(&NVIC_InitStructure);	
  
  //! USART1初始化
	//! 波特率
	USART_InitStructure.USART_BaudRate = bound;
	//! 8位数据格式
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//! 一个停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//! 无奇偶校验位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	//! 无硬件数据流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//! 收发模式
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	//! 初始化串口1
  USART_Init(USART1, &USART_InitStructure); 
	//! 清除串口1中断标志                 
	USART_ClearFlag(USART1,USART_FLAG_TC);
	//! 开启串口接收中断
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//! 使能串口1
  USART_Cmd(USART1, ENABLE); 
}
//********************************************************************//
//! 函数名:USART1_IRQHandler
//! 功能:串口1中断服务函数，接收噪音传感器数据
//! 输入:none
//! 输出:none
//********************************************************************// 
void USART1_IRQHandler(void)                
	{
		u8 temp;
    if(USART_GetITStatus(USART1, USART_IT_RXNE)==SET)  
		{      
				temp=USART_ReceiveData(USART1);
				//! 帧头(可能有数据等于帧头而接收出错的风险)
				if((temp==0XBB)||(number_USART1>0))
				{
					USART1_RX_BUF[number_USART1++]=temp;
					{
						//! 一帧6字节
						if((number_USART1==6)&&(USART1_RX_BUF[2]==0x01)) 
						{
							USART1_RX_STA=1;
							number_USART1=0;
						}
					}
				}									
		}
} 
//********************************************************************//
//! 函数名:uart2_init
//! 功能:串口2初始化，接收pm2.5传感器数据
//! 输入:bound：波特率
//! 输出:none
//********************************************************************// 
void uart2_init(u32 bound)
{
  //! GPIO初始化结构体
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	//! 使能GPIOA和USART2时钟 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	//! USART2_TX   GPIOA.2初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //! USART2_RX	  GPIOA.3初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //! USART2 NVIC 中断优先级配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	//! 抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	//! 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	//! IRQ通道使能	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	//! 根据指定参数初始化VIC寄存器		
	NVIC_Init(&NVIC_InitStructure);	
  
   //! USART2 初始化设置
	 //! 串口波特率配置
	USART_InitStructure.USART_BaudRate = bound;
	//! 8位数据长度数据格式
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//! 1位停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//! 无奇偶效验位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	//! 无硬件数据流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//! 收发模式
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

	//! 初始化串口2
  USART_Init(USART2, &USART_InitStructure); 
	//! 清除串口2中断标志                 
	USART_ClearFlag(USART2,USART_FLAG_TC);
	//! 使能中断
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//! 使能串口
  USART_Cmd(USART2, ENABLE);                 

}
//********************************************************************//
//! 函数名:USART2_IRQHandler
//! 功能:串口2中断服务函数,pm2.5数据接收
//! 输入:none
//! 输出:none
//********************************************************************// 
void USART2_IRQHandler(void)  
{  
		u8 temp;
		if(USART_GetITStatus(USART2, USART_IT_RXNE)==SET)  
		{     
				temp=USART_ReceiveData(USART2);
				//! 帧头0xff;(数据有接收出错的风险)
				if((temp==0XFF)||(number_USART2>0))
				{
					USART2_RX_BUF[number_USART2++]=temp;
					{
						//! 一帧数据9字节
						if(number_USART2==9)
						{
							USART2_RX_STA=1;
							number_USART2=0;
						}
					}
				}									
		}   
} 
//********************************************************************//
//! 函数名:uart3_init
//! 功能:串口3初始化;
//! 输入:none 
//! 输出:none
//********************************************************************// 
void uart3_init(u32 bound)
{
  //! GPIO初始化结构体
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	//! 开启GPIOB和USART3时钟 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	//! USART3_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  //! USART3_RX	  GPIOB.11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  

  //! Usart3 NVIC 中断优先级配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	//! 抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	//! 子优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	//! IRQ通道使能		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//! 根据指定参数初始化VIC寄存器	
	NVIC_Init(&NVIC_InitStructure);	
  
	//! USART3初始化
	//! 波特率配置
	USART_InitStructure.USART_BaudRate = bound;
	//! 8位数据长度格式
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//! 停止位1位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//! 无奇偶校验位
	USART_InitStructure.USART_Parity = USART_Parity_No;
	//! 无硬件数据流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//! 收发模式
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

	//! 初始化串口3
  USART_Init(USART3, &USART_InitStructure);
	//! 清除串口3中断标志                 
	USART_ClearFlag(USART3,USART_FLAG_TC);
	//! 开启串口3接收中断 
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	//! 使能串口3
  USART_Cmd(USART3, ENABLE);              

}
//********************************************************************//
//! 函数名:USART3_IRQHandler
//! 功能:串口3中断服务函数;
//! 输入:none 
//! 输出:none
//********************************************************************//
void USART3_IRQHandler(void)  
{ 
	u8 Res=0; 
	if(USART_GetITStatus(USART3, USART_IT_RXNE)==SET)
	{
		Res=USART_ReceiveData(USART3);
		if((Res=='{')||(number_USART3>0))
		{
			//! 读取数据到buffer
			USART3_RX_BUF[number_USART3++] =Res;
			//! 数据长度小于75字节并且以"}"结束,所以使用"}"判断数据是否接收完毕	
			if((Res=='}')||(number_USART3==75))
			{
					USART3_RX_STA=1;
					number_USART3=0;
			}
		}				
	}  
}
