/*
 * Filename:	Display.c
 * Author:		ZouChao, C.M.16
 * Date:		2015-05-04
 * Version:		v0.1
 * Discription:	display api
 */

#include "Display.h"
#include "Lcd.h"
#include "ascii_8x16.h"
#include "File.h"
#include <stdlib.h>

#define C_Char_Width		8
#define C_Char_Height		16

#define TransparentColor   0xe01f	//0x1fe0

struct Graphic {
	u16 id;
	s16 x;
	s16 y;
//	u8 layer;
};

#define GRAPH_LAYER_SIZE      8
#define GRAPH_MAX_SIZE        8
//static int g_layers[GRAPH_MAX_SIZE];
static struct Graphic g_graphs[GRAPH_LAYER_SIZE*GRAPH_MAX_SIZE];

static u16 R_DisplayBuffer[C_LCD_Height][C_LCD_Width];

u8 * Getdisplaybuf()
{
	return (u8 *)R_DisplayBuffer;
}

static void PutChar8x16(s16 x, s16 y, u8 ch);
void RGBtosixteen(u16 *out,u32 *in,int len);
static int GetVisibleRegion(s16 x, s16 y, u8 width, u8 height,
						s16 *x_start, s16 *y_start, u8 *w_start, u8 *h_start,
						u8 *w, u8 *h);

/************************************** function ***************************************************/
/*Display Initialization*/
static u8 displayisinit=0;
extern uint32_t statusreg;
void Display_Initialization(void)
{
	int i;	
	if(displayisinit)
		return;
	displayisinit=1;
	Lcd_Init();
	if(0x0c000000==statusreg)
	{
		ClearScreen();
		RefreshScreen();
	}
	Lcd_initOn();
	for (i = 0; i < GRAPH_LAYER_SIZE*GRAPH_MAX_SIZE; i++) {
		g_graphs[i].id = 0xffff;
	}
}

/*Put Char to LCD*/
void PutChar(s16 x, s16 y, u8 ch)
{
	PutChar8x16(x, y, ch);
}

/*Put String to LCD*/
void PutString(s16 x, s16 y, u8 *str)
{
	u16 i;

	i = 0;
	while(str[i])
	{
		PutChar8x16(x, y, str[i]);
		x += 8;
		i++;
	}
}

/*Put Hex ASCII to LCD*/
void PutHex(s16 x, s16 y, u8 hex)
{
	u8 tmp, ch, i;

	tmp = hex>>4;
	for(i = 0; i < 2; i++)
	{
		if(tmp < 10)
			ch = '0'+tmp;
		else
			ch = 'a'-10+tmp;
		PutChar(x, y, ch);

		x += C_Char_Width;
		tmp = hex&0x0f;
	}
}

/*Put Hex Block*/
void PutHexBlock(s16 x, s16 y, u8 l, u8 *block, u8 size)
{
	u8 i, j;

	j = 0;
	for(i = 0; i < size; i++)
	{
		PutHex(x+20*j, y, block[i]);
		j++;
		if(j >= l)
		{
			j = 0;
			y += 15;
		}
	}
}

/*Put Num to Lcd*/
void PutNum(s16 x, s16 y, s32 num)
{
	u32 value = 10, tmpnum;
	u8 i = 1;

	if(num >= 0)
	{
		tmpnum = num;
	}
	else
	{
		tmpnum = -num;
		PutChar(x, y, '-');
		x += C_Char_Width;
	}

	if(tmpnum)
	{
		while(value <= tmpnum)
		{
			i++;
			value *= 10;
		}
	}

	x += (i-1)*C_Char_Width;
	while(i--)
	{
		PutChar(x, y, '0'+tmpnum%10);
		x -= C_Char_Width;
		tmpnum /= 10;
	}
}


/*Put 8x16 Char*/
static void PutChar8x16(s16 x, s16 y, u8 ch)
{
	u16 offset;
	u8 dta, i, j;

	if(x < 0 && x > (C_LCD_Width-C_Char_Width) && y < 0 && y > (C_LCD_Height-C_Char_Height))
		return;

	offset = (u16)ch;
	offset <<= 4;
	for(i = 0; i < C_Char_Height; i++)
	{
		dta = TB_ASCII8X16[offset];
		for(j = 0; j < C_Char_Width; j++)
		{													
			if(dta&0x80)
			{
				R_DisplayBuffer[y+i][x+j] = Color_Font;
			}
			else
			{
				R_DisplayBuffer[y+i][x+j] = Color_BackGround;
			}
			dta <<= 1;
		}
		offset++;
	}
}

