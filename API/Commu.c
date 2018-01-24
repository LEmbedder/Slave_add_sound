/*
 * Filename:	Commu.c
 * Author:		C.M.16
 * Date:		2015-10-14
 * Version:		v0.1
 * Discription:	
 * 1. Common file between slaver project and master project, must use the lastest version
   before u update this file.
   2. Last Update	2015-10-14
 */

#include "Commu.h"
#include "CMOS.h"

#ifdef _COMMUNICATE_MASTER_	
#endif

#ifdef _COMMUNICATE_SLAVER_	
#include "Flash.h"
#include "Config.h"
#include "FLASHROM.h"
#include "IAP.h"
#include "Display.h"
#include "GEngineCfg.h"
#include "stm32f10x_iwdg.h"
#endif


typedef enum
{
	//Update System
	Enum_Com_Cmd_SysUpdate = 'A',
	//nRF
	Enum_Com_Cmd_GetnRFID = 1,
	Enum_Com_Cmd_SetnRFID,
	//Nor flash
	Enum_Com_Cmd_FOpen,
	Enum_Com_Cmd_FWrite,
	Enum_Com_Cmd_FRead,
	Enum_Com_Cmd_FClose,
	//ID
	Enum_Com_Cmd_GetSID,
	Enum_Com_Cmd_SetSID,
	//graph
	Enum_Com_Cmd_PutGraph,
	Enum_Com_Cmd_ClearLayer,
	Enum_Com_Cmd_PlayHDR,
	Enum_Com_Cmd_GetFirmwareID,
	Enum_Com_Cmd_Test,
	//Max
	Enum_Com_Cmd_Max,
}Enum_SPI_Cmd_List;


/*SPI Communication Initialization*/
void Com_SPI_Initialization(u8 mode)
{
//	IC_SPI_Init(mode);
}




/********************************* slaver function ************************************/
#ifdef _COMMUNICATE_SLAVER_

void Com_SPI_SystemUpdate(void)
{
	u8 cmdStr[] = "CKT";	//ACKT
	u8 buffer[3];
	u8 i;
	
	IC_SPI_ReadBlock(buffer, 3, 100);
	i = 0;
	while(cmdStr[i] != '\0')
	{
		if(cmdStr[i] != buffer[i])
			break;
		i++;
	}
	if(cmdStr[i] == '\0')
	{
		Resetproisok();
		SystemReset();
//		Iap_RunBootLoader();
	}
	IC_SPI_RxBuffer_Reset();	
}


/*Get nRF ID*/
void Com_SPI_GetnRFID(void)
{
	u8 addr[5];
	u8 add = 0;
	u8 i;
	
	nRF_getAddr(addr);
	IC_SPI_WriteByte(Enum_Com_Cmd_GetnRFID);
	for(i = 0; i < 5; i++)
	{
		IC_SPI_WriteByte(addr[i]);
		add += addr[i];
	}
	IC_SPI_WriteByte(add);
	IC_SPI_RxBuffer_Reset();	
}

/*Set nRF ID*/
void Com_SPI_SetnRFID(void)
{
	u8 addr[6];
	u8 add = 0;
	
	IC_SPI_ReadBlock(addr, 6, 100);
	for(int i = 0; i < 5; i++)
	{
		add += addr[i];
	}
	if(add != addr[5]) return;
	nRF_setAddr(addr);
}

#define C_Com_SPI_File_PageSize		256
static u32 R_Com_SPI_File_BaseAddr = FLASH_GAME_ADDR;
static u32 R_Com_SPI_File_Size = 0;

