/*
 * Filename:	USART.h
 * Author:		C.M.16
 * Date:		2015-07-21
 * Version:		v0.2
 * Discription:	USART driver
 */


#ifndef __USART_H_
#define __USART_H_

#include "stm32f10x.h"		
#include "CMLib.h"

/*Usart Initialization*/
void RS_Init(u32 baudrate);
/*Recieve Data (Only use for intterupt function)*/
void RS_RecieveData(void);
/*Get Message from FIFO Buffer*/
BOOL RS_GetMessage(u8 *ch);
/*Send Message*/
void RS_SendMessage(u8 byte);
/*Send String*/
void RS_SendString(const u8 *str);
/*Send Block*/
void RS_SendBlock(u8 *buf, u16 size);


#endif	/*__USART_H_*/