//x,y : 开始显示的位置
//width,height : 照片像素的宽和高
//w,h :在频中显示的宽和高


static int GetVisibleRegion(s16 x, s16 y, u8 width, u8 height,
						s16 *x_start, s16 *y_start, u8 *w_start, u8 *h_start,
						u8 *w, u8 *h)
{
	if (x >= C_LCD_Width) {
		return -1;
	} else if (x >= 0) {
		*x_start = x;
		*w_start = 0;
	} else if ((x + width) > 0) {
		*x_start = 0;
		*w_start = (u8)(-x);
	} else {
		return -1;
	}

	if ((x + width)	> C_LCD_Width) {
		*w = C_LCD_Width - *x_start;
	} else {
		*w = width - *w_start;
	}

	if (y >= C_LCD_Height) {
		return -1;
	} else if (y >= 0) {
		*y_start = y;
		*h_start = 0;
	} else if ((y + height) > 0) {
		*y_start = 0;
		*h_start = (u8)(-y);
	} else {
		return -1;
	}

	if ((y + height) > C_LCD_Height) {
		*h = C_LCD_Height - *y_start;
	} else {
		*h = height - *h_start;
	}
	
	return 0;
}


void PutGraphBuf(u16 *buf, u8 width, u8 height, s16 x, s16 y)
{
	u8 ws, w, hs, h;
	s16 xs, ys;
	u16 *p1, *p2;
	u16 i, j;

	GetVisibleRegion(x, y, width, height, &xs, &ys, &ws, &hs, &w, &h);

	for (i = 0; i < h; i++) {
		p1 = &buf[width * (hs + i) + ws];
		p2 = (u16 *)(&R_DisplayBuffer[ys + i][xs]);
		
		for (j = 0; j < w; j++) {
			if (*p1 != TransparentColor)     // 去掉透明色
				*p2 = *p1;
			p2++;
			p1++;
		}
	}
}

//u16 graphID : 图片ID
//x,y : 开始显示的位置
/*Put Graph to Lcd*/
void PutGraph(u16 graphID, s16 x, s16 y)
{
	struct Graph_Info info;
	u16 sum;
	u8 ws, w, hs, h,adjusth;
	s16 xs, ys;
	u16 *p;
	u16 i, j;
	u16 buf[C_LCD_Width];
	u32 addr;
	u16 _adjustwidth;
	
	if(graphID==0xfffe)
	{
		RGB2Y();
		return;
	}

	sum = File_GetGraphSum(graphID);
	if ((graphID&0x7fff) >= sum)
		return;

	File_GetGraphInfo(graphID, &info);

	if (GetVisibleRegion(x, y, info.width, info.height, &xs, &ys, &ws, &hs, &w, &h) != 0)
		return;
	if(info.is_palette == 0){
	adjusth=info.height-1-hs;//-i	
	
	_adjustwidth=(info.width+1)>>1;
	_adjustwidth<<=1;
	
	for (i = 0; i < h; i++) {
//		addr = info.addr + (info.width * (hs + i) + ws) * 2;
		addr = info.addr + (_adjustwidth * (adjusth -i) + ws) * 2;
		File_Read(addr, (u8 *)buf, w * 2);
		p = (u16 *)(&R_DisplayBuffer[ys + i][xs]);
		
		for (j = 0; j < w; j++) {
			if (buf[j] != TransparentColor)    // 去掉透明色
				*p = buf[j];
			p++;
		}
	}
 }
 
 	if(info.is_palette == 1){
		u16 i, j,k = 0;
		u8 buf1[C_LCD_Width/2];
		u8 converbuf[C_LCD_Width];
		u32 paletbuf[100];
		u16 sixteen_paletbuf[100];
		u32 addr,paddr;
		u16 _adjustwidth;
		unsigned char pixel0, pixel1;
		
		adjusth=info.height-1-hs;  //-i	
		
		
		_adjustwidth=((info.width + 1)/2); 
		_adjustwidth=((_adjustwidth + 3)/4)*4;
		
		paddr = info.paletteaddr;                                                   //调色板的地址
		for(i = 0;i < info.biBitCount*16/4;i++){
			 File_Read(paddr, (u8 *)(paletbuf + i), 4);
			 paddr = paddr + 4;
		}
		RGBtosixteen(sixteen_paletbuf,paletbuf,16);
		
		ws = ws / 2;
		for (i = 0; i < h; i++) {
				addr = info.addr + (_adjustwidth * (adjusth -i) + ws);                //
				File_Read(addr, (u8 *)buf1, _adjustwidth);                                         //读取位图数据(一个字节)两个像素占一个字节
				p = (u16*)(&R_DisplayBuffer[ys + i][xs]);                                //
				for (j = 0; j < (w/2); j++) {
					pixel0  = (buf1[j]>>4) & 0x0f;
					pixel1  = buf1[j] & 0x0f;
					
					if (sixteen_paletbuf[pixel0] != TransparentColor ){                          // 去掉透明色
						 *p = sixteen_paletbuf[pixel0];
					} 			
					p += 1;					
					
					if (sixteen_paletbuf[pixel1] != TransparentColor ){                         // 去掉透明色
						 *p = sixteen_paletbuf[pixel1];
					}
					p += 1;	
			}
		}
  }
}