/*File Write*/
void Com_SPI_FileWrite(void)
{
	u32 addr;
	u8 buffer[C_Com_SPI_File_PageSize];
	u8 endByte;
	u8 resByte;
	BOOL exp_Spi;
	
	resByte = C_Com_File_RW_EndByte;
	exp_Spi = IC_SPI_ReadBlock((u8 *)&addr, 4, 2);
	if(exp_Spi == TRUE)
	{
		exp_Spi = IC_SPI_ReadBlock(buffer, C_Com_SPI_File_PageSize, 2);
	}
	if(exp_Spi == TRUE)
	{
		endByte = C_IC_Spi_None;
		exp_Spi = IC_SPI_ReadBlock(&endByte, 1, 2);
	}
	
	if(exp_Spi == TRUE)
	{
		if(endByte != C_Com_File_RW_EndByte)
		{
			resByte = C_Com_File_RW_ErrMsg;
		}
		else if(addr >= R_Com_SPI_File_Size)
		{
			resByte = C_Com_File_RW_ErrAddr;
		}
		else
		{
			resByte = C_Com_File_RW_EndByte;
		}
	}
	else
	{
		resByte = C_Com_File_RW_ErrPkg;
	}
	IC_SPI_WriteByte(Enum_Com_Cmd_FWrite);
	IC_SPI_WriteByte(resByte);
	IC_SPI_WriteByte(resByte);
	IC_SPI_RxBuffer_Reset();	
	
	if(resByte == C_Com_File_RW_EndByte)
	{
		addr += R_Com_SPI_File_BaseAddr;
		Flash_Write_Page(addr, buffer, C_Com_SPI_File_PageSize);
//		if((addr & 0xffff) == 0xff00)
//		{
//			Flash_64K_Erase(addr + 0x100);
//		}
		if((addr & 0xfff) == 0xf00)
		{
			Flash_4K_Erase(addr + 0x100);
		}
	}
}

/*File Read*/
void Com_SPI_FileRead(void)
{
	u32 addr;
	u8 buffer[C_Com_SPI_File_PageSize];
	
	IC_SPI_ReadBlock((u8 *)&addr, 4, 100);
	
	addr += R_Com_SPI_File_BaseAddr;
	IC_SPI_RxBuffer_Reset();	
	if(addr >= R_Com_SPI_File_Size)
	{
		IC_SPI_WriteByte(C_Com_File_RW_ErrAddr);
	}
	else
	{
		Flash_Read(addr, buffer, C_Com_SPI_File_PageSize);
		IC_SPI_WriteByte(Enum_Com_Cmd_FRead);
		IC_SPI_WriteBlock(buffer, C_Com_SPI_File_PageSize);
	}
	
}

/*File Open*/
void Com_SPI_FileOpen(void)
{
	u32 size;
	u8 mode;
	u8 area;
	u32 addr;
	BOOL exp;
	
	exp = IC_SPI_ReadBlock(&mode, 1, 10);
	if(exp != TRUE) return;
	exp = IC_SPI_ReadBlock(&area, 1, 10);
	if(exp != TRUE) return;
	exp = IC_SPI_ReadBlock((u8 *)&size, 4, 10);
	if(exp != TRUE) return;
	IC_SPI_WriteByte(Enum_Com_Cmd_FOpen);
	
	
	if(area == C_Com_File_Area_InData)
	{
		if(size > FLASH_SYSTEM_SIZE)
		{
			size = FLASH_SYSTEM_SIZE;
			return;
		}
		addr = FLASH_SYSTEM_ADDR;
	}
	else if(area == C_Com_File_Area_ExtData)
	{
		if(size > FLASH_GAME_SIZE)
		{
			size = FLASH_GAME_SIZE;
		}
		addr = FLASH_GAME_ADDR;
	}
	else
	{
		return;
	}
	R_Com_SPI_File_Size = size;
	R_Com_SPI_File_BaseAddr = addr;

	IC_SPI_RxBuffer_Reset();
	//Flash_64K_Erase(R_Com_SPI_File_BaseAddr);
	Flash_4K_Erase(R_Com_SPI_File_BaseAddr);

//	if((mode & C_Com_File_rwMode_Write))
//	{
//		u32 dt, t;
//		u8 msg;
//		
//		t = CMOS_GetTimer();
//		do{
//			if(IC_SPI_ReadByte(&msg) == TRUE)
//			{
//				if(msg == Enum_Com_Cmd_FWrite)
//				{
//					Com_SPI_FileWrite();
//					t = CMOS_GetTimer();
//				}
//				else if(msg == Enum_Com_Cmd_FClose)
//				{
//					break;
//				}
//			}
//			dt = CMOS_GetTimerCount(t);
//			ClrAutoPowerOff();
//			IWDG_ReloadCounter();
//		}while(dt < 15000);
//		IC_SPI_RxBuffer_Reset();
//	}
}

