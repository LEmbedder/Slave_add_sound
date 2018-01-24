/*
 * Filename:	Display.h
 * Author:		ZouChao, C.M.16
 * Date:		2015-05-04
 * Version:		v0.1
 * Discription:	display api
 */

#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include "stm32f10x.h"

#define Color_BackGround	WHITE
#define Color_Font			BLACK

typedef struct {
	u8 *title;
	u8 x;
	u8 y;
	u16 backgroud;
}STRUCT_CONSOLE, *PSTRUCT_CONSOLE;


/*Display Initialization*/
void Display_Initialization(void);
/*Clear Screen*/
void ClearScreen(void);
/*Refresh Screen*/
void RefreshScreen(void);
/*Frush Buffer*/
void FrushDspBuffer(u16 color);

void FrushRectangle(u16 color, s16 x, s16 y, s16 width, s16 height);

/*Put Char to LCD*/
void PutChar(s16 x, s16 y, u8 ch);
/*Put String to LCD*/
void PutString(s16 x, s16 y, u8 *str);
/*Put Hex ASCII to LCD*/
void PutHex(s16 x, s16 y, u8 hex);
/*Put Hex Block*/
void PutHexBlock(s16 x, s16 y, u8 l, u8 *block, u8 size);
/*Put Num to Lcd*/
void PutNum(s16 x, s16 y, s32 num);

void PutGraphBuf(u16 *buf, u8 width, u8 height, s16 x, s16 y);
/*Put Graph to Lcd*/
void PutGraph(u16 graphID, s16 x, s16 y);

void AddGraph(u16 id, s16 x, s16 y, u8 layer);
void ClearGraphLayer(u8 layer);
void RefreshGraphs(void);

void SetDisplayUpdate(void);
void Display_CheckUpdate(void);

/*Open Console*/
void Console_Open(u8 *title);
/*Add String*/
void Console_Add_String(u8 *str);
/*Add Hex*/
void Console_Add_Hex(u8 *hexbuf, u8 size);
/*Clear Console*/
void Console_Clear(void);

void RGB2Y(void);

#endif/*__DISPLAY_H_*/

/*
 * need to add set background color function
 * need to add set transparent color function
 */
