/*
 * Filename:	lcd.c
 * Author:		Z.C. Liang
 * Date:		2015-07-30
 * Version:		v0.1
 * Discription:	lcd driver
 */

#include "lcd.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "misc.h"
#include "CMOS.h"


#define LCD_CS      GPIO_Pin_4	  // GPIOC
#define LCD_RST     GPIO_Pin_5	  // GPIOC
#define LCD_RD      GPIO_Pin_8	  // GPIOC
#define LCD_WR      GPIO_Pin_7	  // GPIOC
#define LCD_RS      GPIO_Pin_9	  // GPIOC
#define LCD_DB0     GPIO_Pin_5	  // GPIOA
#define LCD_DB1     GPIO_Pin_6	  // GPIOA
#define LCD_DB2     GPIO_Pin_7	  // GPIOA
#define LCD_DB3     GPIO_Pin_8	  // GPIOA
#define LCD_DB4     GPIO_Pin_9	  // GPIOA
#define LCD_DB5     GPIO_Pin_10	  // GPIOA
#define LCD_DB6     GPIO_Pin_11	  // GPIOA
#define LCD_DB7     GPIO_Pin_12	  // GPIOA

#define LCD_CS_SET     GPIOC->BSRR = LCD_CS
#define LCD_RST_SET    GPIOC->BSRR = LCD_RST
#define LCD_RD_SET     GPIOC->BSRR = LCD_RD
#define LCD_WR_SET     GPIOC->BSRR = LCD_WR
#define LCD_RS_SET     GPIOC->BSRR = LCD_RS

#define LCD_CS_CLR     GPIOC->BRR = LCD_CS
#define LCD_RST_CLR    GPIOC->BRR = LCD_RST
#define LCD_RD_CLR     GPIOC->BRR = LCD_RD
#define LCD_WR_CLR     GPIOC->BRR = LCD_WR
#define LCD_RS_CLR     GPIOC->BRR = LCD_RS


static void Lcd_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void DB_Pin_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void DB_Pin_In(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

static void DB_Write(u8 data)
{
	GPIOA->BSRR = data << 5;
	GPIOA->BRR = ~data << 5;
}

static u8 DB_Read(void)
{
	u8 data;

	data = (u8)(GPIOA->IDR >> 5);

	return data;
}

static void Lcd_Reset(void)
{
	LCD_RST_CLR;
	mDelay(10);
	LCD_RST_SET;
	mDelay(10);
}

void Lcd_Init(void)
{
	Lcd_GPIOInit();
	Lcd_Reset();	// open LED+

	Lcd_Write_Cmd(0x3a);       // pixel format
	Lcd_Write_Param(0x05);	   // 16bit/pixel

	Lcd_Write_Cmd(0x36);    
	Lcd_Write_Param(0x68);   //0x68	  // rgb order, 使用横屏
}

static u8 lcdstatu;

void Lcd_initOn(void)
{
	Lcd_Write_Cmd(0x11);     // sleep out
	Lcd_Write_Cmd(0x29);     // display on
	lcdstatu=1;
}

void Lcd_On(void)
{
	if(lcdstatu!=2)
		return;
	Lcd_Init();
	Lcd_Write_Cmd(0x11);     // sleep out
	Lcd_Write_Cmd(0x29);     // display on
	lcdstatu=1;
}

void Lcd_Off(void)
{
	if(lcdstatu!=1)
		return;
	LCD_RST_CLR;	// close LED+
	lcdstatu=2;
}

void Lcd_Set_Window(int x0, int y0, int x1, int y1)
{
	Lcd_Write_Cmd(0x2a);
	Lcd_Write_Param((u8)((x0 >> 8) & 0xff));
	Lcd_Write_Param((u8)(x0 & 0xff));
	Lcd_Write_Param((u8)((x1 >> 8) & 0xff));
	Lcd_Write_Param((u8)(x1 & 0xff));

	Lcd_Write_Cmd(0x2b);
	Lcd_Write_Param((u8)((y0 >> 8) & 0xff));
	Lcd_Write_Param((u8)(y0 & 0xff));
	Lcd_Write_Param((u8)((y1 >> 8) & 0xff));
	Lcd_Write_Param((u8)(y1 & 0xff));
}

void Lcd_Write_Cmd(u8 value)
{
	LCD_CS_CLR;
	LCD_RS_CLR;
	LCD_RD_SET;

	LCD_WR_CLR;
	DB_Write(value);
	LCD_WR_SET;

	LCD_CS_SET;
}

void Lcd_Write_Param(u8 value)
{
	LCD_CS_CLR;
	LCD_RS_SET;
	LCD_RD_SET;

	LCD_WR_CLR;
	DB_Write(value);
	LCD_WR_SET;

	LCD_CS_SET;
}

void Lcd_Write_Params(u8 *param, int param_size)
{
	int i;

	LCD_CS_CLR;
	LCD_RS_SET;
	LCD_RD_SET;

	for (i = 0; i < param_size; i++) {
		LCD_WR_CLR;
		DB_Write(param[i]);
		LCD_WR_SET;
	}

	LCD_CS_SET;
}

void Lcd_Write_Buf(u8 cmd, u8 *param, int param_size)
{
	int i;

	LCD_CS_CLR;
	LCD_RD_SET;
	LCD_RS_CLR;

	LCD_WR_CLR;
	DB_Write(cmd);
	LCD_WR_SET;

	LCD_RS_SET;

	for (i = 0; i < param_size; i+=2) {
		LCD_WR_CLR;
		DB_Write(param[i+1]);
		LCD_WR_SET;
		
		LCD_WR_CLR;
		DB_Write(param[i]);
		LCD_WR_SET;
	}

	LCD_CS_SET;
}

void Lcd_Read_Buf(u8 cmd, u8 *buf, int read_size)
{
	int i;

	LCD_CS_CLR;
	LCD_RS_CLR;
	LCD_RD_SET;

	LCD_WR_CLR;
	DB_Write(cmd);
	LCD_WR_SET;

	DB_Pin_In();
	LCD_RS_SET;

	for (i = 0; i < read_size; i++) {
		LCD_RD_CLR;
		LCD_RD_SET;
		buf[i] = DB_Read();
	}

	DB_Pin_Out();
	LCD_CS_SET;
}

void Lcd_RefreshScreen(u16 *buffer)
{
//	Lcd_Set_Window(0, 0, C_LCD_Width - 1, C_LCD_Height - 1);
	Lcd_Set_Window(1, 2, C_LCD_Width + 0, C_LCD_Height + 1);
	Lcd_Write_Buf(0x2c, (u8 *)buffer, C_LCD_Width * C_LCD_Height * 2);	
}

void Lcd_Rotate_90(void)
{
	Lcd_Write_Cmd(0x36);    
	Lcd_Write_Param(0xc8);
}

void Lcd_Rotate_Default(void)
{
	Lcd_Write_Cmd(0x36);    
	Lcd_Write_Param(0x68);   //0x68	  // rgb order, 使用横屏
}