extern void File_SetBaseAddr(u32 addr,u32 gameaddr);
/*File Close*/
void Com_SPI_FileClose(void)
{
	File_SetBaseAddr(FLASH_SYSTEM_ADDR, FLASH_GAME_ADDR);
	IC_SPI_RxBuffer_Reset();	
	return ;
}


/*Get Slaver ID*/
void Com_SPI_GetSID(void)
{
	u32 sid;
	
	sid = readSlaverID();
	IC_SPI_WriteByte(Enum_Com_Cmd_GetSID);
	IC_SPI_WriteBlock((u8 *)&sid, 4);
}

/*Set Slaver ID*/
void Com_SPI_SetSID(void)
{
	u32 sid;
	u8 flg;
	
	IC_SPI_ReadBlock((u8 *)&sid, 4, 100);
	IC_SPI_ReadBlock((u8 *)&flg, 1, 100);
	if(sid > 4 || flg != 0xaa) return;
	WriteSlaverID(sid);
}

/*Put Graph*/
void Com_SPI_PutGraph(void)
{
	u16 id;
	s16 x, y;
	u8 layer;
	BOOL exp;
	
	exp = IC_SPI_ReadBlock((u8 *)&id, 2, 2);
	if(exp == TRUE)
	{
		exp = IC_SPI_ReadBlock((u8 *)&x, 2, 2);
	}
	if(exp == TRUE)
	{
		exp = IC_SPI_ReadBlock((u8 *)&y, 2, 2);
	}
	if(exp == TRUE)
	{
		exp = IC_SPI_ReadBlock((u8 *)&layer, 1, 2);
	}
	if(exp == TRUE)
	{
		AddGraph(id, x, y, layer);
		RefreshGraphs();
	}
}

/*Clear Layer*/
void Com_SPI_ClearLayer(void)
{
	BOOL exp;
	u8 layer;
	
	exp = IC_SPI_ReadBlock((u8 *)&layer, 1, 2);
	if(exp == TRUE)
	{
		ClearGraphLayer(layer);
		RefreshGraphs();
	}
}

/*Play HDR*/
void Com_SPI_PlayHDR(void)
{
	Animation_Arg hdr;
	
	BOOL exp;
	
	exp = IC_SPI_ReadBlock((u8 *)&(hdr.animationID), 2, 2);
	if(exp == TRUE)
	{
		exp = IC_SPI_ReadBlock((u8 *)&(hdr.x), 2, 2);
	}
	if(exp == TRUE)
	{
		exp = IC_SPI_ReadBlock((u8 *)&(hdr.y), 2, 2);
	}
	if(exp == TRUE)
	{
		exp = IC_SPI_ReadBlock((u8 *)&(hdr.layer), 1, 2);
	}
	if(exp == TRUE)
	{
		GEA_AddAnimation(&hdr);
		RefreshGraphs();
	}
}

/*Get Firmware ID*/
void Com_SPI_GetFirmwareID(void)
{
	u8 fmid[6]={"1.06"};
	
	IC_SPI_WriteByte(Enum_Com_Cmd_GetFirmwareID);
	for(u8 i = 0; i < 6; i++)
	{
		IC_SPI_WriteByte(fmid[i]);
	}
	IC_SPI_RxBuffer_Reset();	
}

/*test*/
u8 * Getdisplaybuf(void);
void Com_SPI_Test(void)
{
	int i;
	u8 *p=Getdisplaybuf();
	ClearScreen();
	for(i=0;i<16;i++)
	{
		IC_SPI_ReadBlock(p, 2048, 100);
		p+=2048;
		IWDG_ReloadCounter();
	}
	RefreshScreen();
}

