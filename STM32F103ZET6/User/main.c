#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "LED.h"
#include "lcd.h"
#include "image2lcd.h"
#include "dht11.h"
#include "zph01.h"
#include "time.h"
//! 屏蔽编译器对LCD显示中文字符串函数的警告
#pragma  diag_suppress 870

u8 pm25_lcd[6]={0},hh06_lcd[6]={0},dht11_flag=0;
//! SWPU图标
extern const u8 gImage_swpu[];
//! 公交E图标                                   
extern const u8 gImage_gongjiaoe[];
//! 定时器3标志,中断一次加一                              
extern u8 time3_flag,usart_time,time_hours,time_minutes,time_seconds;
//! 串口2数据缓存Buffer和标志,串口1噪音数据,串口2 pm2.5数据,串口3 GSM模块数据
extern u8  USART1_RX_BUF[7],USART2_RX_BUF[10],USART3_RX_BUF[75];   
extern u16 USART1_RX_STA,USART2_RX_STA,USART3_RX_STA;
//! 温湿度
u8 temperature=0,humidity=0;  
//! pm2.5,噪音数据的整数和小数                                     
u8 pm25_zheng=0,pm25_xiao=0,hh06_zheng=0,hh06_xiao=0; 
//! 图片显示标志             
u8 Image_flag=0;                                                   

void lcd_display(void);
void data_pros(void);
void data_pm25(void);
void data_hh06(void);
//! 串口1和串口2使能,禁止函数
void disadle_uart(void);  
void enadle_uart(void);

