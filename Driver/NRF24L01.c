/*
 * Filename:	NRF24L01.c
 * Author:		ZouChao, C.M.16
 * Date:		2015-05-22
 * Version:		v0.1
 * Discription:	2.4G driver
 */
#include "CMOS.h"
#include "NRF24L01.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "misc.h"
#include "CMLIB.h"

#define NRF_READ_REG		0x00  // Define read command to register
#define NRF_WRITE_REG		0x20  // Define write command to register
#define RD_RX_PLOAD			0x61  // Define RX payload register address
#define WR_TX_PLOAD			0xA0  // Define TX payload register address
#define FLUSH_TX			0xE1  // Define flush TX register command
#define FLUSH_RX			0xE2  // Define flush RX register command
#define REUSE_TX_PL     	0xE3  // Define reuse TX payload register command
#define NOP             	0xFF  // Define No Operation, might be used to read status register

#define R_RX_PL_WID			0x60
#define W_ACK_PAYLOAD		0xA8
#define W_TX_PAYLOAD_NOACK	0xB0

//***************************************************//
// SPI(nRF24L01) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address


#define Pin_CSN      GPIO_Pin_12      // GPIOB
#define Pin_CLK      GPIO_Pin_13       // GPIOB
#define Pin_MISO     GPIO_Pin_14       // GPIOB
#define Pin_MOSI     GPIO_Pin_15       // GPIOB
#define Pin_IRQ      GPIO_Pin_6       // GPIOC
#define Pin_CE       GPIO_Pin_3       // GPIOA




/*SPI Start*/
static void nRF_SPIStart(void);
/*SPI READ WRITE*/
static u8 nRF_SPIRW(u8 cmd);
/*SPI Stop*/
static void nRF_SPIStop(void);

/*Write one Byte to Register*/
u8 nRF_RegisterWriteByte(u8 reg, u8 cmd);
/*Write Commands to Register*/
static u8 nRF_RegisterWriteBuffer(u8 reg, u8 *cmdbuf, u8 length);
/*Read one Byte to Register*/
static u8 nRF_RegisterReadByte(u8 reg);
/*Read cmds to Register*/
static u8 nRF_RegisterReadBuffer(u8 reg, u8 *cmdbuf, u8 length);


//#define TX_ADR_WIDTH    5   // 5  bytes TX(RX) address width
//#define TX_PLOAD_WIDTH  32  // 32 bytes TX payload
//const u8 TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x43,0x10,0x10,0x01}; // Define a static TX address


/**************************************** funtion **************************************************/

void SPI_NRF_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	// CSN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// CLK, MISO, MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
												   
	// IRQ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOD,&GPIO_InitStructure);
	// CE
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_Cmd(SPI2, ENABLE);
	
	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
}
void nRF_EXTIcfg(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_ClearITPendingBit(EXTI_Line6);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling;    //上升沿下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NFR_Priority();
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 
}


/*NRF Initialization*/
void nRF_Init(void)
{
	SPI_NRF_Init();
	CE_CLR;
	CSN_SET;
	IRQ_SET;
	nRF_PowerDown();
	nRF_StandbyMode();
	nRF_GetStatusRegister();
	nRF_ClearStatusRegister();
}

/*Poweroff*/
void nRF_PowerDown(void)
{
	CE_CLR;
	nRF_RegisterWriteByte(CONFIG, 0x0D); 
	mDelay(2);
	nRF_RegisterWriteByte(CONFIG, 0x0F); 
	mDelay(5);	// 4.5ms
}


/*Standby mode*/
void nRF_StandbyMode(void)
{
	CE_CLR;
}

