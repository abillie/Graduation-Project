#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"	 
#include "delay.h"
//! 字库	
#include "HzLib_65k.h" 
//--------------------------------------------------------------//
//! 本程序由淘宝商家提供,仅供学习使用;
//! 正点原子@ALIENTEK
//! 技术论坛:www.openedv.com
//! 创建日期:2010/7/4
//! 版本 V2.9
//! 版权所有,盗版必究.
//! Copyright(C) 广州市星翼电子有限公司
//! All rights reserved
//! 主要用于2.4寸,2.8寸,3.5寸,4.3寸,7寸 TFT液晶屏驱动
//! 支持驱动芯片型号: ILI9341,ILI9325,RM68042,RM68021,ILI9320,ILI9328,LGDP4531,LGDP4535
//! SPFD5408,1505,B505/C505,NT35310,NT35510,SSD1963	 
//--------------------------------------------------------------//
//! LCD画笔颜色 红色 
u16 POINT_COLOR=0x0000;	
//! 显示字符串时的背景颜色 白色
u16 BACK_COLOR=0xFFFF; 
//! 默认竖屏
_lcd_dev lcddev;
	 
//********************************************************************//
//! 函数名:LCD_WR_REG
//! 功能:写寄存器函数
//! 输入:regval 寄存器序号
//! 输出:none
//********************************************************************//
void LCD_WR_REG(u16 regval)
{   
	LCD->LCD_REG=regval;
}
//********************************************************************//
//! 函数名:LCD_WR_DATA
//! 功能:写LCD数据
//! 输入:data 写入的数据值
//! 输出:none
//********************************************************************//
void LCD_WR_DATA(u16 data)
{	 
	LCD->LCD_RAM=data;
}
//********************************************************************//
//! 函数名:LCD_RD_DATA
//! 功能:读LCD数据
//! 输入:none
//! 输出:ram 读取结果
//********************************************************************//
u16 LCD_RD_DATA(void)
{
	//! 防止编译器优化
	vu16 ram;			
	ram=LCD->LCD_RAM;	
	return ram;	 
}
//********************************************************************//
//! 函数名:LCD_WriteReg
//! 功能:写寄存器
//! 输入:LCD_Reg 寄存器地址;LCD_RegValue 要写入的数据
//! 输出:none
//********************************************************************//
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{
	//! 寄存器序号
	LCD->LCD_REG = LCD_Reg;
	//! 写入的数据		 
	LCD->LCD_RAM = LCD_RegValue;
}
//********************************************************************//
//! 函数名:LCD_ReadReg  
//! 功能:读寄存器
//! 输入:LCD_Reg 寄存器地址;
//! 输出:读到的数据
//********************************************************************//
u16 LCD_ReadReg(u16 LCD_Reg)
{
  //! 写入要读的寄存器序号
	LCD_WR_REG(LCD_Reg);
	delay_us(5);
  //! 返回读取结果		  
	return LCD_RD_DATA();
}
//********************************************************************//
//! 函数名:LCD_WriteRAM_Prepare
//! 功能:开始写GRAM
//! 输入:none
//! 输出:none
//********************************************************************//
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;	  
}	 
//********************************************************************//
//! 函数名:LCD_WriteRAM
//! 功能:LCD写GRAM
//! 输入:RGB_Code 颜色值
//! 输出:none
//********************************************************************//
void LCD_WriteRAM(u16 RGB_Code)
{					
	//! 写十六位GRAM		    
	LCD->LCD_RAM = RGB_Code;
}
//********************************************************************//
//! 函数名:LCD_BGR2RGB
//! 功能:GBR转RGB 注:函数命名有错
//! 输入:c GBR格式颜色值
//! 输出:rgb RGB格式颜色值
//********************************************************************//
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 
//********************************************************************//
//! 函数名:opt_delay
//! 功能:延时函数 注:在编译器MDK -O1 时间优化时需配置
//! 输入:i 延时值
//! 输出:none
//********************************************************************//
void opt_delay(u8 i)
{
	while(i--);
}
//********************************************************************//
//! 函数名:LCD_ReadPoint
//! 功能:读取某坐标点的颜色值
//! 输入:x,y 坐标
//! 输出:该点颜色值
//********************************************************************//
u16 LCD_ReadPoint(u16 x,u16 y)
{
 	u16 r=0,g=0,b=0;
	 //! 坐标无效,返回0
	if(x>=lcddev.width||y>=lcddev.height)
		return 0;
	//! 设置光标位置	   
	LCD_SetCursor(x,y);
	//! 向9341,6804,3510,1963芯片发送读GRAM指令 	    
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X1963)
		LCD_WR_REG(0X2E);
	else 
		if(lcddev.id==0X5510)
			LCD_WR_REG(0X2E00);
		else 
		  //! 发送读GRAM指令
			LCD_WR_REG(0X22);      		 			
		if(lcddev.id==0X9320)
			opt_delay(2);				   
 	r=LCD_RD_DATA();
	//! 1963直接读
	if(lcddev.id==0X1963)
		return r;	
	opt_delay(2);
	//! 实际坐标颜色	  
 	r=LCD_RD_DATA();
	 //! 9341,NT35310,NT35510分两次读出  		  			
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)
 	{
		opt_delay(2);	  
		b=LCD_RD_DATA(); 
		//! 第一次读的是RG值,R在前,G在后,各占8位
		g=r&0XFF;	
		g<<=8;
	} 
	if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0XB505||lcddev.id==0XC505)
		//! 直接返回颜色
		return r;
	else 
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)
		//! ILI9341,NT35310,NT35510需要公式计算
		return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));
	//! 其他驱动IC
	else 
		return LCD_BGR2RGB(r);	
}			 
//********************************************************************//
//! 函数名:LCD_DisplayOn
//! 功能:开启LCD显示
//! 输入:none
//! 输出:none
//********************************************************************//
void LCD_DisplayOn(void)
{					   
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X1963)
	  //! 开启9341,6804,5310,1963显示
		LCD_WR_REG(0X29);	
	else 
	if(lcddev.id==0X5510)
		//! 开启5510显示
		LCD_WR_REG(0X2900);	
	else 
		//! 其他驱动IC
		LCD_WriteReg(0X07,0x0173); 
}	 
//********************************************************************//
//! 函数名:LCD_DisplayOff
//! 功能:关闭LCD显示
//! 输入:none
//! 输出:none
//********************************************************************//
void LCD_DisplayOff(void)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X1963)
		//! 关闭9341,6804,5310,1963显示
		LCD_WR_REG(0X28);
	else 
	if(lcddev.id==0X5510)
		//! 关闭5510显示
		LCD_WR_REG(0X2800);
	else 
		//! 其他驱动IC
		LCD_WriteReg(0X07,0x0);
}   
//********************************************************************//
//! 函数名:LCD_SetCursor
//! 功能:设置光标位置
//! 输入:Xpos 横坐标;Ypos 纵坐标
//! 输出:none
//********************************************************************//
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
 	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{		    
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);
		LCD_WR_DATA(Xpos&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_DATA(Ypos&0XFF); 		
	}else 
	if(lcddev.id==0X6804)
	{
		//! 横屏处理
		if(lcddev.dir==1)
			Xpos=lcddev.width-1-Xpos;
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);
		LCD_WR_DATA(Xpos&0XFF); 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_DATA(Ypos&0XFF); 
	}else 
	if(lcddev.id==0X1963)
	{ 
		//! 变换坐标 			 		
		if(lcddev.dir==0)
		{
			Xpos=lcddev.width-1-Xpos;
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);
			LCD_WR_DATA(0); 		
			LCD_WR_DATA(Xpos>>8);
			LCD_WR_DATA(Xpos&0XFF);		 	 
		}else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(Xpos>>8);
			LCD_WR_DATA(Xpos&0XFF); 		
			LCD_WR_DATA((lcddev.width-1)>>8);
			LCD_WR_DATA((lcddev.width-1)&0XFF);		 	 			
		}	
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_DATA(Ypos&0XFF); 		
		LCD_WR_DATA((lcddev.height-1)>>8);
		LCD_WR_DATA((lcddev.height-1)&0XFF); 			 		
		
	}else 
	if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(Xpos>>8); 		
		LCD_WR_REG(lcddev.setxcmd+1);
		LCD_WR_DATA(Xpos&0XFF);			 
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(Ypos>>8);  		
		LCD_WR_REG(lcddev.setycmd+1);
		LCD_WR_DATA(Ypos&0XFF);			
	}else
	{
		//! 横屏调转X,Y坐标
		if(lcddev.dir==1)
			Xpos=lcddev.width-1-Xpos;
		LCD_WriteReg(lcddev.setxcmd, Xpos);
		LCD_WriteReg(lcddev.setycmd, Ypos);
	}	 
} 		 
//********************************************************************//
//! 函数名:LCD_Scan_Dir
//! 功能:设置LCD扫描方向
//! 输入:dir 0~7 代表8个方向 lcd.h 中有定义
//! 输出:none
//********************************************************************//   	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  
	//! 横屏时,6804和1963不改变方向;竖屏时:1963 调转方向
	if((lcddev.dir==1&&lcddev.id!=0X6804&&lcddev.id!=0X1963)||(lcddev.dir==0&&lcddev.id==0X1963))
	{	
		//! 调转方向		   
		switch(dir)
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	} 
	//! 9341,6804,5310,5510,1963 特殊处理
	if(lcddev.id==0x9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X5510||lcddev.id==0X1963)
	{
		switch(dir)
		{
			//! 从左到右,从上到下
			case L2R_U2D:
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			//! 从左到右,从下到上
			case L2R_D2U:
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			//! 从右到左,从上到下
			case R2L_U2D:
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			//! 从右到左,从下到上
			case R2L_D2U:
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;
			//! 从上到下,从左到右	 
			case U2D_L2R:
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			//! 从上到下,从右到左
			case U2D_R2L:
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			//! 从下到上,从左到右
			case D2U_L2R:
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			//! 从下到上,从右到左
			case D2U_R2L:
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		if(lcddev.id==0X5510)
			dirreg=0X3600;
		else
	  	dirreg=0X36;
		//! 5310,5510,1963不需要BGR
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510)&&(lcddev.id!=0X1963))
			regval|=0X08;
		//! 6804和9341的BIT6相反 
		if(lcddev.id==0X6804)
			regval|=0x02;   
		LCD_WriteReg(dirreg,regval);
		//! 1963不做处理
		if(lcddev.id!=0X1963)
		{
			if(regval&0X20)
			{
				//! 交换x,y
				if(lcddev.width<lcddev.height)
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}
			else  
			{
				//! 交换x,y
				if(lcddev.width>lcddev.height)
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}  
		}
		if(lcddev.id==0X5510)
		{
			LCD_WR_REG(lcddev.setxcmd);
			LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+1);
			LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+2);
			LCD_WR_DATA((lcddev.width-1)>>8); 
			LCD_WR_REG(lcddev.setxcmd+3);
			LCD_WR_DATA((lcddev.width-1)&0XFF); 
			LCD_WR_REG(lcddev.setycmd);
			LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+1);
			LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+2);
			LCD_WR_DATA((lcddev.height-1)>>8); 
			LCD_WR_REG(lcddev.setycmd+3);
			LCD_WR_DATA((lcddev.height-1)&0XFF);
		}
		else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.width-1)>>8);
			LCD_WR_DATA((lcddev.width-1)&0XFF);
			LCD_WR_REG(lcddev.setycmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.height-1)>>8);
			LCD_WR_DATA((lcddev.height-1)&0XFF);  
		}
  }
	else 
	{
		switch(dir)
		{
			//! 从左到右,从上到下
			case L2R_U2D:
				regval|=(1<<5)|(1<<4)|(0<<3); 
				break;
			//! 从左到右,从下到上
			case L2R_D2U:
				regval|=(0<<5)|(1<<4)|(0<<3); 
				break;
			//! 从右到左,从上到下
			case R2L_U2D:
				regval|=(1<<5)|(0<<4)|(0<<3);
				break;
			//! 从右到左,从下到上
			case R2L_D2U:
				regval|=(0<<5)|(0<<4)|(0<<3); 
				break;
			//! 从上到下,从左到右	 
			case U2D_L2R:
				regval|=(1<<5)|(1<<4)|(1<<3); 
				break;
			//! 从上到下,从右到左
			case U2D_R2L:
				regval|=(1<<5)|(0<<4)|(1<<3); 
				break;
			//! 从下到上,从左到右
			case D2U_L2R:
				regval|=(0<<5)|(1<<4)|(1<<3); 
				break;
			//! 从下到上,从右到左
			case D2U_R2L:
				regval|=(0<<5)|(0<<4)|(1<<3); 
				break;	 
		} 
		dirreg=0X03;
		regval|=1<<12; 
		LCD_WriteReg(dirreg,regval);
	}
}     
//********************************************************************//
//! 函数名:LCD_DrawPoint
//! 功能:画点
//! 输入:x 横坐标;y 纵坐标 注:POINT_COLOR 点颜色
//! 输出:none
//********************************************************************//
void LCD_DrawPoint(u16 x,u16 y)
{
	//! 设置光标位置
	LCD_SetCursor(x,y);
	//! 开始写GRAM
	LCD_WriteRAM_Prepare();
	LCD->LCD_RAM=POINT_COLOR; 
}
//********************************************************************//
//! 函数名:LCD_Fast_DrawPoint
//! 功能:快速画点
//! 输入:x 横坐标;y 纵坐标;color 点颜色
//! 输出:none
//********************************************************************//
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);
		LCD_WR_DATA(x&0XFF);  			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);
		LCD_WR_DATA(y&0XFF); 		 	 
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(x>>8);  
		LCD_WR_REG(lcddev.setxcmd+1);
		LCD_WR_DATA(x&0XFF);	  
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(y>>8);  
		LCD_WR_REG(lcddev.setycmd+1);
		LCD_WR_DATA(y&0XFF); 
	}else if(lcddev.id==0X1963)
	{
		if(lcddev.dir==0)x=lcddev.width-1-x;
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);
		LCD_WR_DATA(x&0XFF); 		
		LCD_WR_DATA(x>>8);
		LCD_WR_DATA(x&0XFF); 		
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);
		LCD_WR_DATA(y&0XFF); 		
		LCD_WR_DATA(y>>8);
		LCD_WR_DATA(y&0XFF); 		
	}else if(lcddev.id==0X6804)
	{	
		//! 横屏处理	    
		if(lcddev.dir==1)
			x=lcddev.width-1-x;
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);
		LCD_WR_DATA(x&0XFF);			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);
		LCD_WR_DATA(y&0XFF); 		
	}else
	{
		//! 横屏调转x y坐标
 		if(lcddev.dir==1)
		 	x=lcddev.width-1-x;
		LCD_WriteReg(lcddev.setxcmd,x);
		LCD_WriteReg(lcddev.setycmd,y);
	}			 
	LCD->LCD_REG=lcddev.wramcmd; 
	LCD->LCD_RAM=color; 
}	 
//********************************************************************//
//! 函数名:LCD_SSD_BackLightSet
//! 功能:SSD 1963背光设置
//! 输入:pwm 背光等级;0~100:越大越亮
//! 输出:none
//********************************************************************//
void LCD_SSD_BackLightSet(u8 pwm)
{	
	//! 配置PWM输出
	LCD_WR_REG(0xBE);
	//! 设置PWM频率	
	LCD_WR_DATA(0x05);
	//! 设置PWM频率
	LCD_WR_DATA(pwm*2.55);
	//! 设置C
	LCD_WR_DATA(0x01);
	//! 设置D
	LCD_WR_DATA(0xFF);
	//! 设置E	
	LCD_WR_DATA(0x00);
	//! 设置F	
	LCD_WR_DATA(0x00);	
}
//********************************************************************//
//! 函数名:LCD_Display_Dir
//! 功能:设置LCD显示方向
//! 输入:dir 方向;0:竖屏;1:横屏
//! 输出:none
//********************************************************************//
void LCD_Display_Dir(u8 dir)
{
	//! 竖屏
	if(dir==0)	
	{
		//! 竖屏
		lcddev.dir=0;
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
			if(lcddev.id==0X6804||lcddev.id==0X5310)
			{
				lcddev.width=320;
				lcddev.height=480;
			}
		}
		else 
		if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=480;
			lcddev.height=800;
		}
		else 
		if(lcddev.id==0X1963)
		{
			//! 设置写入GRAM的指令
			lcddev.wramcmd=0X2C;
			//! 设置写X坐标指令	
			lcddev.setxcmd=0X2B;
			//! 设置写Y坐标指令
			lcddev.setycmd=0X2A;
			//!	设置宽度480
			lcddev.width=480;	
			//!	设置高度800
			lcddev.height=800;	 
		}
		else
		{
			lcddev.wramcmd=0X22;
	 		lcddev.setxcmd=0X20;
			lcddev.setycmd=0X21;  
		}
	}
	//! 横屏
	else 			
	{	 
		//! 横屏 				
		lcddev.dir=1;	
		lcddev.width=320;
		lcddev.height=240;
		if(lcddev.id==0X9341||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
		}
		else 
		if(lcddev.id==0X6804)	 
		{
 			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2B;
			lcddev.setycmd=0X2A; 
		}
		else 
		if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=800;
			lcddev.height=480;
		}
		else 
		if(lcddev.id==0X1963)
		{
			//! 设置写入GRAM的指令
			lcddev.wramcmd=0X2C;
			//! 设置写X坐标指令
			lcddev.setxcmd=0X2A;
			//! 设置写Y坐标指令
			lcddev.setycmd=0X2B;
			//! 设置宽度800
			lcddev.width=800;
			//! 设置高度480	
			lcddev.height=480;	
		}
		else
		{
			lcddev.wramcmd=0X22;
	 		lcddev.setxcmd=0X21;
			lcddev.setycmd=0X20;  
		}
		if(lcddev.id==0X6804||lcddev.id==0X5310)
		{ 	 
			lcddev.width=480;
			lcddev.height=320; 			
		}
	} 
	//! 默认扫描方向
	LCD_Scan_Dir(DFT_SCAN_DIR);	
}	 
//********************************************************************//
//! 函数名:LCD_Set_Window
//! 功能:设置窗口,并且自动画点坐标到左上角(sx sy)
//! 输入:(sx,sy)左上角坐标;(ex,ey)竖屏窗口值
//! 输出:none
//********************************************************************//
void LCD_Set_Window(u16 sx,u16 sy,u16 ex,u16 ey)	   
{
	u8 hsareg,heareg,vsareg,veareg;
	u16 hsaval,heaval,vsaval,veaval;  
	//! 横屏
#if USE_HORIZONTAL 	
	//! 窗口值
	hsaval=sy;				
	heaval=ey;
	vsaval=319-ex;
	veaval=319-sx;	
#else	
  //! 竖屏		
	//! 窗口值
	hsaval=sx;				
	heaval=ex;
	vsaval=sy;
	veaval=ey;	
#endif
	if(lcddev.id==0X9341)
	{
		LCD_WR_REG(0x2A);
		LCD_WR_DATA(hsaval>>8); 
		LCD_WR_DATA(hsaval&0XFF);	 
		LCD_WR_DATA(heaval>>8); 
		LCD_WR_DATA(heaval&0XFF);	 
		LCD_WR_REG(0x2B); 
		LCD_WR_DATA(vsaval>>8); 
		LCD_WR_DATA(vsaval&0XFF);	 
		LCD_WR_DATA(veaval>>8); 
		LCD_WR_DATA(veaval&0XFF);	 
	}
	else
	{
		//! 8989
	 	if(lcddev.id==0X8989)
		{
			//! 水平方向窗口寄存器(1289由一个寄存器控制)
			hsareg=0X44;
			heareg=0X44;
			//! 得到寄存器值
			hsaval|=(heaval<<8);
			heaval=hsaval;
			//! 垂直方向窗口寄存器
			vsareg=0X45;
			veareg=0X46;
		}
		//! 其他驱动IC
		else  
		{
			//! 水平方向窗口寄存器
			hsareg=0X50;
			heareg=0X51;
			//! 垂直方向窗口寄存器
			vsareg=0X52;
			veareg=0X53; 
		}								  
		//! 设置寄存器值
		LCD_WriteReg(hsareg,hsaval);
		LCD_WriteReg(heareg,heaval);
		LCD_WriteReg(vsareg,vsaval);
		LCD_WriteReg(veareg,veaval);
	}
}
//********************************************************************//
//! 函数名:LCD_Init
//! 功能:初始化LCD驱动,本函数可初始化各种ILI93xx液晶
//! 输入:none
//! 输出:none
//********************************************************************//
void LCD_Init(void)
{ 
	//! GPIO结构体声明
 	GPIO_InitTypeDef GPIO_InitStructure;
	//! FSMC NORSRAM结构体声明
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	//! FSMC 读写时序结构体,当扩展模式时,仅有读时序有效
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	//! FSMC 写时序结构体,扩展模式使用
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	//! 使能FSMC时钟
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
	//! 使能PART B,D,E,G及AFIO复用功能时钟,
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG,ENABLE);
	//! PB0---------背光功能
	//! PB1---------RESET功能  高电平工作,低电平硬件复位
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	//! 推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	//! GPIO最大输出频率50MHz	
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
 	//! PORTD复用推挽输出  PD0,1,8,9,10,14,15为数据管脚
	//! PD4--------RD; PD5-----------WR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure); 
  	 
	//! PORTE复用推挽输出   PE7,8,9,10,11,12,13,14,15为数据管脚 详细定义见"GPIO管脚及功能定义.txt" 文件
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	
 	//! 复用推挽输出 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);    	    	 											 

  //! PORTG复用推挽输出 	PG12----CS;    PG0-----RS;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_12;	
 	//! 复用推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure); 
	
	//拉高RESET管脚,液晶屏才工作
	GPIO_SetBits(GPIOB,GPIO_Pin_1);                


	//! 读时序地址建立时间(ADDSET)为2个HCLK,一个HCLK:1/36M=27ns
	readWriteTiming.FSMC_AddressSetupTime = 0x01;	 
  //! 地址保持时间(ADDHLD)模式A未使用
	readWriteTiming.FSMC_AddressHoldTime = 0x00;	 	
  //! 数据保存时间为16个HCLK,驱动IC读数据的时候,速度不能太快
	readWriteTiming.FSMC_DataSetupTime = 0x0f;
	//! 总线翻转持续时间
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	//! ͬ同步模式输出时钟信号的周期,此处未使用
  readWriteTiming.FSMC_CLKDivision = 0x00;
	//! 数据延迟周期
  readWriteTiming.FSMC_DataLatency = 0x00;
  //! FSMC模式:A 
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	
    
		
  //! 写数据地址建立时间,(ADDSET)为1个HCLK
	writeTiming.FSMC_AddressSetupTime = 0x00;	
	//! 写数据地址保持时间 1 HCLK
  writeTiming.FSMC_AddressHoldTime = 0x00;	
	//! 数据保存时间,4个HCLK
  writeTiming.FSMC_DataSetupTime = 0x03;		        	
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
	//! 模式A
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	

 
  //! 使用NORSRAM的NE4块,NOR被分为4块,操作第四块
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
  //! 不复用数据地址
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	//! 储存器类型SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;
  //! 储存器数据宽度16bit	
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  //! 禁止连续访问,对应单次访问;连续指一次访问连续地址,也可理解为分组访问
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
  //! 等待信号极性:低
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	//! 禁止异步传输等待
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
  //! 禁止AHB(高性能总线)对未对齐的分组进行两次线性分割访问,在分组访问时才需配置,AHB对应APB(外围总线)	
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  //! 等待信号激活时刻	
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  //! 存储器写使能
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  //! 禁止分组访问时插入等待时间,此处未使用;
	//! 仅在分组模式有效,通过检测NWAIT信号,判断NAND是否繁忙,是就插入等待时间,	
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  //! 读写使用不同时序
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
  //! 禁止连续(分组)写
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  //! 读时序
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
  //! 写时序
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;             
  //! 初始化FSMC配置
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);                                
  //! 使能BANK1 NE5块
 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);                                
		
  //! 延时50 ms 
	delay_ms(50);
	//! 读ID 					
  lcddev.id=LCD_ReadReg(0x0000); 
	//! 读取ID异常,9341未复位会读出9300  
  if(lcddev.id<0XFF||lcddev.id==0XFFFF||lcddev.id==0X9300)
	{	
 		//! 尝试9341 ID读取	
		LCD_WR_REG(0XD3);
		//! 预读取(假装读取)				   
		lcddev.id=LCD_RD_DATA();
		//! 读到0x00
 		lcddev.id=LCD_RD_DATA();
		//! 读到0x93
  	lcddev.id=LCD_RD_DATA(); 							   
 		lcddev.id<<=8;
		 //! 读到0x41
		lcddev.id|=LCD_RD_DATA();	
		//! 非9341,尝试是不是6804   			   
 		if(lcddev.id!=0X9341)
		{	
 			LCD_WR_REG(0XBF);
			//! 预读取(假装读) 				   
			lcddev.id=LCD_RD_DATA();
			//! 读回0x01  	 
	 		lcddev.id=LCD_RD_DATA();
			//! 读回0xD0			   
	 		lcddev.id=LCD_RD_DATA(); 	
			//! 读回0x68		  	
	  	lcddev.id=LCD_RD_DATA();
			lcddev.id<<=8;
			//! 读回0x04
	  	lcddev.id|=LCD_RD_DATA(); 
			//! 不是6804 尝试读取是不是NT35310 
			if(lcddev.id!=0X6804)
			{ 
				LCD_WR_REG(0XD4);	
				//! 预读取(假装读)			   
				lcddev.id=LCD_RD_DATA();
				//! 读回0x01
				lcddev.id=LCD_RD_DATA();
				//! 读回0x53
				lcddev.id=LCD_RD_DATA();
				lcddev.id<<=8;
				//! 读回0x10	 
				lcddev.id|=LCD_RD_DATA();	
				//! 不是NT35310 尝试读取是不是NT35510	 
				if(lcddev.id!=0X5310)	
				{
					LCD_WR_REG(0XDA00);	
					//! 预读取(假装读取)
					lcddev.id=LCD_RD_DATA();	 
					LCD_WR_REG(0XDB00);
					//! 读回0x80	
					lcddev.id=LCD_RD_DATA();
					lcddev.id<<=8;	
					LCD_WR_REG(0XDC00);
					//! 读回0x00	
					lcddev.id|=LCD_RD_DATA();
					//! NT35510读回的ID是8000H,为方便,我们强制设置为5510	
					if(lcddev.id==0x8000)
						lcddev.id=0x5510;
					//! 也不是5510 尝试读取是不是SSD1963
					if(lcddev.id!=0X5510)	
					{
						LCD_WR_REG(0XA1);
						//! 预读取
						lcddev.id=LCD_RD_DATA();
						//! 读回0x57
						lcddev.id=LCD_RD_DATA();
						lcddev.id<<=8;
						//! 读回0x61	 
						lcddev.id|=LCD_RD_DATA();	
						if(lcddev.id==0X5761)
							//! SSD1963读回的ID是0x5761,为方便,强制为0x1963
							lcddev.id=0X1963;
					}
				}
			}
 		}  	
	}
	//! 打印ID,调试的时候可以打开
 	//printf(" LCD ID:%x\r\n",lcddev.id);
	//! 9341初始化
	if(lcddev.id==0X9341)
	{	 
		LCD_WR_REG(0xCF);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0xC1); 
		LCD_WR_DATA(0X30); 
		LCD_WR_REG(0xED);  
		LCD_WR_DATA(0x64); 
		LCD_WR_DATA(0x03); 
		LCD_WR_DATA(0X12); 
		LCD_WR_DATA(0X81); 
		LCD_WR_REG(0xE8);  
		LCD_WR_DATA(0x85); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x7A); 
		LCD_WR_REG(0xCB);  
		LCD_WR_DATA(0x39); 
		LCD_WR_DATA(0x2C); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x34); 
		LCD_WR_DATA(0x02); 
		LCD_WR_REG(0xF7);  
		LCD_WR_DATA(0x20); 
		LCD_WR_REG(0xEA);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00);
		//! Power control 
		LCD_WR_REG(0xC0);
		//! VRH[5:0]      
		LCD_WR_DATA(0x1B);
		//! Power control   
		LCD_WR_REG(0xC1);
		//! SAP[2:0];BT[3:0]     
		LCD_WR_DATA(0x01);
		//! VCM control    
		LCD_WR_REG(0xC5);
		//! 3F     
		LCD_WR_DATA(0x30);
		//! 3C 	 
		LCD_WR_DATA(0x30);
		//! VCM control2  	 
		LCD_WR_REG(0xC7);    
		LCD_WR_DATA(0XB7);
		//! Memory Access Control  
		LCD_WR_REG(0x36);    
		LCD_WR_DATA(0x48); 
		LCD_WR_REG(0x3A);   
		LCD_WR_DATA(0x55); 
		LCD_WR_REG(0xB1);   
		LCD_WR_DATA(0x00);   
		LCD_WR_DATA(0x1A);
		//! Display Function Control 
		LCD_WR_REG(0xB6);     
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0xA2);
		//! 3Gamma Function Disable 
		LCD_WR_REG(0xF2);     
		LCD_WR_DATA(0x00);
		//! Gamma curve selected  
		LCD_WR_REG(0x26);    
		LCD_WR_DATA(0x01);
		//! Set Gamma  
		LCD_WR_REG(0xE0);    
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x2A); 
		LCD_WR_DATA(0x28); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x0E); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x54); 
		LCD_WR_DATA(0XA9); 
		LCD_WR_DATA(0x43); 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00);
		//! Set Gamma  		 
		LCD_WR_REG(0XE1);    
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x15); 
		LCD_WR_DATA(0x17); 
		LCD_WR_DATA(0x07); 
		LCD_WR_DATA(0x11); 
		LCD_WR_DATA(0x06); 
		LCD_WR_DATA(0x2B); 
		LCD_WR_DATA(0x56); 
		LCD_WR_DATA(0x3C); 
		LCD_WR_DATA(0x05); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_REG(0x2B); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x3f);
		LCD_WR_REG(0x2A); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xef);
		//! Exit Sleep	 
		LCD_WR_REG(0x11); 
		delay_ms(120);
		LCD_WR_REG(0x29);
		//! display on 	
	}
	else
	if(lcddev.id==0x6804)
	{
		LCD_WR_REG(0X11);
		delay_ms(20);
		//!VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
		LCD_WR_REG(0XD0);
		LCD_WR_DATA(0X07); 
		LCD_WR_DATA(0X42); 
		LCD_WR_DATA(0X1D);
		//！VCOMH VCOM_AC amplitude setting 
		LCD_WR_REG(0XD1);
		LCD_WR_DATA(0X00);
		LCD_WR_DATA(0X1a);
		LCD_WR_DATA(0X09);
		//! Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting 
		LCD_WR_REG(0XD2);
		LCD_WR_DATA(0X01);
		LCD_WR_DATA(0X22);
		//! REV SM GS 
		LCD_WR_REG(0XC0);
		LCD_WR_DATA(0X10);
		LCD_WR_DATA(0X3B);
		LCD_WR_DATA(0X00);
		LCD_WR_DATA(0X02);
		LCD_WR_DATA(0X11);
		//! Frame rate setting = 72HZ  when setting 0x03
		LCD_WR_REG(0XC5);
		LCD_WR_DATA(0X03);
		//! Gamma setting
		LCD_WR_REG(0XC8);
		LCD_WR_DATA(0X00);
		LCD_WR_DATA(0X25);
		LCD_WR_DATA(0X21);
		LCD_WR_DATA(0X05);
		LCD_WR_DATA(0X00);
		LCD_WR_DATA(0X0a);
		LCD_WR_DATA(0X65);
		LCD_WR_DATA(0X25);
		LCD_WR_DATA(0X77);
		LCD_WR_DATA(0X50);
		LCD_WR_DATA(0X0f);
		LCD_WR_DATA(0X00);	  
						  
   	LCD_WR_REG(0XF8);
		LCD_WR_DATA(0X01);	  

 		LCD_WR_REG(0XFE);
 		LCD_WR_DATA(0X00);
 		LCD_WR_DATA(0X02);
		//! Exit invert mode
		LCD_WR_REG(0X20);

		LCD_WR_REG(0X36);
		//! 原来是0x0a
		LCD_WR_DATA(0X08);
		
		LCD_WR_REG(0X3A);
		//! 16位模式
		LCD_WR_DATA(0X55);  
		LCD_WR_REG(0X2B);
		LCD_WR_DATA(0X00);
		LCD_WR_DATA(0X00);
		LCD_WR_DATA(0X01);
		LCD_WR_DATA(0X3F);
		
		LCD_WR_REG(0X2A);
		LCD_WR_DATA(0X00);
		LCD_WR_DATA(0X00);
		LCD_WR_DATA(0X01);
		LCD_WR_DATA(0XDF);
		delay_ms(120);
		LCD_WR_REG(0X29); 	 
 	}
	 else
	 if(lcddev.id==0x5310)
	{ 
		LCD_WR_REG(0xED);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0xFE);

		LCD_WR_REG(0xEE);
		LCD_WR_DATA(0xDE);
		LCD_WR_DATA(0x21);

		LCD_WR_REG(0xF1);
		LCD_WR_DATA(0x01);
		LCD_WR_REG(0xDF);
		LCD_WR_DATA(0x10);

		//! VCOMvoltage
		LCD_WR_REG(0xC4);
		//! 原来是5f
		LCD_WR_DATA(0x8F);

		LCD_WR_REG(0xC6);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xE2);
		LCD_WR_DATA(0xE2);
		LCD_WR_DATA(0xE2);
		LCD_WR_REG(0xBF);
		LCD_WR_DATA(0xAA);

		LCD_WR_REG(0xB0);
		LCD_WR_DATA(0x0D);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x0D);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x11);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x19);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x21);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x2D);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x3D);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x5D);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x5D);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xB1);
		LCD_WR_DATA(0x80);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x8B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x96);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xB2);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x02);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x03);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xB3);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xB4);
		LCD_WR_DATA(0x8B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x96);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xA1);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xB5);
		LCD_WR_DATA(0x02);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x03);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x04);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xB6);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xB7);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x3F);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x5E);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x64);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x8C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xAC);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xDC);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x70);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x90);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xEB);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xDC);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xB8);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xBA);
		LCD_WR_DATA(0x24);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xC1);
		LCD_WR_DATA(0x20);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x54);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xFF);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xC2);
		LCD_WR_DATA(0x0A);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x04);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xC3);
		LCD_WR_DATA(0x3C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x3A);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x39);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x37);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x3C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x36);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x32);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x2F);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x2C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x29);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x26);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x24);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x24);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x23);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x3C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x36);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x32);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x2F);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x2C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x29);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x26);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x24);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x24);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x23);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xC4);
		LCD_WR_DATA(0x62);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x05);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x84);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xF0);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x18);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xA4);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x18);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x50);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x0C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x17);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x95);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xF3);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xE6);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xC5);
		LCD_WR_DATA(0x32);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x44);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x65);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x76);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x88);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xC6);
		LCD_WR_DATA(0x20);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x17);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xC7);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xC8);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xC9);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xE0);
		LCD_WR_DATA(0x16);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x1C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x21);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x36);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x46);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x52);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x64);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x7A);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x8B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x99);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xA8);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xB9);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xC4);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xCA);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xD2);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xD9);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xE0);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xF3);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xE1);
		LCD_WR_DATA(0x16);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x1C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x22);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x36);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x45);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x52);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x64);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x7A);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x8B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x99);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xA8);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xB9);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xC4);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xCA);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xD2);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xD8);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xE0);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xF3);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xE2);
		LCD_WR_DATA(0x05);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x0B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x1B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x34);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x44);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x4F);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x61);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x79);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x88);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x97);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xA6);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xB7);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xC2);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xC7);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xD1);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xD6);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xDD);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xF3);
		LCD_WR_DATA(0x00);
		LCD_WR_REG(0xE3);
		LCD_WR_DATA(0x05);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xA);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x1C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x33);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x44);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x50);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x62);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x78);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x88);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x97);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xA6);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xB7);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xC2);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xC7);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xD1);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xD5);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xDD);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xF3);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xE4);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x02);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x2A);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x3C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x4B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x5D);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x74);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x84);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x93);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xA2);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xB3);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xBE);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xC4);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xCD);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xD3);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xDD);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xF3);
		LCD_WR_DATA(0x00);
		LCD_WR_REG(0xE5);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x02);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x29);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x3C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x4B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x5D);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x74);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x84);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x93);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xA2);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xB3);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xBE);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xC4);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xCD);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xD3);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xDC);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xF3);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xE6);
		LCD_WR_DATA(0x11);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x34);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x56);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x76);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x77);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x66);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x88);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x99);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xBB);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x99);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x66);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x55);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x55);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x45);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x43);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x44);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xE7);
		LCD_WR_DATA(0x32);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x55);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x76);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x66);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x67);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x67);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x87);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x99);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xBB);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x99);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x77);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x44);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x56);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x23); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x33);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x45);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xE8);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x99);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x87);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x88);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x77);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x66);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x88);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xAA);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xBB);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x99);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x66);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x55);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x55);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x44);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x44);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x55);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xE9);
		LCD_WR_DATA(0xAA);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0x00);
		LCD_WR_DATA(0xAA);

		LCD_WR_REG(0xCF);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xF0);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x50);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xF3);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0xF9);
		LCD_WR_DATA(0x06);
		LCD_WR_DATA(0x10);
		LCD_WR_DATA(0x29);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0x3A);
		//! 66
		LCD_WR_DATA(0x55);

		LCD_WR_REG(0x11);
		delay_ms(100);
		LCD_WR_REG(0x29);
		LCD_WR_REG(0x35);
		LCD_WR_DATA(0x00);

		LCD_WR_REG(0x51);
		LCD_WR_DATA(0xFF);
		LCD_WR_REG(0x53);
		LCD_WR_DATA(0x2C);
		LCD_WR_REG(0x55);
		LCD_WR_DATA(0x82);
		LCD_WR_REG(0x2c);
	}
	else
  if(lcddev.id==0x5510)
	{
		LCD_WriteReg(0xF000,0x55);
		LCD_WriteReg(0xF001,0xAA);
		LCD_WriteReg(0xF002,0x52);
		LCD_WriteReg(0xF003,0x08);
		LCD_WriteReg(0xF004,0x01);
		//! AVDD Set AVDD 5.2V
		LCD_WriteReg(0xB000,0x0D);
		LCD_WriteReg(0xB001,0x0D);
		LCD_WriteReg(0xB002,0x0D);
		//! AVDD ratio
		LCD_WriteReg(0xB600,0x34);
		LCD_WriteReg(0xB601,0x34);
		LCD_WriteReg(0xB602,0x34);
		//! AVEE -5.2V
		LCD_WriteReg(0xB100,0x0D);
		LCD_WriteReg(0xB101,0x0D);
		LCD_WriteReg(0xB102,0x0D);
		//! AVEE ratio
		LCD_WriteReg(0xB700,0x34);
		LCD_WriteReg(0xB701,0x34);
		LCD_WriteReg(0xB702,0x34);
		//! VCL -2.5V
		LCD_WriteReg(0xB200,0x00);
		LCD_WriteReg(0xB201,0x00);
		LCD_WriteReg(0xB202,0x00);
		//! VCL ratio
		LCD_WriteReg(0xB800,0x24);
		LCD_WriteReg(0xB801,0x24);
		LCD_WriteReg(0xB802,0x24);
		//! VGH 15V (Free pump)
		LCD_WriteReg(0xBF00,0x01);
		LCD_WriteReg(0xB300,0x0F);
		LCD_WriteReg(0xB301,0x0F);
		LCD_WriteReg(0xB302,0x0F);
		//! VGH ratio
		LCD_WriteReg(0xB900,0x34);
		LCD_WriteReg(0xB901,0x34);
		LCD_WriteReg(0xB902,0x34);
		//! VGL_REG -10V
		LCD_WriteReg(0xB500,0x08);
		LCD_WriteReg(0xB501,0x08);
		LCD_WriteReg(0xB502,0x08);
		LCD_WriteReg(0xC200,0x03);
		//! VGLX ratio
		LCD_WriteReg(0xBA00,0x24);
		LCD_WriteReg(0xBA01,0x24);
		LCD_WriteReg(0xBA02,0x24);
		//! VGMP/VGSP 4.5V/0V
		LCD_WriteReg(0xBC00,0x00);
		LCD_WriteReg(0xBC01,0x78);
		LCD_WriteReg(0xBC02,0x00);
		//! VGMN/VGSN -4.5V/0V
		LCD_WriteReg(0xBD00,0x00);
		LCD_WriteReg(0xBD01,0x78);
		LCD_WriteReg(0xBD02,0x00);
		//! VCOM
		LCD_WriteReg(0xBE00,0x00);
		LCD_WriteReg(0xBE01,0x64);
		//! Gamma Setting
		LCD_WriteReg(0xD100,0x00);
		LCD_WriteReg(0xD101,0x33);
		LCD_WriteReg(0xD102,0x00);
		LCD_WriteReg(0xD103,0x34);
		LCD_WriteReg(0xD104,0x00);
		LCD_WriteReg(0xD105,0x3A);
		LCD_WriteReg(0xD106,0x00);
		LCD_WriteReg(0xD107,0x4A);
		LCD_WriteReg(0xD108,0x00);
		LCD_WriteReg(0xD109,0x5C);
		LCD_WriteReg(0xD10A,0x00);
		LCD_WriteReg(0xD10B,0x81);
		LCD_WriteReg(0xD10C,0x00);
		LCD_WriteReg(0xD10D,0xA6);
		LCD_WriteReg(0xD10E,0x00);
		LCD_WriteReg(0xD10F,0xE5);
		LCD_WriteReg(0xD110,0x01);
		LCD_WriteReg(0xD111,0x13);
		LCD_WriteReg(0xD112,0x01);
		LCD_WriteReg(0xD113,0x54);
		LCD_WriteReg(0xD114,0x01);
		LCD_WriteReg(0xD115,0x82);
		LCD_WriteReg(0xD116,0x01);
		LCD_WriteReg(0xD117,0xCA);
		LCD_WriteReg(0xD118,0x02);
		LCD_WriteReg(0xD119,0x00);
		LCD_WriteReg(0xD11A,0x02);
		LCD_WriteReg(0xD11B,0x01);
		LCD_WriteReg(0xD11C,0x02);
		LCD_WriteReg(0xD11D,0x34);
		LCD_WriteReg(0xD11E,0x02);
		LCD_WriteReg(0xD11F,0x67);
		LCD_WriteReg(0xD120,0x02);
		LCD_WriteReg(0xD121,0x84);
		LCD_WriteReg(0xD122,0x02);
		LCD_WriteReg(0xD123,0xA4);
		LCD_WriteReg(0xD124,0x02);
		LCD_WriteReg(0xD125,0xB7);
		LCD_WriteReg(0xD126,0x02);
		LCD_WriteReg(0xD127,0xCF);
		LCD_WriteReg(0xD128,0x02);
		LCD_WriteReg(0xD129,0xDE);
		LCD_WriteReg(0xD12A,0x02);
		LCD_WriteReg(0xD12B,0xF2);
		LCD_WriteReg(0xD12C,0x02);
		LCD_WriteReg(0xD12D,0xFE);
		LCD_WriteReg(0xD12E,0x03);
		LCD_WriteReg(0xD12F,0x10);
		LCD_WriteReg(0xD130,0x03);
		LCD_WriteReg(0xD131,0x33);
		LCD_WriteReg(0xD132,0x03);
		LCD_WriteReg(0xD133,0x6D);
		LCD_WriteReg(0xD200,0x00);
		LCD_WriteReg(0xD201,0x33);
		LCD_WriteReg(0xD202,0x00);
		LCD_WriteReg(0xD203,0x34);
		LCD_WriteReg(0xD204,0x00);
		LCD_WriteReg(0xD205,0x3A);
		LCD_WriteReg(0xD206,0x00);
		LCD_WriteReg(0xD207,0x4A);
		LCD_WriteReg(0xD208,0x00);
		LCD_WriteReg(0xD209,0x5C);
		LCD_WriteReg(0xD20A,0x00);

		LCD_WriteReg(0xD20B,0x81);
		LCD_WriteReg(0xD20C,0x00);
		LCD_WriteReg(0xD20D,0xA6);
		LCD_WriteReg(0xD20E,0x00);
		LCD_WriteReg(0xD20F,0xE5);
		LCD_WriteReg(0xD210,0x01);
		LCD_WriteReg(0xD211,0x13);
		LCD_WriteReg(0xD212,0x01);
		LCD_WriteReg(0xD213,0x54);
		LCD_WriteReg(0xD214,0x01);
		LCD_WriteReg(0xD215,0x82);
		LCD_WriteReg(0xD216,0x01);
		LCD_WriteReg(0xD217,0xCA);
		LCD_WriteReg(0xD218,0x02);
		LCD_WriteReg(0xD219,0x00);
		LCD_WriteReg(0xD21A,0x02);
		LCD_WriteReg(0xD21B,0x01);
		LCD_WriteReg(0xD21C,0x02);
		LCD_WriteReg(0xD21D,0x34);
		LCD_WriteReg(0xD21E,0x02);
		LCD_WriteReg(0xD21F,0x67);
		LCD_WriteReg(0xD220,0x02);
		LCD_WriteReg(0xD221,0x84);
		LCD_WriteReg(0xD222,0x02);
		LCD_WriteReg(0xD223,0xA4);
		LCD_WriteReg(0xD224,0x02);
		LCD_WriteReg(0xD225,0xB7);
		LCD_WriteReg(0xD226,0x02);
		LCD_WriteReg(0xD227,0xCF);
		LCD_WriteReg(0xD228,0x02);
		LCD_WriteReg(0xD229,0xDE);
		LCD_WriteReg(0xD22A,0x02);
		LCD_WriteReg(0xD22B,0xF2);
		LCD_WriteReg(0xD22C,0x02);
		LCD_WriteReg(0xD22D,0xFE);
		LCD_WriteReg(0xD22E,0x03);
		LCD_WriteReg(0xD22F,0x10);
		LCD_WriteReg(0xD230,0x03);
		LCD_WriteReg(0xD231,0x33);
		LCD_WriteReg(0xD232,0x03);
		LCD_WriteReg(0xD233,0x6D);
		LCD_WriteReg(0xD300,0x00);
		LCD_WriteReg(0xD301,0x33);
		LCD_WriteReg(0xD302,0x00);
		LCD_WriteReg(0xD303,0x34);
		LCD_WriteReg(0xD304,0x00);
		LCD_WriteReg(0xD305,0x3A);
		LCD_WriteReg(0xD306,0x00);
		LCD_WriteReg(0xD307,0x4A);
		LCD_WriteReg(0xD308,0x00);
		LCD_WriteReg(0xD309,0x5C);
		LCD_WriteReg(0xD30A,0x00);

		LCD_WriteReg(0xD30B,0x81);
		LCD_WriteReg(0xD30C,0x00);
		LCD_WriteReg(0xD30D,0xA6);
		LCD_WriteReg(0xD30E,0x00);
		LCD_WriteReg(0xD30F,0xE5);
		LCD_WriteReg(0xD310,0x01);
		LCD_WriteReg(0xD311,0x13);
		LCD_WriteReg(0xD312,0x01);
		LCD_WriteReg(0xD313,0x54);
		LCD_WriteReg(0xD314,0x01);
		LCD_WriteReg(0xD315,0x82);
		LCD_WriteReg(0xD316,0x01);
		LCD_WriteReg(0xD317,0xCA);
		LCD_WriteReg(0xD318,0x02);
		LCD_WriteReg(0xD319,0x00);
		LCD_WriteReg(0xD31A,0x02);
		LCD_WriteReg(0xD31B,0x01);
		LCD_WriteReg(0xD31C,0x02);
		LCD_WriteReg(0xD31D,0x34);
		LCD_WriteReg(0xD31E,0x02);
		LCD_WriteReg(0xD31F,0x67);
		LCD_WriteReg(0xD320,0x02);
		LCD_WriteReg(0xD321,0x84);
		LCD_WriteReg(0xD322,0x02);
		LCD_WriteReg(0xD323,0xA4);
		LCD_WriteReg(0xD324,0x02);
		LCD_WriteReg(0xD325,0xB7);
		LCD_WriteReg(0xD326,0x02);
		LCD_WriteReg(0xD327,0xCF);
		LCD_WriteReg(0xD328,0x02);
		LCD_WriteReg(0xD329,0xDE);
		LCD_WriteReg(0xD32A,0x02);
		LCD_WriteReg(0xD32B,0xF2);
		LCD_WriteReg(0xD32C,0x02);
		LCD_WriteReg(0xD32D,0xFE);
		LCD_WriteReg(0xD32E,0x03);
		LCD_WriteReg(0xD32F,0x10);
		LCD_WriteReg(0xD330,0x03);
		LCD_WriteReg(0xD331,0x33);
		LCD_WriteReg(0xD332,0x03);
		LCD_WriteReg(0xD333,0x6D);
		LCD_WriteReg(0xD400,0x00);
		LCD_WriteReg(0xD401,0x33);
		LCD_WriteReg(0xD402,0x00);
		LCD_WriteReg(0xD403,0x34);
		LCD_WriteReg(0xD404,0x00);
		LCD_WriteReg(0xD405,0x3A);
		LCD_WriteReg(0xD406,0x00);
		LCD_WriteReg(0xD407,0x4A);
		LCD_WriteReg(0xD408,0x00);
		LCD_WriteReg(0xD409,0x5C);
		LCD_WriteReg(0xD40A,0x00);
		LCD_WriteReg(0xD40B,0x81);

		LCD_WriteReg(0xD40C,0x00);
		LCD_WriteReg(0xD40D,0xA6);
		LCD_WriteReg(0xD40E,0x00);
		LCD_WriteReg(0xD40F,0xE5);
		LCD_WriteReg(0xD410,0x01);
		LCD_WriteReg(0xD411,0x13);
		LCD_WriteReg(0xD412,0x01);
		LCD_WriteReg(0xD413,0x54);
		LCD_WriteReg(0xD414,0x01);
		LCD_WriteReg(0xD415,0x82);
		LCD_WriteReg(0xD416,0x01);
		LCD_WriteReg(0xD417,0xCA);
		LCD_WriteReg(0xD418,0x02);
		LCD_WriteReg(0xD419,0x00);
		LCD_WriteReg(0xD41A,0x02);
		LCD_WriteReg(0xD41B,0x01);
		LCD_WriteReg(0xD41C,0x02);
		LCD_WriteReg(0xD41D,0x34);
		LCD_WriteReg(0xD41E,0x02);
		LCD_WriteReg(0xD41F,0x67);
		LCD_WriteReg(0xD420,0x02);
		LCD_WriteReg(0xD421,0x84);
		LCD_WriteReg(0xD422,0x02);
		LCD_WriteReg(0xD423,0xA4);
		LCD_WriteReg(0xD424,0x02);
		LCD_WriteReg(0xD425,0xB7);
		LCD_WriteReg(0xD426,0x02);
		LCD_WriteReg(0xD427,0xCF);
		LCD_WriteReg(0xD428,0x02);
		LCD_WriteReg(0xD429,0xDE);
		LCD_WriteReg(0xD42A,0x02);
		LCD_WriteReg(0xD42B,0xF2);
		LCD_WriteReg(0xD42C,0x02);
		LCD_WriteReg(0xD42D,0xFE);
		LCD_WriteReg(0xD42E,0x03);
		LCD_WriteReg(0xD42F,0x10);
		LCD_WriteReg(0xD430,0x03);
		LCD_WriteReg(0xD431,0x33);
		LCD_WriteReg(0xD432,0x03);
		LCD_WriteReg(0xD433,0x6D);
		LCD_WriteReg(0xD500,0x00);
		LCD_WriteReg(0xD501,0x33);
		LCD_WriteReg(0xD502,0x00);
		LCD_WriteReg(0xD503,0x34);
		LCD_WriteReg(0xD504,0x00);
		LCD_WriteReg(0xD505,0x3A);
		LCD_WriteReg(0xD506,0x00);
		LCD_WriteReg(0xD507,0x4A);
		LCD_WriteReg(0xD508,0x00);
		LCD_WriteReg(0xD509,0x5C);
		LCD_WriteReg(0xD50A,0x00);
		LCD_WriteReg(0xD50B,0x81);

		LCD_WriteReg(0xD50C,0x00);
		LCD_WriteReg(0xD50D,0xA6);
		LCD_WriteReg(0xD50E,0x00);
		LCD_WriteReg(0xD50F,0xE5);
		LCD_WriteReg(0xD510,0x01);
		LCD_WriteReg(0xD511,0x13);
		LCD_WriteReg(0xD512,0x01);
		LCD_WriteReg(0xD513,0x54);
		LCD_WriteReg(0xD514,0x01);
		LCD_WriteReg(0xD515,0x82);
		LCD_WriteReg(0xD516,0x01);
		LCD_WriteReg(0xD517,0xCA);
		LCD_WriteReg(0xD518,0x02);
		LCD_WriteReg(0xD519,0x00);
		LCD_WriteReg(0xD51A,0x02);
		LCD_WriteReg(0xD51B,0x01);
		LCD_WriteReg(0xD51C,0x02);
		LCD_WriteReg(0xD51D,0x34);
		LCD_WriteReg(0xD51E,0x02);
		LCD_WriteReg(0xD51F,0x67);
		LCD_WriteReg(0xD520,0x02);
		LCD_WriteReg(0xD521,0x84);
		LCD_WriteReg(0xD522,0x02);
		LCD_WriteReg(0xD523,0xA4);
		LCD_WriteReg(0xD524,0x02);
		LCD_WriteReg(0xD525,0xB7);
		LCD_WriteReg(0xD526,0x02);
		LCD_WriteReg(0xD527,0xCF);
		LCD_WriteReg(0xD528,0x02);
		LCD_WriteReg(0xD529,0xDE);
		LCD_WriteReg(0xD52A,0x02);
		LCD_WriteReg(0xD52B,0xF2);
		LCD_WriteReg(0xD52C,0x02);
		LCD_WriteReg(0xD52D,0xFE);
		LCD_WriteReg(0xD52E,0x03);
		LCD_WriteReg(0xD52F,0x10);
		LCD_WriteReg(0xD530,0x03);
		LCD_WriteReg(0xD531,0x33);
		LCD_WriteReg(0xD532,0x03);
		LCD_WriteReg(0xD533,0x6D);
		LCD_WriteReg(0xD600,0x00);
		LCD_WriteReg(0xD601,0x33);
		LCD_WriteReg(0xD602,0x00);
		LCD_WriteReg(0xD603,0x34);
		LCD_WriteReg(0xD604,0x00);
		LCD_WriteReg(0xD605,0x3A);
		LCD_WriteReg(0xD606,0x00);
		LCD_WriteReg(0xD607,0x4A);
		LCD_WriteReg(0xD608,0x00);
		LCD_WriteReg(0xD609,0x5C);
		LCD_WriteReg(0xD60A,0x00);
		LCD_WriteReg(0xD60B,0x81);

		LCD_WriteReg(0xD60C,0x00);
		LCD_WriteReg(0xD60D,0xA6);
		LCD_WriteReg(0xD60E,0x00);
		LCD_WriteReg(0xD60F,0xE5);
		LCD_WriteReg(0xD610,0x01);
		LCD_WriteReg(0xD611,0x13);
		LCD_WriteReg(0xD612,0x01);
		LCD_WriteReg(0xD613,0x54);
		LCD_WriteReg(0xD614,0x01);
		LCD_WriteReg(0xD615,0x82);
		LCD_WriteReg(0xD616,0x01);
		LCD_WriteReg(0xD617,0xCA);
		LCD_WriteReg(0xD618,0x02);
		LCD_WriteReg(0xD619,0x00);
		LCD_WriteReg(0xD61A,0x02);
		LCD_WriteReg(0xD61B,0x01);
		LCD_WriteReg(0xD61C,0x02);
		LCD_WriteReg(0xD61D,0x34);
		LCD_WriteReg(0xD61E,0x02);
		LCD_WriteReg(0xD61F,0x67);
		LCD_WriteReg(0xD620,0x02);
		LCD_WriteReg(0xD621,0x84);
		LCD_WriteReg(0xD622,0x02);
		LCD_WriteReg(0xD623,0xA4);
		LCD_WriteReg(0xD624,0x02);
		LCD_WriteReg(0xD625,0xB7);
		LCD_WriteReg(0xD626,0x02);
		LCD_WriteReg(0xD627,0xCF);
		LCD_WriteReg(0xD628,0x02);
		LCD_WriteReg(0xD629,0xDE);
		LCD_WriteReg(0xD62A,0x02);
		LCD_WriteReg(0xD62B,0xF2);
		LCD_WriteReg(0xD62C,0x02);
		LCD_WriteReg(0xD62D,0xFE);
		LCD_WriteReg(0xD62E,0x03);
		LCD_WriteReg(0xD62F,0x10);
		LCD_WriteReg(0xD630,0x03);
		LCD_WriteReg(0xD631,0x33);
		LCD_WriteReg(0xD632,0x03);
		LCD_WriteReg(0xD633,0x6D);
		//! LV2 Page 0 enable
		LCD_WriteReg(0xF000,0x55);
		LCD_WriteReg(0xF001,0xAA);
		LCD_WriteReg(0xF002,0x52);
		LCD_WriteReg(0xF003,0x08);
		LCD_WriteReg(0xF004,0x00);
		//! Display control
		LCD_WriteReg(0xB100, 0xCC);
		LCD_WriteReg(0xB101, 0x00);
		//Source hold time
		LCD_WriteReg(0xB600,0x05);
		//! Gate EQ control
		LCD_WriteReg(0xB700,0x70);
		LCD_WriteReg(0xB701,0x70);
		//! Source EQ control (Mode 2)
		LCD_WriteReg(0xB800,0x01);
		LCD_WriteReg(0xB801,0x03);
		LCD_WriteReg(0xB802,0x03);
		LCD_WriteReg(0xB803,0x03);
		//! Inversion mode (2-dot)
		LCD_WriteReg(0xBC00,0x02);
		LCD_WriteReg(0xBC01,0x00);
		LCD_WriteReg(0xBC02,0x00);
		//! Timing control 4H w/ 4-delay
		LCD_WriteReg(0xC900,0xD0);
		LCD_WriteReg(0xC901,0x02);
		LCD_WriteReg(0xC902,0x50);
		LCD_WriteReg(0xC903,0x50);
		LCD_WriteReg(0xC904,0x50);
		LCD_WriteReg(0x3500,0x00);
		//! 16-bit/pixel
		LCD_WriteReg(0x3A00,0x55);  
		LCD_WR_REG(0x1100);
		delay_us(120);
		LCD_WR_REG(0x2900);
		//! 9325
	}
	else
	if(lcddev.id==0x9325)
	{
		LCD_WriteReg(0x00E5,0x78F0); 
		LCD_WriteReg(0x0001,0x0100); 
		LCD_WriteReg(0x0002,0x0700); 
		LCD_WriteReg(0x0003,0x1030); 
		LCD_WriteReg(0x0004,0x0000); 
		LCD_WriteReg(0x0008,0x0202);  
		LCD_WriteReg(0x0009,0x0000);
		LCD_WriteReg(0x000A,0x0000); 
		LCD_WriteReg(0x000C,0x0000); 
		LCD_WriteReg(0x000D,0x0000);
		LCD_WriteReg(0x000F,0x0000);
		//! power on sequence VGHVGL
		LCD_WriteReg(0x0010,0x0000);   
		LCD_WriteReg(0x0011,0x0007);  
		LCD_WriteReg(0x0012,0x0000);  
		LCD_WriteReg(0x0013,0x0000); 
		LCD_WriteReg(0x0007,0x0000); 
		//! vgh 
		LCD_WriteReg(0x0010,0x1690);   
		LCD_WriteReg(0x0011,0x0227);
		//! delayms(100);
		//! vregiout 
		LCD_WriteReg(0x0012,0x009D); //0x001b
		//! delayms(100); 
		//! vom amplitude
		LCD_WriteReg(0x0013,0x1900);
		//！delayms(100); 
		//！vom H
		LCD_WriteReg(0x0029,0x0025); 
		LCD_WriteReg(0x002B,0x000D); 
		//! gamma
		LCD_WriteReg(0x0030,0x0007);
		LCD_WriteReg(0x0031,0x0303);
		//! 0006
		LCD_WriteReg(0x0032,0x0003);
		LCD_WriteReg(0x0035,0x0206);
		LCD_WriteReg(0x0036,0x0008);
		LCD_WriteReg(0x0037,0x0406);
		//! 0200 
		LCD_WriteReg(0x0038,0x0304);
		LCD_WriteReg(0x0039,0x0007);
		//! 0504 
		LCD_WriteReg(0x003C,0x0602);
		LCD_WriteReg(0x003D,0x0008); 
		//! ram
		LCD_WriteReg(0x0050,0x0000); 
		LCD_WriteReg(0x0051,0x00EF);
		LCD_WriteReg(0x0052,0x0000); 
		LCD_WriteReg(0x0053,0x013F);  
		LCD_WriteReg(0x0060,0xA700); 
		LCD_WriteReg(0x0061,0x0001); 
		LCD_WriteReg(0x006A,0x0000); 
		//
		LCD_WriteReg(0x0080,0x0000); 
		LCD_WriteReg(0x0081,0x0000); 
		LCD_WriteReg(0x0082,0x0000); 
		LCD_WriteReg(0x0083,0x0000); 
		LCD_WriteReg(0x0084,0x0000); 
		LCD_WriteReg(0x0085,0x0000); 
		//
		LCD_WriteReg(0x0090,0x0010); 
		LCD_WriteReg(0x0092,0x0600); 
		
		LCD_WriteReg(0x0007,0x0133);
		LCD_WriteReg(0x00,0x0022);
	}
	else
	//! ILI9328   OK
  if(lcddev.id==0x9328)  
	{
		//! internal timeing  
  	LCD_WriteReg(0x00EC,0x108F);
		//! ADD      
 		LCD_WriteReg(0x00EF,0x1234);      
		//LCD_WriteReg(0x00e7,0x0010);
		//! 开启内部时钟      
    //LCD_WriteReg(0x0000,0x0001);
    LCD_WriteReg(0x0001,0x0100);
		//! 电源开启     
    LCD_WriteReg(0x0002,0x0700);
		//! 65K  RGB                   
		//LCD_WriteReg(0x0003,(1<<3)|(1<<4) ); 	
		//DRIVE TABLE(寄存器 03H)
		//BIT3=AM BIT4:5=ID0:1
		//AM ID0 ID1   FUNCATION
		// 0  0   0	   R->L D->U
		// 1  0   0	   D->U	R->L
		// 0  1   0	   L->R D->U
		// 1  1   0    D->U	L->R
		// 0  0   1	   R->L U->D
		// 1  0   1    U->D	R->L
		// 0  1   1    L->R U->D 正常使用这个
		// 1  1   1	   U->D	L->R

		//65K 
		LCD_WriteReg(0x0003,(1<<12)|(3<<4)|(0<<3) );   
		LCD_WriteReg(0x0004,0x0000);                                   
		LCD_WriteReg(0x0008,0x0202);	           
		LCD_WriteReg(0x0009,0x0000);         
		LCD_WriteReg(0x000a,0x0000);//display setting         
		LCD_WriteReg(0x000c,0x0001);//display setting          
		LCD_WriteReg(0x000d,0x0000);//0f3c          
		LCD_WriteReg(0x000f,0x0000);
		//! 电源配置
		LCD_WriteReg(0x0010,0x0000);   
		LCD_WriteReg(0x0011,0x0007);
		LCD_WriteReg(0x0012,0x0000);                                                                 
		LCD_WriteReg(0x0013,0x0000);                 
		LCD_WriteReg(0x0007,0x0001);                 
		delay_ms(50); 
		LCD_WriteReg(0x0010,0x1490);   
		LCD_WriteReg(0x0011,0x0227);
		delay_ms(50); 
		LCD_WriteReg(0x0012,0x008A);                  
		delay_ms(50); 
		LCD_WriteReg(0x0013,0x1a00);   
		LCD_WriteReg(0x0029,0x0006);
		LCD_WriteReg(0x002b,0x000d);
		delay_ms(50); 
		LCD_WriteReg(0x0020,0x0000);                                                            
		LCD_WriteReg(0x0021,0x0000);           
		delay_ms(50); 
		//! 伽马校正
		LCD_WriteReg(0x0030,0x0000); 
		LCD_WriteReg(0x0031,0x0604);   
		LCD_WriteReg(0x0032,0x0305);
		LCD_WriteReg(0x0035,0x0000);
		LCD_WriteReg(0x0036,0x0C09); 
		LCD_WriteReg(0x0037,0x0204);
		LCD_WriteReg(0x0038,0x0301);        
		LCD_WriteReg(0x0039,0x0707);     
		LCD_WriteReg(0x003c,0x0000);
		LCD_WriteReg(0x003d,0x0a0a);
		delay_ms(50); 
		//! 水平GRAM起始位置
		LCD_WriteReg(0x0050,0x0000);
		//! 水平GRAM终止位置  
		LCD_WriteReg(0x0051,0x00ef);
		//! 垂直GRAM起始位置                    
		LCD_WriteReg(0x0052,0x0000);
		//! 垂直GRAM终止位置                   
		LCD_WriteReg(0x0053,0x013f);   
 
		LCD_WriteReg(0x0060,0xa700);        
		LCD_WriteReg(0x0061,0x0001); 
		LCD_WriteReg(0x006a,0x0000);
		LCD_WriteReg(0x0080,0x0000);
		LCD_WriteReg(0x0081,0x0000);
		LCD_WriteReg(0x0082,0x0000);
		LCD_WriteReg(0x0083,0x0000);
		LCD_WriteReg(0x0084,0x0000);
		LCD_WriteReg(0x0085,0x0000);
	
		LCD_WriteReg(0x0090,0x0010);     
		LCD_WriteReg(0x0092,0x0600);  
		//! 开启显示设置  
		LCD_WriteReg(0x0007,0x0133); 
	}
	else
	//! 9320测试ok
	if(lcddev.id==0x9320)
	{
		LCD_WriteReg(0x00,0x0000);
		//! Driver Output Contral.
		LCD_WriteReg(0x01,0x0100);
		//! LCD Driver Waveform Contral.	
		LCD_WriteReg(0x02,0x0700);
		//! Entry Mode Set.	
		LCD_WriteReg(0x03,0x1030);
		//! Entry Mode Set.
		//LCD_WriteReg(0x03,0x1018);	
	  //! Scalling Contral.
		LCD_WriteReg(0x04,0x0000);
		//! Display Contral 2.(0x0207)	
		LCD_WriteReg(0x08,0x0202);
		//! Display Contral 3.(0x0000)	
		LCD_WriteReg(0x09,0x0000);
		//! Frame Cycle Contal.(0x0000)	
		LCD_WriteReg(0x0a,0x0000);
		//! Extern Display Interface Contral 1.(0x0000)	
		LCD_WriteReg(0x0c,(1<<0));
		//Frame Maker Position.	
		LCD_WriteReg(0x0d,0x0000);
		//! Extern Display Interface Contral 2.	 	
		LCD_WriteReg(0x0f,0x0000);	   
		delay_ms(50); 
		//! Display Contral.
		LCD_WriteReg(0x07,0x0101);	
		delay_ms(50);
		//! Power Control 1.(0x16b0) 								  
		LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));
		//! Power Control 2.(0x0001)	
		LCD_WriteReg(0x11,0x0007);
		//! Power Control 3.(0x0138)								
		LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));
		//! Power Control 4.				
		LCD_WriteReg(0x13,0x0b00);
		//! Power Control 7.								
		LCD_WriteReg(0x29,0x0000);								
	
		LCD_WriteReg(0x2b,(1<<14)|(1<<4));
		//! Set X Star	    
		LCD_WriteReg(0x50,0);	
		//! 水平GRAM终止位置Set X End.
		//! Set Y Star
		LCD_WriteReg(0x51,239);
		//! Set Y End.t.	
		LCD_WriteReg(0x52,0);	
		LCD_WriteReg(0x53,319);	
	  //! Driver Output Control.
		LCD_WriteReg(0x60,0x2700);
		//! Driver Output Control.	
		LCD_WriteReg(0x61,0x0001);
		//! Vertical Srcoll Control.	
		LCD_WriteReg(0x6a,0x0000);	
	  //! Display Position? Partial Display 1.
		LCD_WriteReg(0x80,0x0000);
		//! RAM Address Start? Partial Display 1.	
		LCD_WriteReg(0x81,0x0000);
		//! RAM Address End-Partial Display 1.	
		LCD_WriteReg(0x82,0x0000);
		//! Displsy Position? Partial Display 2.	
		LCD_WriteReg(0x83,0x0000);
		//! RAM Address Start? Partial Display 2.	
		LCD_WriteReg(0x84,0x0000);
		//! RAM Address End? Partial Display 2.	
		LCD_WriteReg(0x85,0x0000);	
	  //! Frame Cycle Contral.(0x0013)
		LCD_WriteReg(0x90,(0<<7)|(16<<0));
		//! Panel Interface Contral 2.(0x0000)	
		LCD_WriteReg(0x92,0x0000);
		//! Panel Interface Contral 3.	
		LCD_WriteReg(0x93,0x0001);
		//! Frame Cycle Contral.(0x0110)	
		LCD_WriteReg(0x95,0x0110);	
		LCD_WriteReg(0x97,(0<<8));
		//! Frame Cycle Contral.	
		LCD_WriteReg(0x98,0x0000);
		//! (0x0173)		   
		LCD_WriteReg(0x07,0x0173);	
	}
	else
	//! 9331 OK	
	if(lcddev.id==0X9331)	 
	{
		LCD_WriteReg(0x00E7, 0x1014);
		//! set SS and SM bit
		LCD_WriteReg(0x0001, 0x0100);
		//! set 1 line inversion 
		LCD_WriteReg(0x0002, 0x0200);
		//! 65K  
    LCD_WriteReg(0x0003,(1<<12)|(3<<4)|(1<<3));
		//! set GRAM write direction and BGR=1.   
		//LCD_WriteReg(0x0003, 0x1030);
		//! set the back porch and front porch 
		LCD_WriteReg(0x0008, 0x0202);
		//! set non-display area refresh cycle ISC[3:0] 
		LCD_WriteReg(0x0009, 0x0000);
		//! FMARK function 
		LCD_WriteReg(0x000A, 0x0000);
		//! RGB interface setting 
		LCD_WriteReg(0x000C, 0x0000);
		//! Frame marker Position 
		LCD_WriteReg(0x000D, 0x0000);
		//! RGB interface polarity 
		LCD_WriteReg(0x000F, 0x0000); 
		//*************Power On sequence ****************//
		//! SAP, BT[3:0], AP, DSTB, SLP, STB
		LCD_WriteReg(0x0010, 0x0000); 
		//! DC1[2:0], DC0[2:0], VC[2:0]
		LCD_WriteReg(0x0011, 0x0007);
		//! VREG1OUT voltage 
		LCD_WriteReg(0x0012, 0x0000);
		//! VDV[4:0] for VCOM amplitude 
		LCD_WriteReg(0x0013, 0x0000); 
		//! Dis-charge capacitor power voltage
		delay_ms(200);
		//! SAP, BT[3:0], AP, DSTB, SLP, STB 
		LCD_WriteReg(0x0010, 0x1690); 
		//! DC1[2:0], DC0[2:0], VC[2:0]
		LCD_WriteReg(0x0011, 0x0227);
		//! Delay 50ms 
		delay_ms(50); 
		//! Internal reference voltage= Vci;
		LCD_WriteReg(0x0012, 0x000C);
		//! Delay 50ms
		delay_ms(50);
		//! Set VDV[4:0] for VCOM amplitude 
		LCD_WriteReg(0x0013, 0x0800);
		//! Set VCM[5:0] for VCOMH 
		LCD_WriteReg(0x0029, 0x0011);
		//! Set Frame Rate 
		LCD_WriteReg(0x002B, 0x000B); 
		//! Delay 50ms
		delay_ms(50);
		//! GRAM horizontal Address 
		LCD_WriteReg(0x0020, 0x0000);
		//! GRAM Vertical Address 
		LCD_WriteReg(0x0021, 0x013f); 
		// ----------- Adjust the Gamma Curve ----------//
		LCD_WriteReg(0x0030, 0x0000);
		LCD_WriteReg(0x0031, 0x0106);
		LCD_WriteReg(0x0032, 0x0000);
		LCD_WriteReg(0x0035, 0x0204);
		LCD_WriteReg(0x0036, 0x160A);
		LCD_WriteReg(0x0037, 0x0707);
		LCD_WriteReg(0x0038, 0x0106);
		LCD_WriteReg(0x0039, 0x0707);
		LCD_WriteReg(0x003C, 0x0402);
		LCD_WriteReg(0x003D, 0x0C0F);
		//------------------ Set GRAM area ---------------//
		//! Horizontal GRAM Start Address
		LCD_WriteReg(0x0050, 0x0000);
		//! Horizontal GRAM End Address 
		LCD_WriteReg(0x0051, 0x00EF);
		//! Vertical GRAM Start Address 
		LCD_WriteReg(0x0052, 0x0000);
		//! Vertical GRAM Start Address 
		LCD_WriteReg(0x0053, 0x013F);
		//! Gate Scan Line 
		LCD_WriteReg(0x0060, 0x2700);
		//! NDL,VLE, REV 
		LCD_WriteReg(0x0061, 0x0001);
		//! set scrolling line  
		LCD_WriteReg(0x006A, 0x0000); 
		//-------------- Partial Display Control ---------//
		LCD_WriteReg(0x0080, 0x0000);
		LCD_WriteReg(0x0081, 0x0000);
		LCD_WriteReg(0x0082, 0x0000);
		LCD_WriteReg(0x0083, 0x0000);
		LCD_WriteReg(0x0084, 0x0000);
		LCD_WriteReg(0x0085, 0x0000);
		//-------------- Panel Control -------------------//
		LCD_WriteReg(0x0090, 0x0010);
		LCD_WriteReg(0x0092, 0x0600);
		//! 262K color and display ON
		LCD_WriteReg(0x0007, 0x0133);
	}
	else
	if(lcddev.id==0x5408)
	{
		LCD_WriteReg(0x01,0x0100);
		//! LCD Driving Waveform Contral								  
		LCD_WriteReg(0x02,0x0700);
		//! Entry Mode设置 	 
		LCD_WriteReg(0x03,0x1030);   
		//ָ! 指针从左至右自上而下的刷新模式
		//! Normal Mode(Window Mode disable)
		//! RGB格式
		//! 16位数据两次传输的8位总线设置
		//!	Scalling Control register 
		LCD_WriteReg(0x04,0x0000);
		//! Display Control 2     
		LCD_WriteReg(0x08,0x0207);
		//！Display Control 3  
		LCD_WriteReg(0x09,0x0000);
		//!Frame Cycle Control 	 
		LCD_WriteReg(0x0A,0x0000);
		//! External Display Interface Control 1  	 
		LCD_WriteReg(0x0C,0x0000);
		//! Frame Maker Position 
		LCD_WriteReg(0x0D,0x0000); 
		//! External Display Interface Control 2		 
		LCD_WriteReg(0x0F,0x0000);  
 		delay_ms(20);
		//! TFT 液晶图像显示方案14
		//! 0x14B0 //Power Control 1
		LCD_WriteReg(0x10,0x16B0);
		//！0x0007 //Power Control 2 
		LCD_WriteReg(0x11,0x0001); 
		//!0x0000 //Power Control 3
		LCD_WriteReg(0x17,0x0001);
		//！0x013B //Power Control 4 
		LCD_WriteReg(0x12,0x0138);
		//!0x0800 //Power Control 5 
		LCD_WriteReg(0x13,0x0800); 
		//! NVM read data 2
		LCD_WriteReg(0x29,0x0009);
		//! NVM read data 3 
		LCD_WriteReg(0x2a,0x0009); 
		LCD_WriteReg(0xa4,0x0000);
		//! 设置操作窗口的x轴开始列	 
		LCD_WriteReg(0x50,0x0000); 
		//! 设置操作窗口的x轴结束列	 
		LCD_WriteReg(0x51,0x00EF);
		//! 设置操作窗口的y轴开始列	 
		LCD_WriteReg(0x52,0x0000);
		//! 设置操作窗口的y轴结束列	 
		LCD_WriteReg(0x53,0x013F); 
		//! Driver Output Control
		LCD_WriteReg(0x60,0x2700); 
		//! 设置屏幕的点数以及扫描的起始行
		//! Driver Output Control
		LCD_WriteReg(0x61,0x0001);
		//! Vertical Scroll Control 
		LCD_WriteReg(0x6A,0x0000);
		//! Display Position -C Partial Display 1 
		LCD_WriteReg(0x80,0x0000);
		//! RAM Address Start -C Partial Display 1 
		LCD_WriteReg(0x81,0x0000);
		//! RAM address End - Partial Display 1 
		LCD_WriteReg(0x82,0x0000);
		//!Display Position -C Partial Display 2 
		LCD_WriteReg(0x83,0x0000);
		//！RAM Address Start -C Partial Display 2 
		LCD_WriteReg(0x84,0x0000);
		//!RAM address End -C Partail Display2 
		LCD_WriteReg(0x85,0x0000);
		//! Frame Cycle Control 
		LCD_WriteReg(0x90,0x0013);
		//! Panel Interface Control 2 
		LCD_WriteReg(0x92,0x0000);
		//! Panel Interface control 3  
		LCD_WriteReg(0x93,0x0003); 
		//! Frame Cycle Control
		LCD_WriteReg(0x95,0x0110);  
		LCD_WriteReg(0x07,0x0173);		 
		delay_ms(50);
	}	
	else
	//! 1505 OK
	if(lcddev.id==0x1505)
	{
		//! second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
		LCD_WriteReg(0x0007,0x0000);
		delay_ms(50);
		//! 0x011A   why need to set several times? 
		LCD_WriteReg(0x0012,0x011C);
		//! NVM	
		LCD_WriteReg(0x00A4,0x0001); 
		LCD_WriteReg(0x0008,0x000F);
		LCD_WriteReg(0x000A,0x0008);
		LCD_WriteReg(0x000D,0x0008);	    
  	//! 伽马校正
		LCD_WriteReg(0x0030,0x0707);
		//! 0x0707
		LCD_WriteReg(0x0031,0x0007); 
		LCD_WriteReg(0x0032,0x0603); 
		LCD_WriteReg(0x0033,0x0700); 
		LCD_WriteReg(0x0034,0x0202);
		//! 0x0606 
		LCD_WriteReg(0x0035,0x0002); 
		LCD_WriteReg(0x0036,0x1F0F);
		//! 0x0f0f  0x0105
		LCD_WriteReg(0x0037,0x0707); 
		LCD_WriteReg(0x0038,0x0000); 
		LCD_WriteReg(0x0039,0x0000); 
		LCD_WriteReg(0x003A,0x0707);
		//! 0x0303 
		LCD_WriteReg(0x003B,0x0000);
		//! 0x0707 
		LCD_WriteReg(0x003C,0x0007);
		//! 0x1313//0x1f08 
		LCD_WriteReg(0x003D,0x0000); 
		delay_ms(50); 
		LCD_WriteReg(0x0007,0x0001);
		//! 开启电源
		LCD_WriteReg(0x0017,0x0001);
		delay_ms(50); 
		//! 电源配置
		LCD_WriteReg(0x0010,0x17A0); 
		//! reference voltage VC[2:0]   Vciout = 1.00*Vcivl
		LCD_WriteReg(0x0011,0x0217);
		//0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out 
		LCD_WriteReg(0x0012,0x011E);
		//! VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
		LCD_WriteReg(0x0013,0x0F00);
		LCD_WriteReg(0x002A,0x0000); 
		//! 0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage 
		LCD_WriteReg(0x0029,0x000A);
		//! 0x013C  power supply on
		LCD_WriteReg(0x0012,0x013E);
		//Coordinates Control//
		LCD_WriteReg(0x0050,0x0000);//0x0e00
		LCD_WriteReg(0x0051,0x00EF); 
		LCD_WriteReg(0x0052,0x0000); 
		LCD_WriteReg(0x0053,0x013F); 
		//! Pannel Image Control
		LCD_WriteReg(0x0060,0x2700); 
		LCD_WriteReg(0x0061,0x0001); 
		LCD_WriteReg(0x006A,0x0000); 
		LCD_WriteReg(0x0080,0x0000); 
 	  //! Partial Image Control
		LCD_WriteReg(0x0081,0x0000); 
		LCD_WriteReg(0x0082,0x0000); 
		LCD_WriteReg(0x0083,0x0000); 
		LCD_WriteReg(0x0084,0x0000); 
		LCD_WriteReg(0x0085,0x0000); 
  	//! Panel Interface Control
		//! 0x0010 frenqucy
		LCD_WriteReg(0x0090,0x0013);
		LCD_WriteReg(0x0092,0x0300); 
		LCD_WriteReg(0x0093,0x0005); 
		LCD_WriteReg(0x0095,0x0000); 
		LCD_WriteReg(0x0097,0x0000); 
		LCD_WriteReg(0x0098,0x0000); 

		LCD_WriteReg(0x0001,0x0100); 
		LCD_WriteReg(0x0002,0x0700);
		//! 扫描方向 上->下->左->右 
		LCD_WriteReg(0x0003,0x1038);
		LCD_WriteReg(0x0004,0x0000); 
		LCD_WriteReg(0x000C,0x0000); 
		LCD_WriteReg(0x000F,0x0000); 
		LCD_WriteReg(0x0020,0x0000); 
		LCD_WriteReg(0x0021,0x0000); 
		LCD_WriteReg(0x0007,0x0021); 
		delay_ms(20);
		LCD_WriteReg(0x0007,0x0061); 
		delay_ms(20);
		LCD_WriteReg(0x0007,0x0173); 
		delay_ms(20);
	}
	else
	if(lcddev.id==0xB505)
	{
		LCD_WriteReg(0x0000,0x0000);
		LCD_WriteReg(0x0000,0x0000);
		LCD_WriteReg(0x0000,0x0000);
		LCD_WriteReg(0x0000,0x0000);
		
		LCD_WriteReg(0x00a4,0x0001);
		delay_ms(20);		  
		LCD_WriteReg(0x0060,0x2700);
		LCD_WriteReg(0x0008,0x0202);
		
		LCD_WriteReg(0x0030,0x0214);
		LCD_WriteReg(0x0031,0x3715);
		LCD_WriteReg(0x0032,0x0604);
		LCD_WriteReg(0x0033,0x0e16);
		LCD_WriteReg(0x0034,0x2211);
		LCD_WriteReg(0x0035,0x1500);
		LCD_WriteReg(0x0036,0x8507);
		LCD_WriteReg(0x0037,0x1407);
		LCD_WriteReg(0x0038,0x1403);
		LCD_WriteReg(0x0039,0x0020);
		
		LCD_WriteReg(0x0090,0x001a);
		LCD_WriteReg(0x0010,0x0000);
		LCD_WriteReg(0x0011,0x0007);
		LCD_WriteReg(0x0012,0x0000);
		LCD_WriteReg(0x0013,0x0000);
		delay_ms(20);
		
		LCD_WriteReg(0x0010,0x0730);
		LCD_WriteReg(0x0011,0x0137);
		delay_ms(20);
		
		LCD_WriteReg(0x0012,0x01b8);
		delay_ms(20);
		
		LCD_WriteReg(0x0013,0x0f00);
		LCD_WriteReg(0x002a,0x0080);
		LCD_WriteReg(0x0029,0x0048);
		delay_ms(20);
		
		LCD_WriteReg(0x0001,0x0100);
		LCD_WriteReg(0x0002,0x0700);
		//! 扫描方向 上->下->左->右 
    LCD_WriteReg(0x0003,0x1038);
		LCD_WriteReg(0x0008,0x0202);
		LCD_WriteReg(0x000a,0x0000);
		LCD_WriteReg(0x000c,0x0000);
		LCD_WriteReg(0x000d,0x0000);
		LCD_WriteReg(0x000e,0x0030);
		LCD_WriteReg(0x0050,0x0000);
		LCD_WriteReg(0x0051,0x00ef);
		LCD_WriteReg(0x0052,0x0000);
		LCD_WriteReg(0x0053,0x013f);
		LCD_WriteReg(0x0060,0x2700);
		LCD_WriteReg(0x0061,0x0001);
		LCD_WriteReg(0x006a,0x0000);
		//LCD_WriteReg(0x0080,0x0000);
		//LCD_WriteReg(0x0081,0x0000);
		LCD_WriteReg(0x0090,0X0011);
		LCD_WriteReg(0x0092,0x0600);
		LCD_WriteReg(0x0093,0x0402);
		LCD_WriteReg(0x0094,0x0002);
		delay_ms(20);
		
		LCD_WriteReg(0x0007,0x0001);
		delay_ms(20);
		LCD_WriteReg(0x0007,0x0061);
		LCD_WriteReg(0x0007,0x0173);
		
		LCD_WriteReg(0x0020,0x0000);
		LCD_WriteReg(0x0021,0x0000);	  
		LCD_WriteReg(0x00,0x22);  
	}
	else
	if(lcddev.id==0xC505)
	{
		LCD_WriteReg(0x0000,0x0000);
		LCD_WriteReg(0x0000,0x0000);
		delay_ms(20);		  
		LCD_WriteReg(0x0000,0x0000);
		LCD_WriteReg(0x0000,0x0000);
		LCD_WriteReg(0x0000,0x0000);
		LCD_WriteReg(0x0000,0x0000);
 		LCD_WriteReg(0x00a4,0x0001);
		delay_ms(20);		  
		LCD_WriteReg(0x0060,0x2700);
		LCD_WriteReg(0x0008,0x0806);
		//! gamma setting
		LCD_WriteReg(0x0030,0x0703);
		LCD_WriteReg(0x0031,0x0001);
		LCD_WriteReg(0x0032,0x0004);
		LCD_WriteReg(0x0033,0x0102);
		LCD_WriteReg(0x0034,0x0300);
		LCD_WriteReg(0x0035,0x0103);
		LCD_WriteReg(0x0036,0x001F);
		LCD_WriteReg(0x0037,0x0703);
		LCD_WriteReg(0x0038,0x0001);
		LCD_WriteReg(0x0039,0x0004);
		
		
		//! 80Hz
		LCD_WriteReg(0x0090, 0x0015);
		//! BT,AP	
		LCD_WriteReg(0x0010, 0X0410);
		//! DC1,DC0,VC	
		LCD_WriteReg(0x0011,0x0247);	
		LCD_WriteReg(0x0012, 0x01BC);
		LCD_WriteReg(0x0013, 0x0e00);
		delay_ms(120);
		LCD_WriteReg(0x0001, 0x0100);
		LCD_WriteReg(0x0002, 0x0200);
		LCD_WriteReg(0x0003, 0x1030);
		
		LCD_WriteReg(0x000A, 0x0008);
		LCD_WriteReg(0x000C, 0x0000);
		
		LCD_WriteReg(0x000E, 0x0020);
		LCD_WriteReg(0x000F, 0x0000);
		//! H Start
		LCD_WriteReg(0x0020, 0x0000);
		//! V Start	
		LCD_WriteReg(0x0021, 0x0000);
		//! vcom2	
		LCD_WriteReg(0x002A,0x003D);	
		delay_ms(20);
		LCD_WriteReg(0x0029, 0x002d);
		LCD_WriteReg(0x0050, 0x0000);
		LCD_WriteReg(0x0051, 0xD0EF);
		LCD_WriteReg(0x0052, 0x0000);
		LCD_WriteReg(0x0053, 0x013F);
		LCD_WriteReg(0x0061, 0x0000);
		LCD_WriteReg(0x006A, 0x0000);
		LCD_WriteReg(0x0092,0x0300); 
 
 		LCD_WriteReg(0x0093, 0x0005);
		LCD_WriteReg(0x0007, 0x0100);
	}
	else
	//! 4531 OK 
	if(lcddev.id==0x4531)
	{
		LCD_WriteReg(0X00,0X0001);   
		delay_ms(10);   
		LCD_WriteReg(0X10,0X1628);
		//! 0x0006     
		LCD_WriteReg(0X12,0X000e);  
		LCD_WriteReg(0X13,0X0A39);   
		delay_ms(10);   
		LCD_WriteReg(0X11,0X0040);   
		LCD_WriteReg(0X15,0X0050);   
		delay_ms(10);
		//! 16    
		LCD_WriteReg(0X12,0X001e);   
		delay_ms(10);   
		LCD_WriteReg(0X10,0X1620);   
		LCD_WriteReg(0X13,0X2A39);   
		delay_ms(10);   
		LCD_WriteReg(0X01,0X0100);   
		LCD_WriteReg(0X02,0X0300);
		//! 改变方向     
		LCD_WriteReg(0X03,0X1038);
		LCD_WriteReg(0X08,0X0202);   
		LCD_WriteReg(0X0A,0X0008);   
		LCD_WriteReg(0X30,0X0000);   
		LCD_WriteReg(0X31,0X0402);   
		LCD_WriteReg(0X32,0X0106);   
		LCD_WriteReg(0X33,0X0503);   
		LCD_WriteReg(0X34,0X0104);   
		LCD_WriteReg(0X35,0X0301);   
		LCD_WriteReg(0X36,0X0707);   
		LCD_WriteReg(0X37,0X0305);   
		LCD_WriteReg(0X38,0X0208);   
		LCD_WriteReg(0X39,0X0F0B);   
		LCD_WriteReg(0X41,0X0002);   
		LCD_WriteReg(0X60,0X2700);   
		LCD_WriteReg(0X61,0X0001);   
		LCD_WriteReg(0X90,0X0210);   
		LCD_WriteReg(0X92,0X010A);   
		LCD_WriteReg(0X93,0X0004);   
		LCD_WriteReg(0XA0,0X0100);   
		LCD_WriteReg(0X07,0X0001);   
		LCD_WriteReg(0X07,0X0021);   
		LCD_WriteReg(0X07,0X0023);   
		LCD_WriteReg(0X07,0X0033);   
		LCD_WriteReg(0X07,0X0133);   
		LCD_WriteReg(0XA0,0X0000); 
	}
	else
	if(lcddev.id==0x4535)
	{			      
		LCD_WriteReg(0X15,0X0030);   
		LCD_WriteReg(0X9A,0X0010);   
 		LCD_WriteReg(0X11,0X0020);   
 		LCD_WriteReg(0X10,0X3428);
		 //! 16    
		LCD_WriteReg(0X12,0X0002);   
 		LCD_WriteReg(0X13,0X1038);   
		delay_ms(40); 
		//! 16  
		LCD_WriteReg(0X12,0X0012);    
		delay_ms(40);   
  		LCD_WriteReg(0X10,0X3420);   
 		LCD_WriteReg(0X13,0X3038);   
		delay_ms(70);   
		LCD_WriteReg(0X30,0X0000);   
		LCD_WriteReg(0X31,0X0402);   
		LCD_WriteReg(0X32,0X0307);   
		LCD_WriteReg(0X33,0X0304);   
		LCD_WriteReg(0X34,0X0004);   
		LCD_WriteReg(0X35,0X0401);   
		LCD_WriteReg(0X36,0X0707);   
		LCD_WriteReg(0X37,0X0305);   
		LCD_WriteReg(0X38,0X0610);   
		LCD_WriteReg(0X39,0X0610); 
		  
		LCD_WriteReg(0X01,0X0100);   
		LCD_WriteReg(0X02,0X0300);
		//! 改变方向   
		LCD_WriteReg(0X03,0X1030);   
		LCD_WriteReg(0X08,0X0808);   
		LCD_WriteReg(0X0A,0X0008);   
 		LCD_WriteReg(0X60,0X2700);   
		LCD_WriteReg(0X61,0X0001);   
		LCD_WriteReg(0X90,0X013E);   
		LCD_WriteReg(0X92,0X0100);   
		LCD_WriteReg(0X93,0X0100);   
 		LCD_WriteReg(0XA0,0X3000);   
 		LCD_WriteReg(0XA3,0X0010);   
		LCD_WriteReg(0X07,0X0001);   
		LCD_WriteReg(0X07,0X0021);   
		LCD_WriteReg(0X07,0X0023);   
		LCD_WriteReg(0X07,0X0033);   
		LCD_WriteReg(0X07,0X0133);   
	}
	else
	if(lcddev.id==0X1963)
	{
		//! Set PLL with OSC = 10MHz (hardware),	Multiplier N = 35, 
		//!! 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz
		LCD_WR_REG(0xE2);	
		//! 参数1 	
		LCD_WR_DATA(0x1D);
		//! 参数2 Divider M = 2, PLL = 300/(M+1) = 100MHz		
		LCD_WR_DATA(0x02);
		//! 参数3 Validate M and N values 		
		LCD_WR_DATA(0x04);		  
		delay_us(100);
		//! Start PLL command
		LCD_WR_REG(0xE0);
		//! enable PLL		
		LCD_WR_DATA(0x01);		
		delay_ms(10);
		//! Start PLL command again
		LCD_WR_REG(0xE0);
		//! now, use PLL output as system clock		
		LCD_WR_DATA(0x03);			
		delay_ms(12);
		//! 软复位  
		LCD_WR_REG(0x01);		
		delay_ms(10);
		//! 设置像素频率,33Mhz
		LCD_WR_REG(0xE6);		
		LCD_WR_DATA(0x2F);
		LCD_WR_DATA(0xFF);
		LCD_WR_DATA(0xFF);
		//! 设置LCD模式
		LCD_WR_REG(0xB0);
		//! 24位模式		
		LCD_WR_DATA(0x20);
		//! TFT模式		
		LCD_WR_DATA(0x00);		
	  //! 设置LCD水平像素
		LCD_WR_DATA((SSD_HOR_RESOLUTION-1)>>8);
		LCD_WR_DATA(SSD_HOR_RESOLUTION-1);
		//! 设置LCD垂直像素		 
		LCD_WR_DATA((SSD_VER_RESOLUTION-1)>>8);
		LCD_WR_DATA(SSD_VER_RESOLUTION-1);
		//! RGB序列		 
		LCD_WR_DATA(0x00);
		//! Set horizontal period
		LCD_WR_REG(0xB4);		
		LCD_WR_DATA((SSD_HT-1)>>8);
		LCD_WR_DATA(SSD_HT-1);
		LCD_WR_DATA(SSD_HPS>>8);
		LCD_WR_DATA(SSD_HPS);
		LCD_WR_DATA(SSD_HOR_PULSE_WIDTH-1);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		//! Set vertical period
		LCD_WR_REG(0xB6);		
		LCD_WR_DATA((SSD_VT-1)>>8);
		LCD_WR_DATA(SSD_VT-1);
		LCD_WR_DATA(SSD_VPS>>8);
		LCD_WR_DATA(SSD_VPS);
		LCD_WR_DATA(SSD_VER_FRONT_PORCH-1);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		//! 设置SSD1963与CPU接口为16bit
		LCD_WR_REG(0xF0);
		//! 16-bit(565 format) data for 16bpp 	  
		LCD_WR_DATA(0x03);	
    //! 开启显示
		LCD_WR_REG(0x29);	
		//! 设置PWM占空比 背光通过占空比可调
		//! 设置自动白平衡DBC
		LCD_WR_REG(0xD0);
		//! disable	
		LCD_WR_DATA(0x00);	
	  //! 配置PWM输出
		LCD_WR_REG(0xBE);
		//! 1设置PWM频率	
		LCD_WR_DATA(0x05);
		//! 2设置PWM占空比	
		LCD_WR_DATA(0xFE);
		//! 3设置C	
		LCD_WR_DATA(0x01);
		//! 4设置D	
		LCD_WR_DATA(0x00);
		//！5设置E	
		LCD_WR_DATA(0x00);
		//! 6设置F	 
		LCD_WR_DATA(0x00);	 
		//! 配置GPIO
		LCD_WR_REG(0xB8);
		//! 2个IO口配置为输出	
		LCD_WR_DATA(0x03);
		//! GPIO使用正常的IO功能	
		LCD_WR_DATA(0x01);	
		LCD_WR_REG(0xBA);
		//! GPIO[1:0]=01,控制LCD方向
		LCD_WR_DATA(0X01);	
		//! 背光设置为最亮
		LCD_SSD_BackLightSet(100);
	}	
	//! 默认竖屏	 
	LCD_Display_Dir(0);
	//! 点亮背光		
	LCD_LED=1;				
	LCD_Clear(WHITE);
}  
//********************************************************************//
//! 函数名:LCD_Clear
//! 功能:清屏函数
//! 输入:color 清屏时填充的函数
//! 输出:none
//********************************************************************//
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	//! 得到总点数
	totalpoint*=lcddev.height;
	//! 6804横屏时特殊处理  			
	if((lcddev.id==0X6804)&&(lcddev.dir==1)) 
	{						    
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;
		//! 设置光标位置  	 			
		LCD_SetCursor(0x00,0x0000);		 
 		lcddev.dir=1;	 
  	lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}
	else
	  //! 设置光标位置 
	  LCD_SetCursor(0x00,0x0000);
	//!	开始写入GRAM
	LCD_WriteRAM_Prepare();     		 	  
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;	
	}
}  
//********************************************************************//
//! 函数名:LCD_Fill
//! 功能:在指定的区域内填充单个颜色
//! 输入:(sx sy) (ex ey)填充矩形的对角坐标,区域大小:(ex-sx+1)*(ey-sy+1)
//! 输出:none
//********************************************************************//
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	u16 temp;
	//! 6804横屏时特殊处理
	if((lcddev.id==0X6804)&&(lcddev.dir==1))	
	{
		temp=sx;
		sx=sy;
		sy=lcddev.width-ex-1;	  
		ex=ey;
		ey=lcddev.width-temp-1;
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 			
		LCD_Fill(sx,sy,ex,ey,color);  
 		lcddev.dir=1;	 
  	lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}else
	{
		xlen=ex-sx+1;	 
		for(i=sy;i<=ey;i++)
		{
			//! 设置光标位置
		 	LCD_SetCursor(sx,i);
			 //! 开始写入GRAM      				 
			LCD_WriteRAM_Prepare();
			//! 显示颜色     		  
			for(j=0;j<xlen;j++)
				LCD->LCD_RAM=color;	    
		}
	}	 
}  
//********************************************************************//
//! 函数名:LCD_Color_Fill
//! 功能:在指定的区域内填充颜色块
//! 输入:(sx sy) (ex ey) 填充矩形的对角坐标,区域大小:(ex-sx+1)*(ey-sy+1)
//! 输出:none
//********************************************************************//
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//�õ����Ŀ���
	height=ey-sy+1;			//�߶�
 	for(i=0;i<height;i++)
	{
		//! 填充宽度
 		LCD_SetCursor(sx,sy+i);
		 //! 开始写入GRAM   
		LCD_WriteRAM_Prepare();
		//! 写入数据    
		for(j=0;j<width;j++)
			LCD->LCD_RAM=color[i*width+j];
	}		  
}  
//********************************************************************//
//! 函数名:LCD_DrawLine
//! 功能:画线
//! 输入:(x1 y1) 起点坐标; (x2 y2) 终点坐标 
//! 输出:none
//********************************************************************// 
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	//! 计算坐标增量 
	delta_x=x2-x1; 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	//! 设置单步方向
	if(delta_x>0)
		incx=1; 
	else
	//! 垂直线
  if(delta_x==0)
		incx=0;
		else
		 {
			 incx=-1;
		 	 delta_x=-delta_x;
		 } 
	if(delta_y>0)
		incy=1; 
	else
	//! 水平线
	if(delta_y==0)
		incy=0;
	else
	{
		incy=-1;
		delta_y=-delta_y;
	} 
	//! 选区基本坐标轴
	if( delta_x>delta_y)
		distance=delta_x; 
	else 
		distance=delta_y; 
	//! 画线输出
	for(t=0;t<=distance+1;t++ )
	{  
		//! 画点
		LCD_DrawPoint(uRow,uCol);
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//********************************************************************//
//! 函数名:LCD_DrawRectangle
//! 功能:画矩形
//! 输入:(x1 y1) (x2 y2) 矩形对角坐标 
//! 输出:none
//********************************************************************// 
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//********************************************************************//
//! 函数名:LCD_Draw_Circle
//! 功能:在指定位置画一个指定大小的圆
//! 输入:(x y) 圆心坐标;r 半径
//! 输出:none
//********************************************************************// 
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;
	b=r;
	//! 判断下个点位置的坐标	  
	di=3-(r<<1);             
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  	LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//! 使用Bresenham算法画圆
		if(di<0)
			di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//********************************************************************//
//! 函数名:LCD_ShowChar
//! 功能:在指定位置显示一个字符
//! 输入:(x y) 起始坐标;num 要显示的字符:" "->"~";size 字体大小:12/16/24;mode:0 叠加方式;1 非叠加方式
//! 输出:none
//********************************************************************// 
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	//! 得到字体一个字符对应点阵集所占的字符数
	u8 csize=(size/8+((size%8)?1:0))*(size/2);
	//! 得到偏移后的值,ASCII字符从" "开始取模,所以 - " "就是对应字符的字库	
 	num=num-' ';
	for(t=0;t<csize;t++)
	{   
		//! 调用1206字体
		if(size==12)
			temp=asc2_1206[num][t]; 	 	
		else
		//! 调用1608字体 
		if(size==16)
			temp=asc2_1608[num][t];	
		else 
		//! 调用2412字体
		if(size==24)
			temp=asc2_2412[num][t];	
		else 
		//! 没有相应字库,返回
			return;								
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)
				LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else 
			if(mode==0)
				LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			//! 超过区域
			if(y>=lcddev.height)
				return;		
			if((y-y0)==size)
			{
				y=y0;
				x++;
				//! 超过区域
				if(x>=lcddev.width)
					return;
				break;
			}
		}  	 
	}  	    	   	 	  
}   
//********************************************************************//
//! 函数名:LCD_Pow
//! 功能:m^n函数
//! 输入:m 底数;n 幂数;
//! 输出:m的n次方
//********************************************************************// 
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)
		result*=m;    
	return result;
}			 
//********************************************************************//
//! 函数名:LCD_ShowNum
//! 功能:显示数字,高位为0则不显示
//! 输入:x,y起点坐标;num 数值(0~4294967295);len 数字的位数;size 字体大小;(颜色可自定义,在函数内部)
//! 输出:none
//********************************************************************//	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//********************************************************************//
//! 函数名:LCD_ShowxNum
//! 功能:显示数字,高位为0还是显示
//! 输入:x,y起点坐标;num 数值(0~999999999);len 要显示的数值长度;size 字体大小;
//! mode 0非叠加显示;1叠加显示
//! 输出:none
//********************************************************************//
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//********************************************************************//
//! 函数名:LCD_ShowString
//! 功能:显示字符串
//! 输入:x,y起点坐标;width,height 区域大小;size 字体大小;*p 字符串指针首地址
//! mode 0非叠加显示;1叠加显示
//! 输出:none
//********************************************************************// 
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
	  //! 判断是不是非法字符
    while((*p<='~')&&(*p>=' '))
    {       
        if(x>=width)
					{x=x0;y+=size;}
				//! 退出
        if(y>=height)
					break;
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}
//********************************************************************//
//! 函数名:GUI_Chinese_Text
//! 功能:在指定坐标显示汉子
//! 输入:x 行坐标,y 列坐标,起点坐标;str 字符串;len 字符长度;charColor 字符颜色;bkColor 背景颜色
//! 输出:none
//! 说明:一个汉字两个字符
//! 调用方法 GUI_Chinese_Text(0,0,"我是汉字",8,0x0000,0xffff)
//********************************************************************// 
void GUI_Chinese_Text(u16 x,u16 y, u8 str[],u8 len,u16 charColor,u16 bkColor)
{
    u16 i=0,b;
    u16 j=0;
	  u16 x_add,y_add;
    u16 tmp_char=0,index=0;
	  x_add=x;
	  y_add=y;
	
	for(b=0;b<len/2;b++)
	{
		  //! 计算区位
			index=(94*(str[b*2] - 0xa1)+(str[b*2+1] - 0xa1));
			for (i=0;i<16;i++)
		    {
			    for (j=0;j<8;j++)
			    {
						//! 查字库
				    tmp_char=HzLib[index*32+i*2];	
			        if ( (tmp_char << j) & 0x80)
			        {
								//! 字符颜色
								LCD_Fast_DrawPoint(x_add+j,y_add+i,charColor);
			        }
			        else
			        {
								//! 背景颜色
								LCD_Fast_DrawPoint(x_add+j,y_add+i,bkColor); 
			        }
			    }
			   	for (j=0;j<8;j++)
			    {
						//! 查字库
				    tmp_char=HzLib[index*32+i*2+1];		
			        if ( (tmp_char << j) & 0x80)
			        {
								//! 字符颜色
								LCD_Fast_DrawPoint(x_add+j+8,y_add+i,charColor); 
			        }
			        else
			        {   
								//! 背景颜色
								LCD_Fast_DrawPoint(x_add+j+8,y_add+i,bkColor); 
			        }
			    }
		     }
			 //! 地址累加	 
			 x_add=x_add+17;									
	}
}



































