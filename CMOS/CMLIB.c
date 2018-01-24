/****************************************
Author		CM16
Data		2014/10/25
File		CMLIB.c
Type		OS_API
Description
 - CM Library
*****************************************/
#include "CMLIB.h"

/*FIFO Initialization*/
void FIFO_Init(PSTRUCT_FIFO_REG pFifoReg, u16 size)
{
	FIFO_Reset(pFifoReg);
	pFifoReg->size = size;
}

/*Reset FIFO Reg*/
void FIFO_Reset(PSTRUCT_FIFO_REG pFifoReg)
{
	pFifoReg->pWrite = 0;
	pFifoReg->pRead = 0;
}

/*Read a byte from FIFO Queue*/
BOOL FIFO_ReadData(u8 *fifobuf, PSTRUCT_FIFO_REG pFifoReg, u8 *dat)
{
	if(pFifoReg->pWrite == pFifoReg->pRead)
		return FALSE;

	if(pFifoReg->pRead >= pFifoReg->size)
	{
		FIFO_Reset(pFifoReg);
		return FALSE;
	}
	*dat = fifobuf[pFifoReg->pRead];
	pFifoReg->pRead++;
	pFifoReg->pRead %= pFifoReg->size;

	return TRUE;
}


/*Write data to FIFO Queue*/
BOOL FIFO_WriteData(u8 *fifobuf, PSTRUCT_FIFO_REG pFifoReg, u8 dat)
{
	u16 pwrite;

	pwrite = pFifoReg->pWrite;
	pwrite++;
	pwrite %= pFifoReg->size;
	if(pwrite == pFifoReg->pRead)
		return FALSE;

	fifobuf[pFifoReg->pWrite] = dat;
	pFifoReg->pWrite = pwrite;
	return TRUE;
}

/*Get FIFO Pop Index*/
BOOL FIFO_GetPopIndex(PSTRUCT_FIFO_REG pFifoReg, u8 *pIndex)
{
	BOOL flg;

	if(pFifoReg->pWrite == pFifoReg->pRead)
	{
		flg = FALSE;
	}
	else if(pFifoReg->pRead >= pFifoReg->size)
	{
		FIFO_Reset(pFifoReg);
		flg = FALSE;
	}
	else
	{
		*pIndex = pFifoReg->pRead;
		pFifoReg->pRead++;
		if(pFifoReg->pRead >= pFifoReg->size)
		{
			pFifoReg->pRead = 0;
		}
		flg = TRUE;
	}

	return flg;
}

/*Get FIFO Push Index*/
BOOL FIFO_GetPushIndex(PSTRUCT_FIFO_REG pFifoReg, u8 *pIndex)
{
	u16 pwrite;
	BOOL flg;

	pwrite = pFifoReg->pWrite;
	pwrite++;
	if(pwrite >= pFifoReg->size)
	{
		pwrite = 0;
	}

	*pIndex = pFifoReg->pWrite;
	if(pwrite == pFifoReg->pRead)
	{
		flg = FALSE;
	}
	else
	{
		pFifoReg->pWrite = pwrite;
		flg = TRUE;
	}

	return flg;
}

/*
*Function:		GetCeiling		
*Description:		get ceiling value		
*input:			
*return:		 
*/
f32 GetCeiling(f32 dat)
{
	s32 d;

	d = (s32)dat + 1;
	return (f32)d;
}

/*
 *Function:		GetFloor		
 *Description:		get floor value		
 *input:			
 *return:		 
 */
f32 GetFloor(f32 dat)
{
	s32 d;

	d = (s32)dat;
	return (f32)d;
}

/*
 *Function:		CheckSum		
 *Description:		check sum function		
 *input:			
 *return:		 
 */
u8 CheckSum_Add(u8 *pStr, u32 size)
{
	u8 cs;
	u32 i;

	cs = 0;
	for(i = 0; i < size; i++)
	{
		cs += pStr[i];
	}

	return cs;
}

/*
 *Function:		CheckSum_BCC		
 *Description:		Block Check Character		
 *input:			
 *return:		 
 */
u8 CheckSum_BCC(u8 *pStr, u32 size)
{
	u8 cs;
	u32 i;

	cs = 0;
	for(i = 0; i < size; i++)
	{
		cs ^= pStr[i];
	}
	return cs;
}

void ccopy(u8* src,u8* des,u32 length)
{
	u32 i;
	u32 start=(u32)src;
	u32 end=(u32)des;
	if(start>=end)
	{
		for(i=0;i<length;i++)
			*des++=*src++;
	}else
	{
		src=(u8 *)(start+length-1);
		des=(u8 *)(end+length-1);
		for(i=length;i>0;i--)
			*des--=*src--;
	}
}
  
void cset(u8 *des,u8 value,u32 size)
{
	int i;
	for(i=0;i<size;i++)
	{
		*des++=value;
	}
}

