
#ifndef __IC_SPI_H_
#define __IC_SPI_H_

#include "stm32f10x.h"
#include "CMLIB.h"

#define C_IC_Spi_None		0xFF

#define C_IC_SPI_Mode_Master	0
#define C_IC_SPI_Mode_Slaver	1

//#define _SPI_SLAVER_
//#define _SPI_MASTER_
//#ifdef _SPI_SLAVER_
//#endif /*_SPI_SLAVER_*/
//#ifdef _SPI_MASTER_
//#endif /*_SPI_MASTER_*/

/*IC SPI Initialization*/
void IC_SPI_Init(u8 mode);

// api for slaver
/*Read Byte*/
BOOL IC_SPI_ReadByte(u8 *dat);
/*Read Block*/
BOOL IC_SPI_ReadBlock(u8 *buffer, u16 size, u32 timeoutCnt);
/*Write Byte*/
void IC_SPI_WriteByte(u8 dat);
/*Write Block*/
void IC_SPI_WriteBlock(u8 *buffer, u16 size);
/*Write String*/
void IC_SPI_WriteString(u8 *str);

/*Reset Recieve Buffer*/
void IC_SPI_RxBuffer_Reset(void);
/*Reset Transport Buffer*/
void IC_SPI_TxBuffer_Reset(void);

// api for master
/*IC SPI Read Write*/
u8 IC_SPI_RW(u8 slaverID, u8 dat);
/*IC SPI Read Write Block*/
void IC_SPI_RW_Block(u8 slaverID, u8 *txBuf, u32 txSize, u8 *rxBuf);

#endif /*__IC_SPI_H_*/