u8 R_nRF_Addr[5] = {0x16, 0x35, 0x00, 0x36, 0x03};
u8 R_EN_AA=0x01;
u8 R_EN_RXADDR=0x01;
void nRF_InitReg(void)
{
	CE_CLR;
	nRF_RegisterWriteBuffer(TX_ADDR, R_nRF_Addr, C_nRF_Addr_Width);    // Writes TX_Address to nRF24L01
  nRF_RegisterWriteBuffer(RX_ADDR_P0, R_nRF_Addr, C_nRF_Addr_Width); // RX_Addr0 same as TX_Adr for Auto.Ack
	nRF_RegisterWriteByte(EN_AA, R_EN_AA);      // Enable Auto.Ack:Pipe0
	nRF_RegisterWriteByte(EN_RXADDR, R_EN_RXADDR);  // Enable Pipe0
	nRF_RegisterWriteByte(SETUP_RETR, 0x2a);	//0x21); // 500us + 86us, 10 retrans...
	nRF_RegisterWriteByte(RF_CH, 40);        // Select RF channel 40
	nRF_RegisterWriteByte(RX_PW_P0, C_nRF_DataStreamSize); // Select same RX payload width as TX Payload width
  nRF_RegisterWriteByte(RF_SETUP, 0x0f);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
}

/*Set Recieve Mode*/
void nRF_RecieveMode()
{
	CE_CLR;
	nRF_FlushRxBuffer();
	nRF_RegisterWriteByte(CONFIG, 0x3f);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..
	CE_SET;
}

///*Rx Setting*/
//void nRF_RxSetReg(u8 *addr, u32 ploadsize)
//{
//	CE_CLR;
//	nRF_RegisterWriteBuffer(RX_ADDR_P0, addr, C_nRF_Addr_Width); // Use the same address on the RX device as the TX device
//	nRF_RegisterWriteByte(EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
//	nRF_RegisterWriteByte(EN_RXADDR, 0x01);  // Enable Pipe0
//	nRF_RegisterWriteByte(RF_CH, 40);        // Select RF channel 40
//	nRF_RegisterWriteByte(RX_PW_P0, ploadsize); // Select same RX payload width as TX Payload width
//	nRF_RegisterWriteByte(RF_SETUP, 0x0f);   // xxxx DataRate  TX_PWR TX_PWR Lna//TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
//	nRF_RegisterWriteByte(CONFIG, 0x3f);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..
//}

///*Rx Mode*/
//void nRF_RxMode(void)
//{
//	CE_SET;
//}

/*Get Rx Data*/
void nRF_RxGetData(u8 *rxbuf, u8 size)
{
	nRF_ReadRxBuffer(rxbuf, size);
}



/*Tx mode*/
void nRF_TxSetTXRP0(u8 *addr)
{
	CE_CLR;
	nRF_RegisterWriteBuffer(TX_ADDR, addr, C_nRF_Addr_Width);    // Writes TX_Address to nRF24L01
	nRF_RegisterWriteBuffer(RX_ADDR_P0, addr, C_nRF_Addr_Width); // RX_Addr0 same as TX_Adr for Auto.Ack
  nRF_RecieveMode();	//nRF_RegisterWriteByte(CONFIG, 0x0f);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled..
	
}

void nRF_TxSetRP1P2(u8 *addr)
{
	CE_CLR;
	nRF_RegisterWriteBuffer(RX_ADDR_P1, addr+4, C_nRF_Addr_Width);
//	nRF_RegisterWriteBuffer(RX_ADDR_P2, &p2, 1);
	nRF_RegisterWriteByte(RX_PW_P1, 32);
	R_EN_AA|=0x02;
	R_EN_RXADDR|=0x02;
	nRF_RegisterWriteByte(EN_AA, R_EN_AA);
	nRF_RegisterWriteByte(EN_RXADDR, R_EN_RXADDR);
  nRF_RecieveMode();	//	nRF_RegisterWriteByte(CONFIG, 0x0f);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled..
}

/*TX DATA*/
void nRF_TxSendData(u8 *txbuf, u8 size)
{
	CE_CLR;
	nRF_FlushTxBuffer();
	nRF_FlushRxBuffer();
  nRF_WriteTxBuffer(txbuf, size); // Writes data to TX payload
}

///*Tx Mode*/
//void nRF_TxMode(void)
//{
//	CE_SET;
//	uDelay(20);
//	CE_CLR;
//}

