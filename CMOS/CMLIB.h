/****************************************
Author		CM16
Data		2014/10/25
File		CMLIB.h
Type		OS_API
Description
 - CM Library
*****************************************/
#ifndef __CMLIB_H_
#define __CMLIB_H_

#include "stm32f10x.h"

#define WORD u16
#define word u16
#define BYTE u8
#define byte u8

typedef float f32;

#ifndef _BOOLEAN_
#define _BOOLEAN_
typedef enum
{
	TRUE = 0,
	FALSE
}boolean;
#endif /*_BOOLEAN_*/

#define BOOL	boolean
//fifo
typedef struct
{
	u16 pWrite;
	u16 pRead;
	u16 size;
}STRUCT_FIFO_REG, *PSTRUCT_FIFO_REG;

#define ABS(x) x = (x >= 0? x: -x)

/*FIFO Initialization*/
void FIFO_Init(PSTRUCT_FIFO_REG pFifoReg, u16 size);
/*Reset FIFO Reg*/
void FIFO_Reset(PSTRUCT_FIFO_REG pFifoReg);
/*Read a byte from FIFO Queue*/
BOOL FIFO_ReadData(u8 *fifobuf, PSTRUCT_FIFO_REG pFifoReg, u8 *dat);
/*Write data to FIFO Queue*/
BOOL FIFO_WriteData(u8 *fifobuf, PSTRUCT_FIFO_REG pFifoReg, u8 dat);

BOOL FIFO_GetPopIndex(PSTRUCT_FIFO_REG pFifoReg, u8 *pIndex);
BOOL FIFO_GetPushIndex(PSTRUCT_FIFO_REG pFifoReg, u8 *pIndex);

f32 GetCeiling(f32 dat);
f32 GetFloor(f32 dat);
void ccopy(u8* src,u8* des,u32 length);
void cset(u8 *des,u8 value,u32 size);

#endif /* __CMLIB_H_ */
