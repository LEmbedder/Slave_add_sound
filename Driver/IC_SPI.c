
#include "IC_SPI.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "misc.h"
#include "CMOS.h"

//#ifdef _SPI_SLAVER_
//#endif /*_SPI_SLAVER_*/
//#ifdef _SPI_MASTER_
//#endif /*_SPI_MASTER_*/

#define SPI3_CS0_HIGH()   GPIOA->BSRR = GPIO_Pin_15
#define SPI3_CS0_LOW()    GPIOA->BRR = GPIO_Pin_15

#define SPI3_CS1_HIGH()   GPIOB->BSRR = GPIO_Pin_6
#define SPI3_CS1_LOW()    GPIOB->BRR = GPIO_Pin_6

#define SPI3_CS2_HIGH()   GPIOB->BSRR = GPIO_Pin_7
#define SPI3_CS2_LOW()    GPIOB->BRR = GPIO_Pin_7

#define SPI3_CS3_HIGH()   GPIOB->BSRR = GPIO_Pin_8
#define SPI3_CS3_LOW()    GPIOB->BRR = GPIO_Pin_8


#define C_Spi_RxBufSize		640 //256 * 2 + 128
static u8 R_Spi_RxBuffer[C_Spi_RxBufSize];
static STRUCT_FIFO_REG R_Spi_RxFifo;

#define C_Spi_TxBufSize		384		//256+128
static u8 R_Spi_TxBuffer[C_Spi_TxBufSize];
static STRUCT_FIFO_REG R_Spi_TxFifo;


static void SPI3_Master_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	SPI_Cmd(SPI3, DISABLE);
	SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, DISABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
		
	// CSN
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	SPI3_CS0_HIGH();
	SPI3_CS1_HIGH();
	SPI3_CS2_HIGH();
	SPI3_CS3_HIGH();

	// CLK, MISO, MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; //SPI_CPOL_High; //
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //SPI_CPHA_1Edge; //
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

	SPI_Init(SPI3, &SPI_InitStructure);
	SPI_Cmd(SPI3, ENABLE);
}

static void SPI3_Slave_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;


	SPI_Cmd(SPI3, DISABLE);
	SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, DISABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	//*
	// CSN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// CLK
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//*/
		
////	// CSN
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
////	GPIO_Init(GPIOA, &GPIO_InitStructure);

////	// CLK, MISO, MOSI
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; 
////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
////	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; //SPI_CPOL_High; //
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //SPI_CPHA_1Edge; //
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;


	NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
	SPI3_Priority();	 
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
	SPI3->DR = 0x00;

	SPI_Init(SPI3, &SPI_InitStructure);
	SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, ENABLE);
	SPI_Cmd(SPI3, ENABLE);
}

u32 r_spi_debug_cnt = 0;
void SPI3_IRQHandler(void)
{
	u8 dat;

	if (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == SET)
	{
		//dat = (u8)SPI_I2S_ReceiveData(SPI3);
		dat = SPI3->DR;
		FIFO_WriteData(R_Spi_RxBuffer, &R_Spi_RxFifo, dat);
		//while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
		dat = C_IC_Spi_None;
		FIFO_ReadData(R_Spi_TxBuffer, &R_Spi_TxFifo, &dat);
		//SPI_I2S_SendData(SPI3, dat);
		SPI3->DR = dat;
		r_spi_debug_cnt++;
	}
}

/*IC SPI Initialization*/
void IC_SPI_Init(u8 mode)
{
	if(mode == C_IC_SPI_Mode_Slaver)
	{
		SPI3_Slave_Init();
		FIFO_Init(&R_Spi_RxFifo, C_Spi_RxBufSize);
		FIFO_Init(&R_Spi_TxFifo, C_Spi_TxBufSize);
	}
	else if(mode == C_IC_SPI_Mode_Master)
	{
		SPI3_Master_Init();
		SPI3_CS0_HIGH();
		SPI3_CS1_HIGH();
		SPI3_CS2_HIGH();
		SPI3_CS3_HIGH();
	}
}

/*Reset Recieve Buffer*/
void IC_SPI_RxBuffer_Reset(void)
{
	FIFO_Reset(&R_Spi_RxFifo);
}

/*Reset Transport Buffer*/
void IC_SPI_TxBuffer_Reset(void)
{
	FIFO_Reset(&R_Spi_TxFifo);
}

/*Read Byte*/
BOOL IC_SPI_ReadByte(u8 *dat)
{
	return FIFO_ReadData(R_Spi_RxBuffer, &R_Spi_RxFifo, dat);
}

