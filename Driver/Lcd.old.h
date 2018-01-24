/*
 * Filename:	LCD.h
 * Author:		ZouChao, C.M.16
 * Date:		2015-05-04
 * Version:		v0.1
 * Discription:	Lcd driver
 */

#ifndef __LCD_H_
#define __LCD_H_

#include "stm32f10x.h"

//定义常用颜色
#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   
#define GRAY1   0x8410      	
#define GRAY2   0x4208   

#define C_LCD_Height	128
#define C_LCD_Width		128


/*Lcd Initialization*/
void Lcd_Init(void);
/*Refresh Screen*/
void Lcd_RefreshScreen(u16 *buffer);

#endif /*__LCD_H_*/

