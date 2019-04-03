#include "sys.h"
#include "lcd.h"
#include "image2lcd.h"
extern void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height); 					   	
extern void LCD_WR_REG(u8 data);
//********************************************************************//
//! 函数名:image_getcolor
//! 功能:从8位数据获取16位颜色
//! 输入:mode: 0:低位在前,高位在后; 1:高位在前,低位在后; str:数据指针; 
//! 输出:16位颜色
//********************************************************************//  					    	 
u16 image_getcolor(u8 mode,u8 *str)
{
	u16 color;
	if(mode)
	{
		color=((u16)*str++)<<8;
		color|=*str;
	}else
	{
		color=*str++;
		color|=((u16)*str)<<8;
	}
	return color;	
}
//********************************************************************//
//! 函数名:image_show
//! 功能:在液晶上画图
//! 输入:xsta,ysta,xend,yend:画图区域; scan:扫描方向; p:图像数组指针
//! 输出:none
//********************************************************************// 
void image_show(u16 xsta,u16 ysta,u16 xend,u16 yend,u8 scan,u8 *p)
{  
	u32 i;
	u32 len=0;
	//! 水平扫描
	if((scan&0x03)==0)
	{
		//! 判断扫描方向
		switch(scan>>6)
		{
			case 0:
				//! 从左到右,从上到下
				LCD_Scan_Dir(L2R_U2D);
				LCD_Set_Window(xsta,ysta,xend,yend);
				//! 设置光标位置
				LCD_SetCursor(xsta,ysta);
				break; 
			case 1:
				//! 从左到右,从下到上
				LCD_Scan_Dir(L2R_D2U);
				LCD_Set_Window(xsta,ysta,xend,yend);
				//! 设置光标位置
				LCD_SetCursor(xsta,yend);
				break; 
			case 2:
				//! 从右到左,从上到下
				LCD_Scan_Dir(R2L_U2D);
				LCD_Set_Window(xsta,ysta,xend,yend);
				//! 设置光标位置
				LCD_SetCursor(xend,ysta);
				break; 
			case 3:
				//! 从右到左,从下到上
				LCD_Scan_Dir(R2L_D2U);
				LCD_Set_Window(xsta,ysta,xend,yend);
				//! 设置光标位置
				LCD_SetCursor(xend,yend);
				break; 
		}
	}
	//! 垂直扫描
	else  
	{
		//! 判断扫描方向
		switch(scan>>6)
		{
			case 0:
				//! 从上到下,从左到右
				LCD_Scan_Dir(U2D_L2R);
				LCD_Set_Window(xsta,ysta,xend,yend);
				//! 设置光标位置
				LCD_SetCursor(xsta,ysta);
				break; 
			case 1:
				//! 从下到上,从左到右
				LCD_Scan_Dir(D2U_L2R);
				LCD_Set_Window(xsta,ysta,xend,yend);
				//! 设置光标位置
				LCD_SetCursor(xsta,yend); 
				break; 
			case 2:
				//! 从上到下,从右到左
				LCD_Scan_Dir(U2D_R2L);
				LCD_Set_Window(xsta,ysta,xend,yend);
				//! 设置光标位置
				LCD_SetCursor(xend,ysta);
				break; 
			case 3:
				//! 从下到上,从右到左
				LCD_Scan_Dir(D2U_R2L);
				LCD_Set_Window(xsta,ysta,xend,yend);
				//! 设置光标位置
				LCD_SetCursor(xend,yend);
				break; 
		}
	}
	//! 开始写入GRAM
	LCD_WriteRAM_Prepare();
	//! 写入数据长度     		
	len=(xend-xsta+1)*(yend-ysta+1);	
	for(i=0;i<len;i++)
	{
		LCD_WR_DATA(image_getcolor(scan&(1<<4),p));
		p+=2;
	}
	//! 使用横屏 	 
#if USE_HORIZONTAL  
	LCD_Set_Window(0,0,319,239);
#else
	LCD_Set_Window(0,0,239,319);
#endif	    					  	    
}  
//********************************************************************//
//! 函数名:image_display
//! 功能:在指定位置显示图片
//! 输入:x,y:指定位置; imgx:图片数据指针; 
//! 输出:none
//********************************************************************// 
void image_display(u16 x,u16 y,u8 * imgx)
{
	HEADCOLOR *imginfo;
	//! 得出HEADCOLOR结构体大小
 	u8 ifosize=sizeof(HEADCOLOR);
	imginfo=(HEADCOLOR*)imgx;
 	image_show(x,y,x+imginfo->w-1,y+imginfo->h-1,imginfo->scan,imgx+ifosize);		
}