typedef void (*cmdFunc)(void);
typedef struct
{
	Enum_SPI_Cmd_List id;
	cmdFunc function;
}Struct_Com_Cmd, *pStruct_Com_Cmd;


const Struct_Com_Cmd TB_Com_Cmd_List[Enum_Com_Cmd_Max] = {
	{
		.id = Enum_Com_Cmd_SysUpdate,
		.function = Com_SPI_SystemUpdate,
	},
	{
		.id = Enum_Com_Cmd_GetnRFID,
		.function = Com_SPI_GetnRFID,
	},
	{
		.id = Enum_Com_Cmd_SetnRFID,
		.function = Com_SPI_SetnRFID,
	},
	{
		.id = Enum_Com_Cmd_FOpen,
		.function = Com_SPI_FileOpen,
	},
	{
		.id = Enum_Com_Cmd_FWrite,
		.function = Com_SPI_FileWrite,
	},
	{
		.id = Enum_Com_Cmd_FRead,
		.function = Com_SPI_FileRead,
	},
	{
		.id = Enum_Com_Cmd_FClose,
		.function = Com_SPI_FileClose,
	},
	{
		.id = Enum_Com_Cmd_GetSID,
		.function = Com_SPI_GetSID,
	},
	{
		.id = Enum_Com_Cmd_SetSID,
		.function = Com_SPI_SetSID,
	},
	{
		.id = Enum_Com_Cmd_PutGraph,
		.function = Com_SPI_PutGraph,
	},
	{
		.id = Enum_Com_Cmd_ClearLayer,
		.function = Com_SPI_ClearLayer,
	},
	{
		.id = Enum_Com_Cmd_PlayHDR,
		.function = Com_SPI_PlayHDR,
	},
	{
		.id = Enum_Com_Cmd_GetFirmwareID,
		.function = Com_SPI_GetFirmwareID,
	},
	{
		.id = Enum_Com_Cmd_Test,
		.function = Com_SPI_Test,
	},	
};

/*Check SPI Message*/
void Com_SPI_CheckMessage(void)
{
	u8 msg;
	cmdFunc func;
	
	while(IC_SPI_ReadByte(&msg) == TRUE)
	{
		if(msg == C_IC_Spi_None)
		{
			continue;
		}
			
		if(TB_Com_Cmd_List[msg % Enum_Com_Cmd_Max].id == msg && msg < Enum_Com_Cmd_Max)
		{
			func = TB_Com_Cmd_List[msg].function;
			(*func)();
		}
		else
		{
			u8 i = 0; 

			while(TB_Com_Cmd_List[i].id != msg && i < Enum_Com_Cmd_Max)
			{
				i++;
			}
			if(i < Enum_Com_Cmd_Max)
			{
				func = TB_Com_Cmd_List[i].function;
				(*func)();
			}
		}
		ClrAutoPowerOff();
	}
}

#endif /*_COMMUNICATE_SLAVER_*/



/********************************* master function ************************************/
#ifdef _COMMUNICATE_MASTER_

/*System Update*/
void Com_SPI_SystemUpdate(u8 slaverID)
{
	u8 cmdStr[] = "ACK\0";
	
	for(int i = 0; i < 4; i++)
	{
		IC_SPI_RW(slaverID, cmdStr[i]);
		uDelay(10);
	}
}

/*Write Bytes*/
static void Com_SPI_WriteBytes(u8 slaverID, u8 *buf, u16 length)
{
	for(u16 i = 0; i < length; i++)
	{
		IC_SPI_RW(slaverID, buf[i]);
		uDelay(1);
	}
}

/*Read Bytes*/
static void Com_SPI_ReadBytes(u8 slaverID, u8 *buf, u16 length)
{
	for(u16 i = 0; i < length; i++)
	{
		buf[i] = IC_SPI_RW(slaverID, C_IC_Spi_None);
		uDelay(10);
	}
}


