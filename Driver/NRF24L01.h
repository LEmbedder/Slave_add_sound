/*
 * Filename:	NRF24L01.h
 * Author:		ZouChao, C.M.16
 * Date:		2015-05-22
 * Version:		v0.1
 * Discription:	2.4G driver
 */

#ifndef __NRF24L01_H_
#define __NRF24L01_H_

#include "stm32f10x.h"
#include "stm32f10x_exti.h"

#define C_nRF_StatusRx     0x40		// STA_MARK_RX
#define C_nRF_StatusTx     0x20		// STA_MARK_TX
#define C_nRF_StatusMx     0x10		// STA_MARK_MX

#define CSN_SET      GPIOB->BSRR = GPIO_Pin_12
#define CLK_SET      GPIOB->BSRR = GPIO_Pin_13
#define MISO_SET     GPIOB->BSRR = GPIO_Pin_14
#define MOSI_SET     GPIOB->BSRR = GPIO_Pin_15
#define IRQ_SET      GPIOC->BSRR = GPIO_Pin_6
#define CE_SET       GPIOA->BSRR = GPIO_Pin_3

#define CSN_CLR      GPIOB->BRR = GPIO_Pin_12
#define CLK_CLR      GPIOB->BRR = GPIO_Pin_13
#define MISO_CLR     GPIOB->BRR = GPIO_Pin_14
#define MOSI_CLR     GPIOB->BRR = GPIO_pin_15
#define IRQ_CLR      GPIOC->BRR = GPIO_Pin_6
#define CE_CLR       GPIOA->BRR = GPIO_Pin_3

#define C_nRF_CtrlCmdSize		32
#define C_nRF_DataStreamSize	32
#define C_nRF_Fifo_Size					15
#define C_nRF_Addr_Width		5

/*NRF Initialization*/
void nRF_Init(void);
/*Poweroff*/
void nRF_PowerDown(void);
/*Standby mode*/
void nRF_StandbyMode(void);

///*Rx Setting*/
//void nRF_RxSetReg(u8 *addr, u32 ploadsize);
///*Rx Mode*/
//void nRF_RxMode(void);
/*Get Rx Data*/
void nRF_RxGetData(u8 *rxbuf, u8 size);

/*Tx mode*/
void nRF_TxSetTXRP0(u8 *addr);
/*TX DATA*/
void nRF_TxSendData(u8 *txbuf, u8 size);
///*Tx Mode*/
//void nRF_TxMode(void);

/*Get IRQ Status*/
u8 nRF_GetIRQStatus(void);
/*Get Status Register*/
u8 nRF_GetStatusRegister(void);
/*Clear Status Register*/
void nRF_ClearStatusRegister(void);

/*Flush Tx Buffer*/
void nRF_FlushTxBuffer(void);
/*Flush Rx Buffer*/
void nRF_FlushRxBuffer(void);


/*Get Reg Value*/
void nRF_GetRegValue(u8 *buf);


/* 0 -- success,  -1 -- failed */
int nRF_Check(void);

/*Write TX Buffer*/
void nRF_WriteTxBuffer(u8 *pbuf, u8 length);
/*Read RX Buffer*/
void nRF_ReadRxBuffer(u8 *pbuf, u8 length);
u8 nRF_RegisterWriteByte(u8 reg, u8 cmd);
void nRF_EXTIcfg(void);

void SendMessage(u8 *buf, u8 size);

void nRF_InitReg(void);

/*Set Recieve Mode*/
void nRF_RecieveMode(void);

void nRF_TxSetRP1P2(u8 *addr);

void ResetRecieveMode(void);

u8 get_NRF_RSSI(void);
#endif /*__NRF24L01_H_*/
