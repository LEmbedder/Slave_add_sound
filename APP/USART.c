/*
 * Filename:	USART.c
 * Author:		C.M.16
 * Date:		2015-07-21
 * Version:		v0.2
 * Discription:	USART driver
 */

#include "USART.h"
#include "CMLib.h"
#include "CMOS.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

typedef struct __FILE FILE;
#define C_RxFIFOSize	50
static STRUCT_FIFO_REG R_RxFIFO;
static u8 R_RxArray[C_RxFIFOSize];

/*HardWare Config*/
static void RS_ConfigHW(u32 baudrate);
/*Register Config*/
static void RS_ConfigReg(void);


/****************************************************************************/
/*Usart Initialization*/
void RS_Init(u32 baudrate)
{
	
	RS_ConfigHW(baudrate);
	RS_ConfigReg();
}

/*HardWare Config*/
static void RS_ConfigHW(u32 baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* config USART3 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
	/* USART3 GPIO config */
	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);    

	/* Configure USART3 Rx (PB.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* USART3 mode config */
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	 
	USART3_Priority();
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART3, ENABLE);

}

/*Register Config*/
static void RS_ConfigReg(void)
{
	FIFO_Init(&R_RxFIFO, C_RxFIFOSize);
}

/*Recieve Data*/
void RS_RecieveData(void)
{
	u8 ch;
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART3);
	}

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		ch = USART_ReceiveData(USART3); 
		FIFO_WriteData(R_RxArray, &R_RxFIFO, ch);	
	} 
}

/*Get Message from FIFO Buffer*/
BOOL RS_GetMessage(u8 *ch)
{
	return FIFO_ReadData(R_RxArray, &R_RxFIFO, ch);
}

/*Send Message*/
void RS_SendMessage(u8 byte)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_SendData(USART3, byte);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}

/*Send String*/
void RS_SendString(const u8 *str)
{
	while(*str)
	{
		RS_SendMessage(*str);
		str++;
	}
}

/*Send Block*/
void RS_SendBlock(u8 *buf, u16 size)
{
	u16 i;

	for(i = 0; i < size; i++)
	{
		RS_SendMessage(buf[i]);
	}
}

int fputc(int ch, FILE *f)
{
	while (!(USART3->SR & USART_FLAG_TXE));
	USART_SendData(USART3, (unsigned char) ch);
	return (ch);
}