/*Read Block*/
BOOL IC_SPI_ReadBlock(u8 *buffer, u16 size, u32 timeoutCnt)
{
	u32 i;
	u8 msg;
	BOOL res = TRUE;
	u32 timeCnt;
	
	timeCnt = CMOS_GetTimer();
	i = 0;
	while(i < size)
	{
		if(IC_SPI_ReadByte(&msg) == TRUE)
		{
			buffer[i] = msg;
			timeCnt = CMOS_GetTimer();
			i++;
		}
		if(CMOS_GetTimerCount(timeCnt) >= timeoutCnt)
		{
			res = FALSE;
			break;
		}
	}
	
	return res;
}

/*Write Byte*/
void IC_SPI_WriteByte(u8 dat)
{
	FIFO_WriteData(R_Spi_TxBuffer, &R_Spi_TxFifo, dat);
}

/*Write Block*/
void IC_SPI_WriteBlock(u8 *buffer, u16 size)
{
	u16 i;
	
	for(i = 0; i < size; i++)
	{
		IC_SPI_WriteByte(buffer[i]);
	}
}

/*Write String*/
void IC_SPI_WriteString(u8 *str)
{
	u16 i = 0;
	
	while(str[i] != '\0')
	{
		IC_SPI_WriteByte(str[i]);
		i++;
	}
}

/*IC SPI Read Write*/
u8 IC_SPI_RW(u8 slaverID, u8 dat)
{
	u8 res;
	
//	GPIO_ResetBits(GPIOB, GPIO_Pin_3);
	//GPIO_SetBits(GPIOB, GPIO_Pin_3);
	//GPIO_ResetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	switch(slaverID)
	{
		case 0:
			SPI3_CS0_LOW();
			break;
		
		case 1:
			SPI3_CS1_LOW();
			break;
		
		case 2:
			SPI3_CS2_LOW();
			break;
		
		case 3:
			SPI3_CS3_LOW();
			break;
		
		default:
			SPI3_CS0_HIGH();
			SPI3_CS1_HIGH();
			SPI3_CS2_HIGH();
			SPI3_CS3_HIGH();
			return 0;
	}
//	uDelay(5);
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
	//SPI_I2S_SendData(SPI3, dat);
	SPI3->DR = dat;
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
	res = SPI3->DR;
//	res = (u8)SPI_I2S_ReceiveData(SPI3);
//	uDelay(5);

	switch(slaverID)
	{
		case 0:
			SPI3_CS0_HIGH();
			break;
		
		case 1:
			SPI3_CS1_HIGH();
			break;
		
		case 2:
			SPI3_CS2_HIGH();
			break;
		
		case 3:
			SPI3_CS3_HIGH();
			break;
		
		default:
			SPI3_CS0_HIGH();
			SPI3_CS1_HIGH();
			SPI3_CS2_HIGH();
			SPI3_CS3_HIGH();
			break;
	}
	
	return res;
}


/*IC SPI Read Write Block*/
void IC_SPI_RW_Block(u8 slaverID, u8 *txBuf, u32 txSize, u8 *rxBuf)
{
	u8 dat;
	u32 i;
	
	switch(slaverID)
	{
		case 0:
			SPI3_CS0_LOW();
			break;
		
		case 1:
			SPI3_CS1_LOW();
			break;
		
		case 2:
			SPI3_CS2_LOW();
			break;
		
		case 3:
			SPI3_CS3_LOW();
			break;
		
		default:
			SPI3_CS0_HIGH();
			SPI3_CS1_HIGH();
			SPI3_CS2_HIGH();
			SPI3_CS3_HIGH();
			return ;
	}
	
	for(i = 0; i < txSize; i++)
	{
		while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
		SPI_I2S_SendData(SPI3, txBuf[i]);
		while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
		dat = (u8)SPI_I2S_ReceiveData(SPI3);
		if(rxBuf != 0)
		{
			rxBuf[i] = dat;
		}
	}

	switch(slaverID)
	{
		case 0:
			SPI3_CS0_HIGH();
			break;
		
		case 1:
			SPI3_CS1_HIGH();
			break;
		
		case 2:
			SPI3_CS2_HIGH();
			break;
		
		case 3:
			SPI3_CS3_HIGH();
			break;
		
		default:
			SPI3_CS0_HIGH();
			SPI3_CS1_HIGH();
			SPI3_CS2_HIGH();
			SPI3_CS3_HIGH();
			break;
	}}