void AddGraph(u16 id, s16 x, s16 y, u8 layer)
{
	int i = 0;
	int index;
	for (i =0 ; i < GRAPH_MAX_SIZE; i++) 
	{
		index=GRAPH_MAX_SIZE*layer+i;
		if (0xffff==g_graphs[index].id )
		{
			g_graphs[index].id = id;
			g_graphs[index].x = x;
			g_graphs[index].y = y;
			break;
		}
	}
}

void ClearGraphLayer(u8 layer)
{
	int i;
	int index;
	for (i =0 ; i < GRAPH_MAX_SIZE; i++) 
	{
		index=GRAPH_MAX_SIZE*layer+i;
		g_graphs[index].id = 0xffff;
	}
}

void stCheck(void);
void RefreshGraphs(void)
{
	int index ;
	
	FrushDspBuffer(Color_BackGround);
	
	for (index = 0; index < GRAPH_MAX_SIZE*GRAPH_LAYER_SIZE; index++) {
		
		if (g_graphs[index].id != 0xffff) {
			PutGraph(g_graphs[index].id, g_graphs[index].x, g_graphs[index].y);
			stCheck();
		}
	}
	RefreshScreen();
}

/*Clear Screen*/
void ClearScreen(void)
{
	FrushDspBuffer(Color_BackGround);
	
}
extern u8 Bat_IsChargForSlave(void);
extern u8 isBatCharging;
#include "data_bmp00.h"
/*Refresh Screen*/
void RefreshScreen(void)
{
	//20161208 added
	if(1==isBatCharging)
	{
		PutGraph(Graph_Pwr3Charg, 90 + 11, 2 + 4);
	}
	
	Lcd_RefreshScreen(&(R_DisplayBuffer[0][0]));
}

void FrushRectangle(u16 color, s16 x, s16 y, s16 width, s16 height)
{
	s16 i, j;
	
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			R_DisplayBuffer[y + i][x + j] = color;
		}
	}
}

/*Frush Buffer*/
void FrushDspBuffer(u16 _color)
{
	u32 *pu32;
	u32 color;
	u16 *pu16;
	u32 j;
	
#if 0==(C_LCD_Height*C_LCD_Width)%2
	if(0==(u32)R_DisplayBuffer%4)
	{
		color=_color<<16;
		color|=_color;
		pu32=(u32 *)R_DisplayBuffer;
		j=C_LCD_Height*C_LCD_Width;
		j<<=1;
		j+=(u32)pu32;
		
		for(;(u32)pu32<j;)
		{
			*pu32++=color;
		}
		return;
	}
#endif	
	pu16=(u16 *)R_DisplayBuffer;	
	j=C_LCD_Height*C_LCD_Width;
	j<<=1;
	j+=(u32)pu16;
	for(;(u32)pu16<j;)
	{
		*pu16++=_color;
	}
		
//	
//	u8 x, y;
//	for(y = 0; y < C_LCD_Height; y++)
//	{
//		for(x = 0; x < C_LCD_Width; x++)
//		{
//			R_DisplayBuffer[y][x] = _color;
//		}
//	}
}