/*Get IRQ Status*/
u8 nRF_GetIRQStatus(void)
{
	return GPIO_ReadInputDataBit(GPIOC, Pin_IRQ);
}

/*Get Status Register*/
u8 nRF_GetStatusRegister(void)
{
	u8 status;

	nRF_SPIStart();
	status = nRF_SPIRW(NOP);
	nRF_SPIStop();

	return status;
}

/*Clear Status Register*/
void nRF_ClearStatusRegister(void)
{
	nRF_RegisterWriteByte(STATUS, NOP);
}

///*Get Reg Value*/
//void nRF_GetRegValue(u8 *buf)
//{
//	u8 i, j;

//	j = 0;
//	for(i = 0; i <= 0x09; i++)
//	{
//		buf[j] = nRF_RegisterReadByte(i);
//		j++;
//	}
//	for(; i <= 0x0B; i++)
//	{
//		nRF_RegisterReadBuffer(i, (buf+j), 5);
//		j += 5;
//	}
//	for(; i <= 0x0F; i++)
//	{
//		buf[j] = nRF_RegisterReadByte(i);
//		j++;
//	}
//	nRF_RegisterReadBuffer(i, (buf+j), 5);
//	j += 5;
//	i++;
//	for(; i <= 0x17; i++)
//	{
//		buf[j] = nRF_RegisterReadByte(i);
//		j++;
//	}
//	nRF_SPIStart();
//	nRF_SPIRW(R_RX_PL_WID);
//	buf[j] = nRF_SPIRW(NOP);
//	nRF_SPIStop();
//	j++;

//}

/********************************** sub function ***********************************************/
/*SPI Start*/
static void nRF_SPIStart(void)
{
	CE_CLR;
	CSN_CLR;
}

/*SPI READ WRITE*/
u8 nRF_SPIRW(u8 cmd)
{
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2, cmd);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI2);
}

/*SPI Stop*/
static void nRF_SPIStop(void)
{
	CSN_SET;
}


/*Write one Byte to Register*/
u8 nRF_RegisterWriteByte(u8 reg, u8 cmd)
{
	u8 status;

	nRF_SPIStart();
	status = nRF_SPIRW(NRF_WRITE_REG|reg);
	nRF_SPIRW(cmd);
	nRF_SPIStop();

	return status;
}

/*Write Commands to Register*/
static u8 nRF_RegisterWriteBuffer(u8 reg, u8 *cmdbuf, u8 length)
{
	u8 status, i;

	nRF_SPIStart();
	status = nRF_SPIRW(NRF_WRITE_REG|reg);
	for(i = 0; i < length; i++)
	{
		nRF_SPIRW(cmdbuf[i]);
	}
	nRF_SPIStop();

	return status;
}

/*Read one Byte to Register*/
static u8 nRF_RegisterReadByte(u8 reg)
{
	u8 value;

	nRF_SPIStart();
	nRF_SPIRW(NRF_READ_REG|reg);
	value = nRF_SPIRW(NOP);
	nRF_SPIStop();

	return value;
}

/*Read cmds to Register*/
static u8 nRF_RegisterReadBuffer(u8 reg, u8 *cmdbuf, u8 length)
{
	u8 status, i;

	nRF_SPIStart();
	status = nRF_SPIRW(NRF_READ_REG|reg);
	for(i = 0; i < length; i++)
	{
		cmdbuf[i] = nRF_SPIRW(NOP);
	}
	nRF_SPIStop();

	return status;
}


/*Flush Tx Buffer*/
void nRF_FlushTxBuffer(void)
{
	nRF_SPIStart();
	nRF_SPIRW(FLUSH_TX);
	nRF_SPIStop();
}


/*Flush Rx Buffer*/
void nRF_FlushRxBuffer(void)
{
	nRF_SPIStart();
	nRF_SPIRW(FLUSH_RX);
	nRF_SPIStop();
}

