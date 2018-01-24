/*
 * Filename:	IAP.h
 * Author:		C.M.16
 * Date:		2015-09-21
 * Version:		v0.1
 * Discription:	IAP driver
 */


#ifndef __IAP_H_
#define __IAP_H_

#include "stm32f10x.h"		
#include "CMLib.h"

typedef enum
{
	updateSP = 0,
	updateSD,
	updateMP	
}Iap_UpdateType;

typedef struct
{
	u8 type;
	u16 cnt;
	u32 size;
	u32 crc;
}Iap_File, *pIap_File;

/*File Initialization*/
void Iap_FileInitialization(void);

/*Open File*/
u8 Iap_OpenFile(Iap_File *file);
/*Update File*/
u8 Iap_UpdateFile(Iap_File *file, u8 *buffer);
/*Close File*/
u8 Iap_CloseFile(Iap_File *file);

/*Run App*/
void Iap_RunApp(void);
/*Run BootLoader*/
void Iap_RunBootLoader(void);

#endif	/*__IAP_H_*/