#define C_DisplayFLAG_UPDATE	1
static u32 DisplayFLAG=0;
void SetDisplayUpdate()
{
	DisplayFLAG|=C_DisplayFLAG_UPDATE;
}

void Display_CheckUpdate()
{
	if(DisplayFLAG&=C_DisplayFLAG_UPDATE)
	{
		RefreshGraphs();
		DisplayFLAG&=(u32)~C_DisplayFLAG_UPDATE;
	}
}

void RGB2Y(void)
{
	u16 *p=(u16 *)&R_DisplayBuffer;	
	int cnt=C_LCD_Height*C_LCD_Width;	
//	unsigned int color;
//	unsigned int temp;

//	for(int i=0;i<cnt;i++)
//	{
//		color=*p;
//		temp=color/3+(color<<5)/3+(color<<11)/3;
//		temp&=0xf800; 
//		color=temp|(temp>>5)|(temp>>11);
//		*p=color;
//		p++;
//	}
	
	u16 t,sour;
  u16 r, g, b;
	for(int i=0;i<cnt;i++)
	{	
		sour=*p;
    r= sour >> 11;
    g= (0x07e0 & sour) >> 6;
    b= 0x1f & sour;
    t = (r*3+b*6+g)/10;
   *p = (t<<11)|(t<<6)|t;
		p++;
	}	
}

/**************************************** console ***************************************************************/
static STRUCT_CONSOLE R_ConsoleReg;

static void Console_NextLine(void)
{
	R_ConsoleReg.x = 1;
	R_ConsoleReg.y += 15;
	if(R_ConsoleReg.y >= (C_LCD_Height-16))
	{
		R_ConsoleReg.y = 20;
		FrushDspBuffer(Color_BackGround);
		PutString(0, 0, R_ConsoleReg.title);
	}
}

/*Open Console*/
void Console_Open(u8 *title)
{
	R_ConsoleReg.x = 1;
	R_ConsoleReg.y = 20;
	R_ConsoleReg.backgroud = Color_BackGround;
	R_ConsoleReg.title = title;

	FrushDspBuffer(Color_BackGround);
	PutString(0, 0, title);
	RefreshScreen();
}

/*Add String*/
void Console_Add_String(u8 *str)
{
	u8 i;

	i = 0;
	while(str[i])
	{
		if(R_ConsoleReg.x >= C_LCD_Width-8)
		{
			Console_NextLine();
		}

		if(str[i] == 0x0d || str[i] == 0x0a)
		{
			Console_NextLine();
		}
		else
		{
			PutChar(R_ConsoleReg.x, R_ConsoleReg.y, str[i]);
			R_ConsoleReg.x += 8;
		}
		i++;
	}
	RefreshScreen();
}

/*Add Hex*/
void Console_Add_Hex(u8 *hexbuf, u8 size)
{
	u8 i;
	u8 hex;

	if(size == 0)
	{
		return ;
	}

	for(i = 0; i < size; i++)
	{
		hex = hexbuf[i];
		if(R_ConsoleReg.x >= C_LCD_Width-16)
		{
			Console_NextLine();
		}
		else
		{
			PutHex(R_ConsoleReg.x, R_ConsoleReg.y, hex);
			R_ConsoleReg.x += 18;
		}
	}
	RefreshScreen();
}

/*Clear Console*/
void Console_Clear(void)
{
	R_ConsoleReg.x = 1;
	R_ConsoleReg.y = 20;
	FrushDspBuffer(Color_BackGround);
	PutString(0, 0, R_ConsoleReg.title);
	RefreshScreen();
}


void RGBtosixteen(u16 *out,u32 *in,int len)
{
    int i;
	  u8 r,g,b;
	  for(i = 0;i < len;i++) 
  	{
     r = (in[i] & 0x00ff0000)>>19;
     g = (in[i] & 0x0000ff00)>>10;
     b = (in[i] & 0x000000ff)>>3;	
			
     out[i] = (r<<11) + (g<<5) + b;
    }
}













