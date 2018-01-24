/*
 * Filename:	IAP.h
 * Author:		C.M.16
 * Date:		2015-09-21
 * Version:		v0.1
 * Discription:	IAP driver
 */

#include "stm32f10x_crc.h"
#include "CMOS.h"
//#include "CMLIB.h"
#include "IAP.h"
#include "Flash.h"

#ifdef BOOTLOADER_SLAVER
#include "NorFlash.h"
#include "FLASHROM.h"
#endif




typedef void (*App_Main)(void);

//ÉèÖÃÕ»¶¥µØÖ·
//addr:Õ»¶¥µØÖ·
__asm void MSR_MSP(u32 addr) 
{
   
//	CPSID I					//close inttrupt
//	BX LR 
	
	MSR MSP, r0 			//set Main Stack value
    BX r14
}

/*Run Iap App*/
void Iap_Jump2Run(u32 addr)
{
	App_Main appM;
	
	if(((*(vu32*)addr)&0x2FFE0000)==0x20000000)
	{
		appM = (App_Main)*(vu32 *)(addr+4);
		__disable_irq();
		MSR_MSP(*(vu32 *)addr);	
		appM();
	}
}


#define C_Iap_Offset_MasterProg		32
#define C_Iap_Offset_SlaverProg		8
#define C_Iap_Offset_SlaverData		0

/*File Initialization*/
void Iap_FileInitialization(void)
{
#ifdef BOOTLOADER_MASTER
	
#elif BOOTLOADER_SLAVER
	Flash_Init();
#endif
}

/*Open File*/
u8 Iap_OpenFile(Iap_File *file)
{
	u8 result = 0;
	
#ifdef BOOTLOADER_MASTER
	if(file->type == updateMP)
	{
		file->cnt = 0;
	}
	else
	{
		result = (u8)-1;
	}
	
#elif BOOTLOADER_SLAVER
	if(file->type == updateSP)
	{
		file->cnt = 0;
	}
	else if(file->type == updateSD)
	{
		file->cnt = 0;
		if(file->size <= FLASH_SYSTEM_SIZE)
		{
			u32 norFlashAddr;
			u32 offset;
			
			norFlashAddr = FLASH_SYSTEM_ADDR;
			offset = 0;
			while(offset < file->size)
			{
//				Flash_4K_Erase(norFlashAddr + offset);
//				offset += 0x1000;
				Flash_64K_Erase(norFlashAddr + offset);
				offset += 0x10000;
			}
		}
		else
		{
			result = (u8)-1;
		}
	}
	else
	{
		result = (u8)-1;
	}

#endif
	
	return result;
}

/*Update File*/
u8 Iap_UpdateFile(Iap_File *file, u8 *buffer)
{
	u8 result = 0;
	
#ifdef BOOTLOADER_MASTER
	if(file->type == updateMP && file->cnt * C_Flash_PageSize < file->size)
	{
		Flash_WritePage((u32 *)buffer, C_Iap_Offset_MasterProg + file->cnt);
		file->cnt++;
	}
	else
	{
		result = (u8)-1;
	}
	
#elif BOOTLOADER_SLAVER
	if(file->type == updateSP && file->cnt * C_Flash_PageSize < file->size)
	{
		Flash_WritePage((u32 *)buffer, C_Iap_Offset_SlaverProg + file->cnt);
		file->cnt++;
	}
	else if(file->type == updateSD && file->cnt * 256 < file->size)
	{
		u8 i;
		u32 offset;
		
		offset = file->cnt * 256;
		for(i = 0; i < 8; i++)	//2048 = 256 * 8
		{
			if(offset < file->size)
			{
				Flash_Write_Page(FLASH_SYSTEM_ADDR + offset, &buffer[i * 256], 256);
				file->cnt++;
				offset += 256;
			}
		}
	}
	else
	{
		result = (u8)-1;
	}
	
#endif
	
	return result;
}

/*Close File*/
u8 Iap_CloseFile(Iap_File *file)
{
	u8 result = 0;
	
#ifdef BOOTLOADER_MASTER
	if(file->type == updateMP)
	{
		u32 *addr;
		u32 length;
		
		addr = (u32 *)(C_Flash_BaseAddr + C_Flash_PageSize * C_Iap_Offset_MasterProg);
		length = (file->size + 3) >> 2;
		CRC_ResetDR();
		file->crc = CRC_CalcBlockCRC(addr, length);
	}
	else
	{
		result = (u8)-1;
	}
	
#elif BOOTLOADER_SLAVER
	if(file->type == updateSP)
	{
		u32 *addr;
		u32 length;
		
		addr = (u32 *)(C_Flash_BaseAddr + C_Flash_PageSize * C_Iap_Offset_SlaverProg);
		length = (file->size + 3) >> 2;
		CRC_ResetDR();
		file->crc = CRC_CalcBlockCRC(addr, length);
	}
	else if(file->type == updateSD)
	{
		file->crc = 0;		//no crc check
	}
	else
	{
		result = (u8)-1;
	}
	
#endif
	return result;
}






/*Run App*/
void Iap_RunApp(void)
{
#ifdef BOOTLOADER_MASTER
	u32 addr;
	addr = C_Flash_BaseAddr + C_Flash_PageSize * C_Iap_Offset_MasterProg;
	Iap_Jump2Run(addr);
#endif	
#ifdef BOOTLOADER_SLAVER
	u32 addr;
	addr = C_Flash_BaseAddr + C_Flash_PageSize * C_Iap_Offset_SlaverProg;
	Iap_Jump2Run(addr);	
#endif	
}

/*Run BootLoader*/
void Iap_RunBootLoader(void)
{
	u32 addr;

#ifdef MS_SLAVE
	addr = 0x08000000L;
#else
	addr = C_Flash_BaseAddr;
#endif
	Iap_Jump2Run(addr);
}