/*Write TX Buffer*/
void nRF_WriteTxBuffer(u8 *pbuf, u8 length)
{
	u8 i;

	nRF_SPIStart();
	nRF_SPIRW(WR_TX_PLOAD);
	for(i = 0; i < length; i++)
	{
		nRF_SPIRW(pbuf[i]);
	}
	nRF_SPIStop();
}

/*Read RX Buffer*/
void nRF_ReadRxBuffer(u8 *pbuf, u8 length)
{
	u8 i;

	nRF_SPIStart();
	nRF_SPIRW(RD_RX_PLOAD);
//	uDelay(1);
	for(i = 0; i < length; i++)
	{
		pbuf[i] = nRF_SPIRW(NOP);
	}
	nRF_SPIStop();
}


int nRF_Check(void)
{
	u8 buf[5] = {0xc2, 0xc2, 0xc2, 0xc2, 0xc2};
	u8 buf1[5] = {0, 0, 0, 0, 0};
	int i;

	nRF_RegisterWriteBuffer(TX_ADDR, buf, 5);
	nRF_RegisterReadBuffer(TX_ADDR, buf1, 5);
	
	for (i = 0; i < 5; i++) {
		if (buf1[i] != 0xc2)
			break;
	}

	if (i == 5)
		return 0;
	else
		return -1;
}

/*nRF Send Message*/

//#define C_SENDMSGTIMEOUT 20
//BOOL nRF_SendMessage(u8 *buf, u8 size)
//{
//	u8 stadues;
//	u32 timeout=CMOS_GetTimer();
//	nRF_ClearStatusRegister();
////	nRF_TxSetReg(R_nRF_Addr,32);
//	nRF_RegisterWriteByte(CONFIG, 0x0e);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled..
//	nRF_TxSendData(buf, size);
//	CE_SET;

//	while(1)
//	{
//		if(0==nRF_GetIRQStatus())
//			break;
//		if(timeout-CMOS_GetTimer()>=C_SENDMSGTIMEOUT)
//			return FALSE;
//	}
//	CE_CLR;
//	stadues=nRF_GetStatusRegister();
//	if(C_nRF_StatusTx&stadues)
//		return TRUE;
//	return FALSE;
//}

void SendMessage(u8 *buf, u8 size)
{
	CE_CLR;
	nRF_ClearStatusRegister();
	nRF_RegisterWriteByte(CONFIG, 0x4e);
	nRF_TxSendData(buf, size);
	EXTI_ClearITPendingBit(EXTI_Line6);
	nRF_ClearStatusRegister();
	CE_SET;
}

void ResetRecieveMode(void)
{
	EXTI_ClearITPendingBit(EXTI_Line6);
	nRF_ClearStatusRegister();
	nRF_RecieveMode();
}

extern void nRF_IRQHandler(void);
extern void nRF_ReceiveDataHandler(u8 status);
extern volatile u32 AutoPowerOffCnt;
extern u32 BkAutoPowerOff;
void EXTI9_5_IRQHandler(void)
{
	u8 status;
	 if((EXTI_GetITStatus(EXTI_Line6) != RESET))
	 {
		CE_CLR;
		status = nRF_GetStatusRegister();
		if(C_nRF_StatusRx==(status&C_nRF_StatusRx))
		{
			if(0==(status&0x0E))
			{
				mDelay(1);
				nRF_IRQHandler();
			}else
			{
				nRF_ReceiveDataHandler(status);
			}
		}
		else if(status&(C_nRF_StatusMx+C_nRF_StatusTx)){
			ResetRecieveMode();
		}
		else{ //20170327 sometimes when entry IRQ the status is 0x0e
			#ifdef HANGUP
			Console_Open("IRQ");
			mDelay(1000);
			Console_Open("");
			mDelay(1000);
			#endif
			BkAutoPowerOff=AutoPowerOffCnt;
			EXTI_ClearITPendingBit(EXTI_Line6);
		}
	 }	 
 }
 
/**读取RSSI寄存器测试NRF信号
* @return 1: 信号大于-60dBm
*					0: 信号小于-60dBm
*/
u8 get_NRF_RSSI(void)
{
	return nRF_RegisterReadByte(CD) & 0x01;
}