//********************************************************************//
//! 函数名:main
//! 功能:主函数
//! 输入:none
//! 输出:none
//********************************************************************//
int main(void)
{	
	//! flag_dht11为读取温湿度数据延时三个主周期而设置,主循环周期大于300ms
	u8 CheckSum=0,j=0,flag_dht11=0;  
	//! GSM模块上传数据buffer
	u8 send[36]={0};
	//! 中断优先级配置,四个抢占优先级,四个副优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//! LCD初始化
	LCD_Init();
	//! LED灯GPIO初始化	
	LED_GPIO_Config();
	//! 延时1.5秒
  delay_ms(1500);
	//! 开启LCD显示
	lcd_display();
	//! 串口1初始化 噪音数据接口
	uart1_init(115200);
	//! 串口2初始化 pm2.5数据接口
	uart2_init(9600);
	//! 串口3初始化 GSM模块
	uart3_init(115200);
	//! 延时1秒
	delay_ms(1000);
	dht11_flag=1;
	//! 初始化DHT11
	if(DHT11_Init())
	{
		dht11_flag=1;
	}
	else
	{
		//! 100 纵坐标,10 横坐标
		LCD_ShowString(100,10,80,24,24,"Error");
		dht11_flag=0;
	}
	//! 定时器3初始化,定时1秒
	time3_init(2000,35999); 
	delay_ms(500);
	//! 关闭串口1,2
	disadle_uart();	
	//! 主循环
	while(1)
	{	
		//! DHT11读取计数
		flag_dht11++;
		//! 点亮LED2
		LED2_ON;
		//! 点亮LED1
		LED1_ON;
		//! 温度采集周期大概900ms
		if(flag_dht11==3)
		{ 
			//! 清标志
			flag_dht11=0;
			//! DHT11数据读取过程大于100ms	
		  data_pros();  	 
		}
		//! 只允许在这300ms开启串口,读取传感器数据
		enadle_uart();   
		delay_ms(150);
		LED1_OFF;
		delay_ms(150);
		disadle_uart();
		//! 噪音数据读取成功标志
		if(USART1_RX_STA)
		{
			USART1_RX_STA=0;
			data_hh06();			
		}
		if(usart_time==15)
		{
			usart_time=0;
			Image_flag=~Image_flag;
			//! 延时5ms是为了解决显示器死掉的问题,大概原因是刷新LCD的频率太高,
			//! 根本原因与中断中更新屏幕内容有关,如果在主函数中正在更新屏幕,但是中断又更新,可能出现错误
			delay_ms(5); 
			//! 在坐标(0,160)到坐标(319,239)区域填充白色                                   
			LCD_Fill(0,167,319,239,WHITE);
			//! 图标刷新标志有效,并且定时器没进定时器中断                  
			if(Image_flag && (TIM_GetITStatus(TIM3,TIM_IT_Update)!=SET))
			{
				delay_ms(5);
				//! 显示SWPU图标
				image_display(45,167,(u8*) gImage_swpu);     
			}
			if((Image_flag==0)&&(TIM_GetITStatus(TIM3,TIM_IT_Update)!=SET))
			{
				//! 显示公交E图标
				delay_ms(5);
				image_display(45,167,(u8*) gImage_gongjiaoe);
			}
	   		 delay_ms(50);
			//! 实际请求网址ַ:http://xkmg9m2k.qcloud.la/weapp/usrcloud?a=255&b=1&c=18&d=15&e=1&f=12&g=50&h=3&i=0  
			//! 域名已在GSM模块中配置,这里仅需配置参数
			if(dht11_flag==1)   //get请求参数:a=255&b=1&c=18&d=15&e=1&f=12&g=50&h=3&i=0
			{
       		 send[0]='a';
				send[1]='=';
				send[2]=0xFF;
				send[3]='&';
				send[4]='b';
				send[5]='=';
				//! 需加48转换成对应的ASCII码,否则服务器无法读出数据
				send[6]=1+48;          
				send[7]='&';
				send[8]='c';
				send[9]='=';
				send[10]=temperature+48;
				send[11]='&';
				send[12]='d';
				send[13]='=';
				send[14]=humidity+48;
				send[15]='&';
				send[16]='e';
				send[17]='=';
				send[18]=pm25_zheng+48;
				send[19]='&';
				send[20]='f';
				send[21]='=';
				send[22]=pm25_xiao+48;
				send[23]='&';
				send[24]='g';
				send[25]='=';
				send[26]=hh06_zheng+48;
				send[27]='&';
				send[28]='h';
				send[29]='=';
				send[30]=hh06_xiao+48;
				send[31]='&';
				send[32]='i';
				send[33]='=';
				send[34]=0+48;
				send[35]='\0';
				//! 向服务器发送请求
				USART_SendString(USART3,send);  
			}
			else
			 {
				 send[0]='a';
				 send[1]='=';
				 send[2]=0xfe;
				 send[35]='\0';
				 //! 向服务器发送错误标志
         		 USART_SendString(USART3,send);
       		}
			 enadle_uart();
		}
	//! 串口收到pm2.5数据
    if(USART2_RX_STA)
    {
			USART2_RX_STA=0;
			CheckSum=FucCheckSum(USART2_RX_BUF,9);
			if(CheckSum==USART2_RX_BUF[8])
			{
        data_pm25();
			}
		}	
	//! GMS串口模块,数据为服务器返回的字符串:
	//! {"a":0,"b":0,"c":0,"d":0,"e":0,"f":2,"g":12,"h":21,"i":22,"j":2}
	//! a为系统状态;b为公交车状态;c为到达时间;d为车到站牌距离的整数;e为距离的小数;f为服务器时间星期几;
	//! g为服务器时间日期;hij分别为服务器时间时分秒
	//! 串口收到一帧服务器的回复标志,站牌每15秒向服务器发送一次请求,请求发送后需要十二三秒才能收到回复(2G网络)
	//! 返回数据为字符串,长度小于75字节但是不固定
    if(USART3_RX_STA) 
		{
			u8 m=0;
			u8 bus_flag=48,system_flag=48,arrivaltime=0,distance_h=0,distance_l=0,day=0,date=0,hours=0,minutes=0,seconds=0;
			LED2_OFF;
			USART3_RX_STA=0;
			for(m=0;m<75;m++)
			{
				//! 系统状态
				if(USART3_RX_BUF[m]=='a')
				{
					bus_flag=USART3_RX_BUF[m+3];
					  if(bus_flag-48==1)
						{
							GUI_Chinese_Text(188,78,"运行中",6,0x0000,0xffff);
						}
						else 
						GUI_Chinese_Text(188,78,"待发车",6,0x0000,0xffff);
					} 
				//! 公交车状态
				if(USART3_RX_BUF[m]=='b')
				{
					system_flag=USART3_RX_BUF[m+3];
					  if(system_flag-48==1)
						{
							LCD_Fill(114,30,122,46,WHITE);
							LCD_ShowString(90,30,24,16,16,"OK!");	
						}
						else
							LCD_ShowString(90,30,32,16,16,"OFF!");
					}
				//! 公交车到达站台时间,单位:分钟
        if(USART3_RX_BUF[m]=='c')
				{
					//! 区分到达时间分钟数为一位还是两位,需做不同处理
					//! 分钟数为一位
					if(USART3_RX_BUF[m+4]==',')
					{       
						arrivaltime=USART3_RX_BUF[m+3]-48;
						LCD_Fill(133,78,141,94,WHITE);
						LCD_ShowxNum(141,78,arrivaltime,1,16,0);
						//! 分钟数为两位
					}
					else
					{
						arrivaltime=(USART3_RX_BUF[m+3]-48)*10+USART3_RX_BUF[m+4]-48;
					  LCD_ShowxNum(133,78,arrivaltime,2,16,0);
					 }
					}
				//! 公交车到站台的距离整数,单位:米	
        if(USART3_RX_BUF[m]=='d')
				{
					if(USART3_RX_BUF[m+4]==',')
					{
						distance_h=USART3_RX_BUF[m+3]-48;
						LCD_Fill(61,78,69,94,WHITE);
						//! LCD显示区域,按像素点计算,下同
						//! 69=57+4+8
						LCD_ShowxNum(69,78,distance_h,1,16,0); 
					}else{
					  distance_h=(USART3_RX_BUF[m+3]-48)*10+USART3_RX_BUF[m+4]-48;
						//! 61=57+4
					  LCD_ShowxNum(61,78,distance_h,2,16,0); 
					 }
					}
				//! 公交车到站台的距离小数,单位:米	
        if(USART3_RX_BUF[m]=='e')
				{
					if(USART3_RX_BUF[m+4]==',')
					{
						distance_l=USART3_RX_BUF[m+3]-48;
						LCD_Fill(85,78,93,94,WHITE);
						//!　77=61+16
						LCD_ShowxNum(77,78,distance_l,1,16,0); 
					}
					else
					{
					  distance_l=(USART3_RX_BUF[m+3]-48)*10+USART3_RX_BUF[m+4]-48;
						//! 77=61+16
						LCD_ShowxNum(77,78,distance_l,2,16,0); 
					}
				}
				//! 当前服务器时间星期几
				if(USART3_RX_BUF[m]=='f')
				{
					day=USART3_RX_BUF[m+3]-48;
					switch(day)
					{
						case 1:
							//! 162+2*8+3=181
							GUI_Chinese_Text(181,10,"星期一",6,0x0000,0xffff);  
							break;
						case 2:
							//! 162+2*8+3=181
							GUI_Chinese_Text(181,10,"星期二",6,0x0000,0xffff); 
							break;
						case 3:
							//! 162+2*8+3=181
							GUI_Chinese_Text(181,10,"星期三",6,0x0000,0xffff);   
							break;
						case 4:
							//! 162+2*8+3=181
							GUI_Chinese_Text(181,10,"星期四",6,0x0000,0xffff);  
							break;
						case 5:
							//! 162+2*8+3=181
							GUI_Chinese_Text(181,10,"星期五",6,0x0000,0xffff); 
							break;
						case 6:
							//! 162+2*8+3=181
							GUI_Chinese_Text(181,10,"星期六",6,0x0000,0xffff); 
							break;
						case 7:
							//! 162+2*8+3=181
							GUI_Chinese_Text(181,10,"星期日",6,0x0000,0xffff);  
							break;
						default:
							break; 
					}
				}
				//! 当前服务器时间日期
				if(USART3_RX_BUF[m]=='g')
				{
					if(USART3_RX_BUF[m+4]==',')
					{
						date=USART3_RX_BUF[m+3]-48;
						LCD_Fill(82,10,90,26,WHITE);
						//! 90=82+8
						LCD_ShowxNum(90,10,date,1,16,0);                 
					}
					else
					{
						date=(USART3_RX_BUF[m+3]-48)*10+USART3_RX_BUF[m+4]-48;
						//! 66+16=82
					  LCD_ShowxNum(82,10,date,2,16,0);                  
						}			
					}
				//! 服务器当前时间:时
				if(USART3_RX_BUF[m]=='h')
				{
					if(USART3_RX_BUF[m+4]==',')
					{
						hours=USART3_RX_BUF[m+3]-48;
						time_hours=hours;
						//! 98+16=114
						LCD_ShowxNum(114,10,0,1,16,0); 
						//! 98+16+8=122                   
						LCD_ShowxNum(122,10,hours,1,16,0);                
					}
					else
					{
						hours=(USART3_RX_BUF[m+3]-48)*10+USART3_RX_BUF[m+4]-48;
					  time_hours=hours;
						//! 98+16=114
						LCD_ShowxNum(114,10,hours,2,16,0);                
					}
					
				}
				//! 服务器时间:分
				if(USART3_RX_BUF[m]=='i')
				{
					if(USART3_RX_BUF[m+4]==',')
					{
						minutes=USART3_RX_BUF[m+3]-48;
						time_minutes=minutes;
						//! 130+8=138
						LCD_ShowxNum(138,10,0,1,16,0);
						//! 130+8+8=146                      
						LCD_ShowxNum(146,10,minutes,1,16,0);                
					}
					else
					{
						minutes=(USART3_RX_BUF[m+3]-48)*10+USART3_RX_BUF[m+4]-48;
					  time_minutes=minutes;
						//! 130+8=138
						LCD_ShowxNum(138,10,minutes,2,16,0);                
					}
				}
				//! 服务器时间:秒
				if(USART3_RX_BUF[m]=='j')
				{
					//! 通过判断数据结束大括弧位置确定
					if(USART3_RX_BUF[m+4]=='}')
					{                     
						seconds=USART3_RX_BUF[m+3]-48;
						time_seconds=seconds;
						//! 154+8=162
						LCD_ShowxNum(162,10,0,1,16,0); 
						//! 154+8+8=170               
						LCD_ShowxNum(170,10,seconds,1,16,0);                
					}
					else
					{
						seconds=(USART3_RX_BUF[m+3]-48)*10+USART3_RX_BUF[m+4]-48;
					  time_seconds=seconds;
						//! 154+8=162
						LCD_ShowxNum(162,10,seconds,2,16,0);                
					}
				}
			}
			//! 清空数据buffer
      for(j=0;j<=75;j++)
			{
				USART3_RX_BUF[j]=0;
			}
		}
	}	
}
//********************************************************************//
//! 函数名:lcd_display
//! 功能:LCD固定内容显示,初始化完成调用一次
//! 输入:none
//! 输出:none
//********************************************************************//
void lcd_display(void)
{
	//! 清屏
	LCD_Clear(WHITE);
	//! 显示默认时间,数据依次含义:开始显示点横纵坐标;要显示的数;数的位数;区域大小;叠加方式
	LCD_ShowxNum(10,10,2018,4,16,1); 
	//! 4*8+10=42              
	GUI_Chinese_Text(42,10,"年",2,0x0000,0xffff);
	//! 42+16=58        
	LCD_ShowxNum(58,10,6,1,16,1);
	//! 58+8=66                         
	GUI_Chinese_Text(66,10,"月",2,0x0000,0xffff); 
	//! 66+16=82        
	LCD_ShowxNum(82,10,7,2,16,1); 
	//! 82+8*2=98                        
	GUI_Chinese_Text(98,10,"日",2,0x0000,0xffff);
	//! 98+16=114         
	LCD_ShowxNum(114,10,13,2,16,1); 
	//! 114+2*8=130                      
	LCD_ShowString(130,9,8,16,16,":");
	//! 130+8=138                    
	LCD_ShowxNum(138,10,47,2,16,1);
	//! 138+2*8=154                       
	LCD_ShowString(154,9,8,16,16,":"); 
	//! 154+8=162                   
	LCD_ShowxNum(162,10,50,2,16,1);
	//! 162+2*8+3=181                        
	GUI_Chinese_Text(181,10,"星期四",6,0x0000,0xffff);
	//! 行间距4像素,第二行30=16+10+4   
	GUI_Chinese_Text(10,30,"系统状态",10,0x0000,0xffff);
	//! 10+5*16=90 
	LCD_ShowString(90,30,32,16,16,"OFF!");                 

  //! 画表格
  //! (0,50)-->(239,50) 50=30+16+4
	LCD_DrawLine(0,50,239,50); 
	//! (0,74)-->(239,74)                           
	LCD_DrawLine(0,74,239,74);
	//! (0,98)-->(239,98)                            
	LCD_DrawLine(0,98,239,98);
	//! (0,122)-->(239,122)                            
	LCD_DrawLine(0,122,239,122);
	//! (57,50)-->(57,122)                          
	LCD_DrawLine(57,50,57,122);
	//! (114,50)-->(114,122)                            
	LCD_DrawLine(114,50,114,122); 
	//! (184,50)-->(184,122)                         
	LCD_DrawLine(184,50,184,122);                          
	
	//! 填充表格内容
	GUI_Chinese_Text(12,54,"线路",4,0x0000,0xffff);
	GUI_Chinese_Text(70,54,"距离",4,0x0000,0xffff);
	GUI_Chinese_Text(133,54,"时间",4,0x0000,0xffff);
	GUI_Chinese_Text(196,54,"状态",4,0x0000,0xffff);
	LCD_ShowString(12,78,16,16,16,"XX");
	GUI_Chinese_Text(28,78,"路",2,0x0000,0xffff);
	
	//! 93=89+4
	GUI_Chinese_Text(93,78,"米",2,0x0000,0xffff);  
	GUI_Chinese_Text(117,78,"约",2,0x0000,0xffff);
	
	GUI_Chinese_Text(149,78,"分钟",4,0x0000,0xffff);
	GUI_Chinese_Text(188,78,"待发车",6,0x0000,0xffff);
	
	//! 0~3共四像素
	GUI_Chinese_Text(3,126,"气温",6,0x0000,0xffff);
	//! 67=3+16*3+8*2    
	GUI_Chinese_Text(67,126,"度",2,0x0000,0xffff);
	//! 87=67+16+4      
	GUI_Chinese_Text(87,126,"颗粒物",8,0x0000,0xffff);
	//! 199=87+4*16+6*8
	LCD_ShowString(199,126,40,16,16,"ug/m3"); 
	//! 146=126+16+4          
  GUI_Chinese_Text(3,146,"湿度",6,0x0000,0xffff);
	//! 67=51+16   
	LCD_ShowString(67,146,24,16,16,"%rh"); 
	//! 103=87+3*16-2*16             
	GUI_Chinese_Text(103,146,"噪音",6,0x0000,0xffff); 
	LCD_ShowString(191,146,16,16,16,"dB");
	
}
//********************************************************************//
//! 函数名:data_pros
//! 功能:温湿度数据处理
//! 输入:none
//! 输出:none
//********************************************************************//
void data_pros(void)	
{
	u8 temp;  	    
	u8 humi;
	u8 temp_buf[3],humi_buf[3];
	//! 关闭定时器,避免打断数据处理过程
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE); 
	//! 温湿度数据读取核心函数,模拟dht11的通信时序,要求us级精确延时
	DHT11_Read_Data(&temp,&humi);
	//! 打开定时器               
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); 
	temp_buf[0]=temp/10+0x30;	
	temp_buf[1]=temp%10+0x30;
	temp_buf[2]='\0';
	delay_ms(5);
	//! 51=3+16*3
	LCD_ShowString(51,126,16,16,16,temp_buf);	
	humi_buf[0]=humi/10+0x30;	
	humi_buf[1]=humi%10+0x30;
	humi_buf[2]='\0';
	delay_ms(5);
	//! 51=3+6*8
  LCD_ShowString(51,146,16,16,16,humi_buf); 
  temperature = temp;
  humidity = 	humi;
}
//********************************************************************//
//! 函数名:data_pm25
//! 功能:pm2.5数据处理
//! 输入:none
//! 输出:none
//********************************************************************//
void data_pm25(void)  
{
	u8 z=0,x=0;
	u32 density=0; 
	//! 整数部分
	z=USART2_RX_BUF[3]; 
	//! 小数部分  
	x=USART2_RX_BUF[4];   
	density=z*20*100 + x*20;
	pm25_zheng=z;
	pm25_xiao=x;

	pm25_lcd[0]=(u8)(density/1000)+0x30;
	pm25_lcd[1]=(u8)(density%1000/100)+0x30;
	pm25_lcd[2]='.';
	pm25_lcd[3]=(u8)(density%100/10)+0x30;
	pm25_lcd[4]=(u8)(density%10)+0x30;
	pm25_lcd[5]='\0';
	delay_ms(5);
	LCD_ShowString(151,126,40,16,16,pm25_lcd);
}
//********************************************************************//
//! 函数名:data_hh06
//! 功能:噪音数据处理
//! 输入:none
//! 输出:none
//********************************************************************//
void data_hh06(void)    
{
	u8 h,l;
	u16 date=0;
	h=USART1_RX_BUF[4];
	l=USART1_RX_BUF[3];
	date=((u16)(h)<<8)|(u16)(l);
	hh06_zheng=(u8)(date/1000)*100+(u8)((date%1000)/100)*10+(u8)((date%100)/10);
	hh06_xiao=(u8)(date%10);
	hh06_lcd[0]=(u8)(date/1000)+0x30;
	hh06_lcd[1]=(u8)((date%1000)/100)+0x30;
	hh06_lcd[2]=(u8)((date%100)/10)+0x30;
	hh06_lcd[3]='.';
	hh06_lcd[4]=(u8)(date%10)+0x30;
	hh06_lcd[5]='\0';
	delay_ms(3);
	LCD_ShowString(151,146,40,16,16,hh06_lcd);
	delay_ms(3);
}
//********************************************************************//
//! 函数名:disadle_uart
//! 功能:关闭串口1,2
//! 输入:none
//! 输出:none
//********************************************************************//
void disadle_uart(void)
{
	USART_Cmd(USART1,DISABLE);
	USART_Cmd(USART2,DISABLE);
}
//********************************************************************//
//! 函数名:enadle_uart
//! 功能:开启串口1,2
//! 输入:none
//! 输出:none
//********************************************************************//
void enadle_uart(void)
{
	USART_Cmd(USART1,ENABLE);
	USART_Cmd(USART2,ENABLE);
}


