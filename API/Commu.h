/*
 * Filename:	Commu.h
 * Author:		C.M.16
 * Date:		2015-10-14
 * Version:		v0.1
 * Discription:	
 * 1. Common file between slaver project and master project, must use the lastest version
   before u update this file.
   2. Last Update	2015-10-14
 */

#ifndef __COMMUNICATE_H_
#define __COMMUNICATE_H_

#include "stm32f10x.h"
#include "CMLIB.h"
#include "IC_SPI.h"

#define _COMMUNICATE_MODE_		0	//0 : slaver mode; 1 : master mode
#if _COMMUNICATE_MODE_ == 0
#define _COMMUNICATE_SLAVER_
#elif _COMMUNICATE_MODE_ == 1
#define _COMMUNICATE_MASTER_
#endif

#define C_Com_SPI_Master	C_IC_SPI_Mode_Master
#define C_Com_SPI_Slaver	C_IC_SPI_Mode_Slaver





#define C_Com_SPI_File_PageSize		256

//Struct_Com_File.rwMode
#define C_Com_File_rwMode_Write		(1<<0)
#define C_Com_File_rwMode_Read		(1<<1)
#define C_Com_File_rwMode_RW		(C_Com_File_rwMode_Write | C_Com_File_rwMode_Read)


//Struct_Com_File.flashArea
typedef enum
{
	C_Com_File_Area_InData = 0,
	C_Com_File_Area_ExtData,
}Enum_Com_File_Area;

typedef struct
{
	u8 flashArea;
	u8 rwMode;
	u32 size;
	u32 ptrPageRead;
	u32 ptrPageWrite;
}Struct_Com_File, *pStruct_Com_File;

#define C_Com_File_RW_EndByte		0x00
#define C_Com_File_RW_ErrPkg		0x15	//package error
#define C_Com_File_RW_ErrStg		0x14	//storage error
#define C_Com_File_RW_ErrMsg		0x13	//Message error
#define C_Com_File_RW_ErrAddr		0x12	//Address error
#define C_Com_File_RW_ErrMode		0x11	//Mode error
#define C_Com_File_RW_ErrConnect	0x10	//Connect error






/*SPI Initialization*/
void Com_SPI_Initialization(u8 mode);
/*Check SPI Message*/
void Com_SPI_CheckMessage(void);



#ifdef _COMMUNICATE_MASTER_	
/*System Update*/
void Com_SPI_SystemUpdate(u8 slaverID);
/*Get nRF ID*/
BOOL Com_SPI_GetnRFID(u8 slaverID, u8 *buf);
/*Set nRF ID*/
void Com_SPI_SetnRFID(u8 slaverID, u8 *buf);

/*File Open*/
BOOL Com_SPI_FileOpen(u8 slaverID, Struct_Com_File *file);
/*File Write*/
BOOL Com_SPI_FileWrite(u8 slaverID, Struct_Com_File *file, u8 *buffer, u8* res);
/*File Read*/
BOOL Com_SPI_FileRead(u8 slaverID, Struct_Com_File *file, u8 *buffer, u8 *res);
/*File Close*/
BOOL Com_SPI_FileClose(u8 slaverID, Struct_Com_File *file);

/*Get Slaver ID*/
BOOL Com_SPI_GetSID(u8 slaverID, u32 *sid);
/*Set Slaver ID*/
void Com_SPI_SetSID(u8 slaverID, u32 sid);

/*Put Graph*/
void Com_SPI_PutGraph(u8 slaverID, u16 graphID, s16 x, s16 y, u8 layer);
/*Clear Layer*/
void Com_SPI_ClearLayer(u8 slaverID, u8 layer);
/*Play HDR*/
void Com_SPI_PlayHDR(u8 slaverID, u16 HDRID, s16 x, s16 y, u8 layer);
/*Get Firmware ID*/
BOOL Com_SPI_GetFirmwareID(u8 slaverID, u8 *FMID);

#endif /*_COMMUNICATE_MASTER_*/




#ifdef _COMMUNICATE_SLAVER_	

#endif /*_COMMUNICATE_SLAVER_*/


#endif /*__COMMUNICATE_H_*/

