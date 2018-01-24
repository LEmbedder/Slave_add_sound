/*
 * Filename:	lcd.c
 * Author:		Z.C. Liang
 * Date:		2015-07-30
 * Version:		v0.1
 * Discription:	lcd driver
 */

#ifndef __LCD_H
#define __LCD_H


#include "stm32f10x.h"

//??????
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


void Lcd_Init(void);
void Lcd_RefreshScreen(u16 *buffer);

void Lcd_Write_Cmd(u8 value);
void Lcd_Write_Param(u8 value);
void Lcd_Write_Params(u8 *param, int param_size);
void Lcd_Write_Buf(u8 cmd, u8 *param, int param_size);
void Lcd_Read_Buf(u8 cmd, u8 *buf, int read_size);
void Lcd_Set_Window(int x0, int y0, int x1, int y1);
void Lcd_On(void);
void Lcd_Off(void);
void Lcd_initOn(void);

void Lcd_Rotate_90(void);
void Lcd_Rotate_Default(void);

#endif