/*Get Slaver Respond*/
static BOOL Com_SPI_GetRespond(u8 slaverID, u8 msg, u8 *res, u32 udt, u32 retryT)
{
	u8 msgR;
	
	if(retryT < 3)
	{
		retryT = 3;
	}
	uDelay(50);
	msgR = IC_SPI_RW(slaverID, C_IC_Spi_None);
	while(msgR != msg && retryT != 0)
	{
		uDelay(udt);
		msgR = IC_SPI_RW(slaverID, C_IC_Spi_None);
		if(msgR != C_IC_Spi_None && res != 0)
		{
			*res = msgR;
		}
		retryT--;
	}
	
	return (retryT == 0? FALSE: TRUE);
}



/*Get nRF ID*/
BOOL Com_SPI_GetnRFID(u8 slaverID, u8 *buf)
{
	IC_SPI_RW(slaverID, Enum_Com_Cmd_GetnRFID);
	if(Com_SPI_GetRespond(slaverID, Enum_Com_Cmd_GetnRFID, 0, 100, 1000) != TRUE)
		return FALSE;
	Com_SPI_ReadBytes(slaverID, buf, 5);
	
	return TRUE;
}

/*Set nRF ID*/
void Com_SPI_SetnRFID(u8 slaverID, u8 *buf)
{
	u8 cs = 0;
	
	IC_SPI_RW(slaverID, Enum_Com_Cmd_SetnRFID);
	for(int i = 0; i < 5; i++)
	{
		cs += buf[i];
	}
	Com_SPI_WriteBytes(slaverID, buf, 5);
	IC_SPI_RW(slaverID, cs);
}




/*File Open*/
BOOL Com_SPI_FileOpen(u8 slaverID, Struct_Com_File *file)
{
	file->ptrPageRead = 0;
	file->ptrPageWrite = 0;

	IC_SPI_RW(slaverID, Enum_Com_Cmd_FOpen);
	IC_SPI_RW(slaverID, file->rwMode);
	IC_SPI_RW(slaverID, file->flashArea);
	Com_SPI_WriteBytes(slaverID, (u8 *)&(file->size), 4);

	return Com_SPI_GetRespond(slaverID, Enum_Com_Cmd_FOpen, 0, 700, 600);
}

/*File Write*/
BOOL Com_SPI_FileWrite(u8 slaverID, Struct_Com_File *file, u8 *buffer, u8 *res)
{	
	if((file->rwMode & C_Com_File_rwMode_Write) == 0)
	{
		*res = C_Com_File_RW_ErrMode;
		return FALSE;
	}
	
	IC_SPI_RW(slaverID, Enum_Com_Cmd_FWrite);
	Com_SPI_WriteBytes(slaverID, (u8 *)&(file->ptrPageWrite), 4);
	Com_SPI_WriteBytes(slaverID, buffer, C_Com_SPI_File_PageSize);
	IC_SPI_RW(slaverID, C_Com_File_RW_EndByte);
	file->ptrPageWrite += C_Com_SPI_File_PageSize;
	//if((file->ptrPageWrite & 0xffff) == 0) uDelay(10000);
	*res = C_Com_File_RW_ErrConnect;
	if(Com_SPI_GetRespond(slaverID, Enum_Com_Cmd_FWrite, res, 400, 600) != TRUE)
		return FALSE;
	if(Com_SPI_GetRespond(slaverID, C_Com_File_RW_EndByte, res, 5, 10) != TRUE)
		return FALSE;
	
	return TRUE;
}

/*File Read*/
BOOL Com_SPI_FileRead(u8 slaverID, Struct_Com_File *file, u8 *buffer, u8 *res)
{
	IC_SPI_RW(slaverID, Enum_Com_Cmd_FRead);
	Com_SPI_WriteBytes(slaverID, (u8 *)&(file->ptrPageRead), 4);
	
	if(Com_SPI_GetRespond(slaverID, Enum_Com_Cmd_FRead, res, 100, 100) != TRUE)
		return FALSE;
	
	Com_SPI_ReadBytes(slaverID, buffer, C_Com_SPI_File_PageSize);
	file->ptrPageRead += C_Com_SPI_File_PageSize;
	
	return TRUE;
}

