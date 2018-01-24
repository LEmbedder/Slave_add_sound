/*
 * Filename:	Flash.h
 * Author:		Z.C. Liang
 * Date:		2015-08-17
 * Version:		v0.1
 * Discription:	W25Q128F VSG Flash driver
 */

#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x.h"

enum Flash_Status_Reg {
	Flash_SR_1 = 0,
	Flash_SR_2 = 1,
	Flash_SR_3 = 2
};


void Flash_Init(void);

void Flash_Write_Status_Reg(enum Flash_Status_Reg reg, u8 val);
u8 Flash_Read_Status_Reg(enum Flash_Status_Reg reg);

void Flash_Chip_Erase(void);
void Flash_4K_Erase(unsigned int addr);
void Flash_4K_Erase_Start(unsigned int addr);
void Flash_32K_Erase(unsigned int addr);
void Flash_64K_Erase(unsigned int addr);

void Flash_Write_Page(unsigned int addr, u8 *buf, int size);
void Flash_Write(unsigned int addr, u8 *buf, int size);

void Flash_Read(unsigned int addr, u8 *buf, int size);


#endif

