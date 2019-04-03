#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"	 
#include "stdlib.h"
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


//!LCD重要参数
typedef struct  
{	
	//! LCD 宽度									    
	u16 width;
	//! LCD 高度			
	u16 height;
	//! LCD ID			
	u16 id;
	//! 屏幕方向;0 竖屏;1 横屏				
	u8  dir;
	//! 开始写GRAM指令				
	u16	wramcmd;
	//! 设置x坐标指令		
	u16  setxcmd;
	//! 设置y坐标指令		
	u16  setycmd;		
}_lcd_dev; 	  

//! 管理LCD的重要参数
extern _lcd_dev lcddev;
//! LCD画笔颜色和背景色  
//! 画笔默认红色
extern u16  POINT_COLOR;
//! 背景默认白色  
extern u16  BACK_COLOR; 


//******************************************************************************//	 
//-----------------LCD端口定义---------------- 
//! LCD背光 PB0
#define	LCD_LED PBout(0)  	    
//LCD地址结构体
typedef struct
{
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;
//! 使用NOR/SRAM的Bank1.sector4,地址HADDR[27,26]=11,A10作为数据命令区分线
//! 注意设置时STM32内部会右移一位对齐 			    
#define LCD_BASE        ((u32)(0x6C000000 | 0x000007FE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
//******************************************************************************//	
	 
//! 扫描方向定义
//! 从左到右,从上到下
#define L2R_U2D  0 
//! 从左到右,从下到上
#define L2R_D2U  1
//! 从右到左,从上到下 
#define R2L_U2D  2 
//! 从右到左,从下到上
#define R2L_D2U  3 

//! 从上到下,从左到右
#define U2D_L2R  4
//! 从上到下,从右到左
#define U2D_R2L  5
//! 从下到上,从左到右
#define D2U_L2R  6 
//! 从下到上,从右到左
#define D2U_R2L  7 	 

//! 默认的扫描方向
#define DFT_SCAN_DIR  L2R_U2D  

//! 画笔颜色
//! 白色
#define WHITE         	 0xFFFF 
//! 黑色 
#define BLACK         	 0x0000	
//! 蓝色 
#define BLUE         	 0x001F  
#define BRED             0XF81F  
#define GRED 			 0XFFE0  
#define GBLUE			 0X07FF
//! 红色  
#define RED           	 0xF800
//! 品红  
#define MAGENTA       	 0xF81F
//! 绿色  
#define GREEN         	 0x07E0 
//! 青色 
#define CYAN          	 0x7FFF
//! 黄色  
#define YELLOW        	 0xFFE0
//! 棕色  
#define BROWN 			 0XBC40 
//! 棕红色 
#define BRRED 			 0XFC07 
//! 灰色 
#define GRAY  			 0X8430  

//! GUI颜色
//! 深蓝色
#define DARKBLUE      	 0X01CF	
//! 浅蓝色
#define LIGHTBLUE      	 0X7D7C
//! 灰蓝色	
#define GRAYBLUE       	 0X5458 
//! 
//! 浅绿色
#define LIGHTGREEN     	 0X841F
//! 浅灰色(PANNEL) 
//#define LIGHTGRAY      0XEF5B
//! 浅灰色(PANNEL),窗体背景色 
#define LGRAY 			 0XC618 
//! 浅灰蓝色(中间层颜色)
#define LGRAYBLUE        0XA651
//! 浅棕蓝色(选择条目的反色) 
#define LBBLUE           0X2B12 
//! 初始化	    															  
void LCD_Init(void);
//! 开显示							
void LCD_DisplayOn(void);
//! 关显示												
void LCD_DisplayOff(void);
//! 清屏												
void LCD_Clear(u16 Color);
//! 设置光标	 											
void LCD_SetCursor(u16 Xpos, u16 Ypos);
//! 画点							
void LCD_DrawPoint(u16 x,u16 y);
//! 快速画点									
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);	
//! 读点					
u16  LCD_ReadPoint(u16 x,u16 y); 
//! 画圆											
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);
//! 画线					
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
//! 画矩形						
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);	
//! 填充单色	   		
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
//! 填充指定颜色		   			
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);
//! 显示一个字符		
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);
//! 显示一个数字						
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
//! 显示数字  						
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);
//! 显示一个字符串			
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
//! 显示汉子		
void GUI_Chinese_Text(u16 x,u16 y, u8 str[],u8 len,u16 charColor,u16 bkColor); 

void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
//! SSD1963背光控制
void LCD_SSD_BackLightSet(u8 pwm);
//! 设置扫描方向							
void LCD_Scan_Dir(u8 dir);
//! 设置显示方向									
void LCD_Display_Dir(u8 dir);
//! 设置窗口								
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);			
void LCD_WR_DATA(u16 data);
//! LCD分辨率设置
//! LCD水平分辨率
#define SSD_HOR_RESOLUTION		800
//! LCD垂直分辨率		
#define SSD_VER_RESOLUTION		480		
//! LCD驱动参数设置
//! 水平脉宽
#define SSD_HOR_PULSE_WIDTH		1
//! 水平前廊		
#define SSD_HOR_BACK_PORCH		46
//! 水平后廊		
#define SSD_HOR_FRONT_PORCH		210		
//! 垂直脉宽
#define SSD_VER_PULSE_WIDTH		1
//! 垂直前廊		
#define SSD_VER_BACK_PORCH		23	
//! 垂直后廊	
#define SSD_VER_FRONT_PORCH		22		
//! 如下参数,自动计算
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_BACK_PORCH)
#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

//! 图像数据头结构 
__packed typedef struct _HEADCOLOR
{
   unsigned char scan;
   unsigned char gray;
   unsigned short w;
   unsigned short h;
   unsigned char is565;
   unsigned char rgb;
}HEADCOLOR; 	

#endif  
	 
	 



