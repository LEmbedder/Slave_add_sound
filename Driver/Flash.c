/*
 * Filename:	Flash.c
 * Author:		Z.C. Liang
 * Date:		2015-08-17
 * Version:		v0.1
 * Discription:	W25Q128F VSG Flash driver
 */

#include "Flash.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "CMOS.h"
#include "Display.h"


#define F_CS_SET     GPIOC->BSRR = GPIO_Pin_13
#define F_CLK_SET    GPIOC->BSRR = GPIO_Pin_14
#define F_WP_SET     GPIOB->BSRR = GPIO_Pin_8
#define F_RST_SET    GPIOB->BSRR = GPIO_Pin_9
#define F_DI_SET     GPIOB->BSRR = GPIO_Pin_6
#define F_DO_SET     GPIOB->BSRR = GPIO_Pin_7

#define F_CS_CLR     GPIOC->BRR = GPIO_Pin_13
#define F_CLK_CLR    GPIOC->BRR = GPIO_Pin_14
#define F_WP_CLR     GPIOB->BRR = GPIO_Pin_8
#define F_RST_CLR    GPIOB->BRR = GPIO_Pin_9
#define F_DI_CLR     GPIOB->BRR = GPIO_Pin_6
#define F_DO_CLR     GPIOB->BRR = GPIO_Pin_7 



static void F_WR_Start(void);
static void F_WR_Stop(void);
static void F_SPI_W(u8 data);
static u8 F_SPI_R(void);
static void F_QPI_W(u8 data);
static u8 F_QPI_R(void);

static int F_SPI_Check_JEDEC_ID(void);
static int F_QPI_Check_JEDEC_ID(void);

static void F_Enter_QPI_Mode(void);

static void Flash_Write_Enable(void);
static void Flash_Wait_Done(void);

static void Flash_Set_Read_Param(u8 dummy_clocks, u8 wrap_length);


static void F_GPIO_SPI_Mode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	F_CS_SET;
	F_CLK_SET;
	F_WP_SET;
	F_RST_SET;
}

static void F_GPIO_QPI_Mode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	F_CS_SET;
	F_CLK_SET;
}