/*File Close*/
BOOL Com_SPI_FileClose(u8 slaverID, Struct_Com_File *file)
{
	file->ptrPageRead = 0;
	file->ptrPageWrite = 0;

	IC_SPI_RW(slaverID, Enum_Com_Cmd_FClose);
	
	return TRUE;
}

/*Get Slaver ID*/
BOOL Com_SPI_GetSID(u8 slaverID, u32 *sid)
{
	IC_SPI_RW(slaverID, Enum_Com_Cmd_GetSID);
	
	if(Com_SPI_GetRespond(slaverID, Enum_Com_Cmd_GetSID, 0, 1000, 1000) != TRUE)
		return FALSE;
	Com_SPI_ReadBytes(slaverID, (u8 *)sid, 4);
	
	return TRUE;
}

/*Set Slaver ID*/
void Com_SPI_SetSID(u8 slaverID, u32 sid)
{
	IC_SPI_RW(slaverID, Enum_Com_Cmd_SetSID);
	Com_SPI_WriteBytes(slaverID, (u8 *)&(sid), 4);
	IC_SPI_RW(slaverID, 0xaa);
}


/*Put Graph*/
void Com_SPI_PutGraph(u8 slaverID, u16 graphID, s16 x, s16 y, u8 layer)
{
	IC_SPI_RW(slaverID, Enum_Com_Cmd_PutGraph);
	Com_SPI_WriteBytes(slaverID, (u8 *)&graphID, 2);
	Com_SPI_WriteBytes(slaverID, (u8 *)&x, 2);
	Com_SPI_WriteBytes(slaverID, (u8 *)&y, 2);
	IC_SPI_RW(slaverID, layer);
}

/*Clear Layer*/
void Com_SPI_ClearLayer(u8 slaverID, u8 layer)
{
	IC_SPI_RW(slaverID, Enum_Com_Cmd_ClearLayer);
	IC_SPI_RW(slaverID, layer);
}

/*Play HDR*/
void Com_SPI_PlayHDR(u8 slaverID, u16 HDRID, s16 x, s16 y, u8 layer)
{
	IC_SPI_RW(slaverID, Enum_Com_Cmd_PlayHDR);
	Com_SPI_WriteBytes(slaverID, (u8 *)&HDRID, 2);
	Com_SPI_WriteBytes(slaverID, (u8 *)&x, 2);
	Com_SPI_WriteBytes(slaverID, (u8 *)&y, 2);
	IC_SPI_RW(slaverID, layer);
}

/*Fill Display Buffer*/
void Com_SPI_FillDspBuf(u8 slaverID, u8 rowIndex, u8 *buffer)
{
}

/*Refresh Screen*/
void Com_SPI_ScrRefresh(u8 slaverID)
{
}

/*Play Animation*/
void Com_SPI_PlayAnimation(u8 slaverID, u16 anmID)
{
}

/*Play Voice*/
void Com_SPI_PlayVoice(u8 slaverID, u16 voiceID)
{
}

/*Get Firmware ID*/
BOOL Com_SPI_GetFirmwareID(u8 slaverID, u8 *FMID)
{
	IC_SPI_RW(slaverID, Enum_Com_Cmd_GetFirmwareID);
	if(Com_SPI_GetRespond(slaverID, Enum_Com_Cmd_GetFirmwareID, 0, 100, 1000) != TRUE)
		return FALSE;
	Com_SPI_ReadBytes(slaverID, FMID, 6);
	
	return TRUE;
}


/*Check SPI Message*/
void Com_SPI_CheckMessage(void)
{
	u8 msg;
	
	if(IC_SPI_ReadByte(&msg) == TRUE)
	{
		if(msg == Enum_Com_Cmd_SysUpdate)
		{
			//reboot system
		}
	}
}

#endif /*_COMMUNICATE_MASTER_*/




