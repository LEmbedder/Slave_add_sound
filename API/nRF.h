/*
 * Filename:	nRF.h
 * Author:		ZouChao, C.M.16
 * Date:		2015-06-17
 * Version:		v0.1
 * Discription:	nRF api
 */

#ifndef __NRF_H_
#define __NRF_H_

#include "stm32f10x.h"
#include "CMLIB.h"
#include "NRF24L01.h"

typedef struct
{
	u8 para[C_nRF_CtrlCmdSize];
}STRUCT_nRF_CMD, *PSTRUCT_nRF_CMD;


				 
/*nRF Initialization*/
void nRF_Initialization(void);

/*Get nRF Message*/
BOOL nRF_GetMessage(u8 *buf);

/*Set nRF Address*/
void nRF_SetAddress(u8 *pAddr);

#endif /*__NRF_H_*/