static void F_DB_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void F_DB_In(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void Flash_Init(void)
{
	u8 status;

	F_GPIO_SPI_Mode();

	if (F_SPI_Check_JEDEC_ID() == 0) {
		F_Enter_QPI_Mode();
	}

	F_GPIO_QPI_Mode();

	if (F_QPI_Check_JEDEC_ID() == 0) {

	} else {
		Display_Initialization();
		Console_Open((u8 *)"Flash ERR");
		while(1);
	}

	// write protection disable
	status = Flash_Read_Status_Reg(Flash_SR_1);
	status = status & (~0x7c);
	Flash_Write_Status_Reg(Flash_SR_1, status);
	status = Flash_Read_Status_Reg(Flash_SR_2);
	status = status & (~0x40);
	Flash_Write_Status_Reg(Flash_SR_2, status);
	status = Flash_Read_Status_Reg(Flash_SR_3);
	status = status & (~0x04);
	Flash_Write_Status_Reg(Flash_SR_3, status);
}

void Flash_Write_Status_Reg(enum Flash_Status_Reg reg, u8 val)
{
	u8 cmd;

	if (reg == Flash_SR_1) {
		cmd = 0x01;
	} else if (reg == Flash_SR_2) {
		cmd = 0x31;
	} else if (reg == Flash_SR_3) {
		cmd = 0x11;
	} else {
		cmd = 0x01;
	}

	Flash_Write_Enable();

	F_WR_Start();
	F_QPI_W(cmd);
	F_QPI_W(val);
	F_WR_Stop();

	Flash_Wait_Done();
}

u8 Flash_Read_Status_Reg(enum Flash_Status_Reg reg)
{
	u8 cmd, status;

	if (reg == Flash_SR_1) {
		cmd = 0x05;
	} else if (reg == Flash_SR_2) {
		cmd = 0x35;
	} else if (reg == Flash_SR_3) {
		cmd = 0x15;
	} else {
		cmd = 0x05;
	}

	F_WR_Start();
	F_QPI_W(cmd);
	F_DB_In();
	status = F_QPI_R();
	F_DB_Out();
	F_WR_Stop();

	return status;
}

void Flash_Chip_Erase(void)
{
	Flash_Write_Enable();

	F_WR_Start();
	F_QPI_W(0x60);
	F_WR_Stop();

	Flash_Wait_Done();
}

void Flash_4K_Erase(unsigned int addr)
{
	Flash_Write_Enable();

	F_WR_Start();
	F_QPI_W(0x20);
	F_QPI_W((addr >> 16) & 0xff);
	F_QPI_W((addr >> 8) & 0xff);
	F_QPI_W(addr & 0xff);
	F_WR_Stop();

	Flash_Wait_Done();
}

void Flash_4K_Erase_Start(unsigned int addr)
{
	Flash_Write_Enable();

	F_WR_Start();
	F_QPI_W(0x20);
	F_QPI_W((addr >> 16) & 0xff);
	F_QPI_W((addr >> 8) & 0xff);
	F_QPI_W(addr & 0xff);
	F_WR_Stop();
}

void Flash_32K_Erase(unsigned int addr)
{
	Flash_Write_Enable();

	F_WR_Start();
	F_QPI_W(0x52);
	F_QPI_W((addr >> 16) & 0xff);
	F_QPI_W((addr >> 8) & 0xff);
	F_QPI_W(addr & 0xff);
	F_WR_Stop();

	Flash_Wait_Done();
}

void Flash_64K_Erase(unsigned int addr)
{
	Flash_Write_Enable();

	F_WR_Start();
	F_QPI_W(0xd8);
	F_QPI_W((addr >> 16) & 0xff);
	F_QPI_W((addr >> 8) & 0xff);
	F_QPI_W(addr & 0xff);
	F_WR_Stop();

	Flash_Wait_Done();
}

void Flash_Write(unsigned int addr, u8 *buf, int size)
{
	u8 tmp[256];
	int _len=0;
	u8 *p=buf;
	if(size<=0)
		return;
	int start=(u32)(addr&0xffffff00);
	if(start!=addr)
	{
		Flash_Read(start,tmp,256);
		int index=addr%256;
		while(index<256)
		{
			tmp[index++]=*p++;
			size--;
			if(0==size)
				break;
		}
		Flash_Write_Page(start,tmp,256);
		start+=256;
	}
	while(size>0)
	{
		_len=256;
		if(size<256)
			_len=size;
		Flash_Write_Page(start,p,_len);
		start+=_len;
		p+=_len;
		size-=_len;
	}
}

void Flash_Write_Page(unsigned int addr, u8 *buf, int size)
{
	int i;
	Flash_Wait_Done();
	Flash_Write_Enable();

	F_WR_Start();

	F_QPI_W(0x02);
	F_QPI_W((addr >> 16) & 0xff);
	F_QPI_W((addr >> 8) & 0xff);
	F_QPI_W(addr & 0xff);

	for (i = 0; i < size; i++) {
		F_QPI_W(buf[i]);
	}

	F_WR_Stop();

	Flash_Wait_Done();
}

void Flash_Read(unsigned int addr, u8 *buf, int size)
{
	u16 data;
	long int i;
	Flash_Wait_Done();
	Flash_Set_Read_Param(1, 0);

	F_WR_Start();

	F_QPI_W(0x0b);
	F_QPI_W((addr >> 16) & 0xff);
	F_QPI_W((addr >> 8) & 0xff);
	F_QPI_W(addr & 0xff);

	F_CLK_SET;
	F_CLK_CLR;
	F_CLK_SET;
	F_CLK_CLR;
	F_CLK_SET;
	F_CLK_CLR;
	F_CLK_SET;
	F_CLK_CLR;

	F_DB_In();
	
//	SPIREAD(buf,size);
	
	for (i = 0; i < size; i++) {
		buf[i] = F_QPI_R();
		
		
		
//		F_CLK_SET;
//		data=GPIOB->IDR;
//		F_CLK_CLR;
//		
//		data = data << 4;
//		data = data&0x3c00;
//		
//		F_CLK_SET;
//		data =data | GPIOB->IDR;
//		F_CLK_CLR;
//		
//		data = data &0x3fc0;
//		data = data>>6;
//		buf[i] = (u8)data;
		
	}
	F_DB_Out();

	F_WR_Stop();
}


static void Flash_Set_Read_Param(u8 dummy_clocks, u8 wrap_length)
{
	u8 data;

	data = ((dummy_clocks & 0x03) << 4)	| (wrap_length & 0x03);

	F_WR_Start();
	F_QPI_W(0xc0);
	F_QPI_W(data);
	F_WR_Stop();
}

static void Flash_Write_Enable(void)
{
	F_WR_Start();
	F_QPI_W(0x06);
	F_WR_Stop();
}

static void Flash_Wait_Done(void)
{
	F_WR_Start();
	F_QPI_W(0x05);
	F_DB_In();
	while (F_QPI_R() & 0x01)
		;
	F_DB_Out();
	F_WR_Stop();
}

u8 Flash_IsBusy(void)
{
	u8 ret;
	F_WR_Start();
	F_QPI_W(0x05);
	F_DB_In();
	ret=(F_QPI_R() & 0x01);
	F_DB_Out();
	F_WR_Stop();
	return ret;
}

static void F_Enter_QPI_Mode(void)
{
	u8 reg1, reg2;

	F_WR_Start();    // read status register 2
	F_SPI_W(0x35);
	reg2 = F_SPI_R();
	F_WR_Stop();

	reg2 = reg2 | 0x02;

	F_WR_Start();	  // write enable
	F_SPI_W(0x06);
	F_WR_Stop();

	F_WR_Start();	  // write status register 2
	F_SPI_W(0x31);
	F_SPI_W(reg2);
	F_WR_Stop();

	F_WR_Start();
	F_SPI_W(0x05);
	reg1 = F_SPI_R();
	while (reg1 & 0x01)
		reg1 = F_SPI_R();
	F_WR_Stop();

	F_WR_Start();	   //  enter QPI 
	F_SPI_W(0x38);
	F_WR_Stop();
}


static int F_SPI_Check_JEDEC_ID(void)
{
	u8 mf, idh, idl;

	F_WR_Start();

	F_SPI_W(0x9f);
	mf = F_SPI_R();
	idh = F_SPI_R();
	idl = F_SPI_R();
	
	F_WR_Stop();
	
	if ((mf == 0xef) && (idh == 0x40) && (idl == 0x18))
		return 0;
	else
		return -1; 
}

static int F_QPI_Check_JEDEC_ID(void)
{
	u8 mf, idh, idl;

	F_WR_Start();

	F_QPI_W(0x9f);
	F_DB_In();
	mf = F_QPI_R();
	idh = F_QPI_R();
	idl = F_QPI_R();
	F_DB_Out();

	F_WR_Stop();

	if ((mf == 0xef) && (idh == 0x60) && (idl == 0x18))
		return 0;
	else
		return -1; 
}

static void F_WR_Start(void)
{	  
 	F_CLK_CLR;
	F_CS_CLR;
}

static void F_WR_Stop(void)
{
	F_CS_SET;
	F_CLK_SET;
}

static void F_SPI_W(u8 data)
{
	u8 i;

	for (i = 0; i < 8; i++) {
		if (data & 0x80)
			F_DI_SET;
		else
			F_DI_CLR;

		F_CLK_SET;
		F_CLK_CLR;

		data =  data << 1;
	}
}

static u8 F_SPI_R(void)
{
	u8 ret, i;

	for (i = 0; i < 8; i++) {
		ret = ret << 1;

		F_CLK_CLR;

		if (GPIOB->IDR & GPIO_Pin_7)
			ret = ret + 1;

		F_CLK_SET;
	}

	return ret;
}

static void F_QPI_W(u8 data)
{
	GPIOB->BSRR = (data & 0xf0) << 2;
	GPIOB->BRR = (~data	& 0xf0) << 2;
	
	F_CLK_SET;
	F_CLK_CLR;

	GPIOB->BSRR = (data & 0x0f) << 6;
	GPIOB->BRR = (~data & 0x0f) << 6;

	F_CLK_SET;
	F_CLK_CLR;
}

static u8 F_QPI_R(void)
{
	u8 ret;

	F_CLK_CLR;
	ret = GPIOB->IDR >> 2;
	F_CLK_SET;

	F_CLK_CLR;
	ret = (ret & 0xf0) | ((GPIOB->IDR >> 6) & 0x0f);
	F_CLK_SET;

	return ret;
}

