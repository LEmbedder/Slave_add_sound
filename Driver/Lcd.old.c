/*
 * Filename:	LCD.c
 * Author:		ZouChao, C.M.16
 * Date:		2015-07-12
 * Version:		v0.2
 * Discription:	Lcd driver
 */

//MCU: STM32

//IC: ST7735S
//Clock: 13M

#include "Lcd.h"
#include "CMOS.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"

/*测试程序横竖屏切换，横屏请开启此宏，竖屏请屏蔽*/
//#define USE_LANDSCAPE		

//---------------------------液晶屏接线说明-------------------------------------//

//RESET			PB10
//RS			PB11
//CS			PB12
//SCLK			PB13
//BL			PB14
//SDA			PB15






#define Lcd_SCLK_Low()			GPIOA->BRR = GPIO_Pin_5
#define Lcd_SCLK_High()			GPIOA->BSRR = GPIO_Pin_5
#define Lcd_SDA_Low()			GPIOA->BRR = GPIO_Pin_7
#define Lcd_SDA_High()			GPIOA->BSRR = GPIO_Pin_7
#define Lcd_RS_Low()			GPIOA->BRR = GPIO_Pin_8
#define Lcd_RS_High()			GPIOA->BSRR = GPIO_Pin_8
#define Lcd_CS_Low()			GPIOA->BRR = GPIO_Pin_6
#define Lcd_CS_High()			GPIOA->BSRR = GPIO_Pin_6
#define Lcd_Reset_Low()			GPIOA->BRR = GPIO_Pin_0
#define Lcd_Reset_High()		GPIOA->BSRR = GPIO_Pin_0
#define Lcd_BackLight_Low()		GPIOA->BRR = GPIO_Pin_1
#define Lcd_BackLight_High()	GPIOA->BSRR = GPIO_Pin_1


/*SPI Config*/
static void Lcd_SPI_Config(void);
/*SPI Write Data*/
static void Lcd_SPI_Write(u8 Data);

/************************************** funtion **************************************/
/*SPI Config*/
static void Lcd_SPI_Config(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	SPI_Cmd(SPI1, DISABLE);
	
	/* GPIO Configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	/* GPIO Configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_6|GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	/* SPI2 Configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;				//只发送数据
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 						//发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;								//时钟悬空低
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;   							//数据捕获与第一个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;								//软件控制CS
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//波特率预分频值位64 	72/8 = 9M S
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 						//数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	/* Enable SPI2  */
	SPI_Cmd(SPI1, ENABLE);



/*模拟SPI*/
//	GPIO_InitTypeDef GPIO_InitStructure;
//
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

/*SPI Write Data*/
static void Lcd_SPI_Write(u8 Data)
{
/*SPI单工*/
	while((SPI1->SR &SPI_I2S_FLAG_TXE) == RESET);
	SPI1->DR = Data;
	while((SPI1->SR &SPI_I2S_FLAG_BSY) != RESET);
	
/*SPI全双工*/
//	while((SPI2->SR &SPI_I2S_FLAG_TXE) == RESET);
//	SPI2->DR = Data;
//	while((SPI2->SR &SPI_I2S_FLAG_RXNE)==RESET);
//	SPI2->DR;

/*模拟SPI*/
//	u8 i = 0x80;
//
//	while(i)
//	{
//		if(Data&i) //输出数据	
//			Lcd_SDA_High();
//		else
//			Lcd_SDA_Low();
//		Lcd_SCLK_Low();
//		i >>= 1;
//		Lcd_SCLK_High();
//	}		
}

/*Write CMD*/
static void Lcd_WriteIndex(u8 Data)
{
	Lcd_CS_Low();
	Lcd_RS_Low();
	Lcd_SPI_Write(Data); 		
	Lcd_CS_High();
}

/*Write 8 bit Data*/
static void Lcd_WriteData(u8 Data)
{	
	Lcd_CS_Low();
	Lcd_RS_High();
	Lcd_SPI_Write(Data); 		
	Lcd_CS_High();
}

///*Write 16 bit Data*/
//static void LCD_WriteData_16Bit(u16 Data)
//{
//	Lcd_CS_Low();
//	Lcd_RS_High();
//	Lcd_SPI_Write(Data>>8); 		//写入高8位数据
//	Lcd_SPI_Write(Data); 			//写入低8位数据
//	Lcd_CS_High();
//}

/*LCD Reset*/
static void Lcd_Reset()
{
    Lcd_Reset_Low();
    mDelay(100);
    Lcd_Reset_High();
    mDelay(100);
}
//////////////////////////////////////////////////////////////////////////////////////////////
/*Lcd Initialization*/
void Lcd_Init(void)
{
	Lcd_SPI_Config();
	Lcd_SCLK_High();
	Lcd_CS_High();
	Lcd_BackLight_Low();
	Lcd_Reset();//Reset before LCD Init.

	Lcd_WriteIndex(0x11);//Sleep exit 
	mDelay (120);
	Lcd_BackLight_High();
		
	//ST7735R Frame Rate
	Lcd_WriteIndex(0xB1); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 

	Lcd_WriteIndex(0xB2); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 

	Lcd_WriteIndex(0xB3); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 
	
	Lcd_WriteIndex(0xB4); //Column inversion 
	Lcd_WriteData(0x07); 
	
	//ST7735R Power Sequence
	Lcd_WriteIndex(0xC0); 
	Lcd_WriteData(0xA2); 
	Lcd_WriteData(0x02); 
	Lcd_WriteData(0x84); 
	Lcd_WriteIndex(0xC1); 
	Lcd_WriteData(0xC5); 

	Lcd_WriteIndex(0xC2); 
	Lcd_WriteData(0x0A); 
	Lcd_WriteData(0x00); 

	Lcd_WriteIndex(0xC3); 
	Lcd_WriteData(0x8A); 
	Lcd_WriteData(0x2A); 
	Lcd_WriteIndex(0xC4); 
	Lcd_WriteData(0x8A); 
	Lcd_WriteData(0xEE); 
	
	Lcd_WriteIndex(0xC5); //VCOM 
	Lcd_WriteData(0x0E); 
	
	Lcd_WriteIndex(0x36); //MX, MY, RGB mode 
#ifdef USE_LANDSCAPE
	Lcd_WriteData(0xA8); //竖屏C8 横屏08 A8
#else
	Lcd_WriteData(0xC8); //竖屏C8 横屏08 A8
#endif		
	//ST7735R Gamma Sequence
	Lcd_WriteIndex(0xe0); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x1a); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x18); 
	Lcd_WriteData(0x2f); 
	Lcd_WriteData(0x28); 
	Lcd_WriteData(0x20); 
	Lcd_WriteData(0x22); 
	Lcd_WriteData(0x1f); 
	Lcd_WriteData(0x1b); 
	Lcd_WriteData(0x23); 
	Lcd_WriteData(0x37); 
	Lcd_WriteData(0x00); 	
	Lcd_WriteData(0x07); 
	Lcd_WriteData(0x02); 
	Lcd_WriteData(0x10); 

	Lcd_WriteIndex(0xe1); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x1b); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x17); 
	Lcd_WriteData(0x33); 
	Lcd_WriteData(0x2c); 
	Lcd_WriteData(0x29); 
	Lcd_WriteData(0x2e); 
	Lcd_WriteData(0x30); 
	Lcd_WriteData(0x30); 
	Lcd_WriteData(0x39); 
	Lcd_WriteData(0x3f); 
	Lcd_WriteData(0x00); 
	Lcd_WriteData(0x07); 
	Lcd_WriteData(0x03); 
	Lcd_WriteData(0x10);  
	
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00+2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x80+2);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00+3);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x80+3);
	
	Lcd_WriteIndex(0xF0); //Enable test command  
	Lcd_WriteData(0x01); 
	Lcd_WriteIndex(0xF6); //Disable ram power save mode 
	Lcd_WriteData(0x00); 
	
	Lcd_WriteIndex(0x3A); //65k mode 
	Lcd_WriteData(0x05); 
	
	Lcd_WriteIndex(0x29);//Display on
}



/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
void Lcd_SetRegion(u16 x_start, u16 y_start, u16 x_end, u16 y_end)
{	
#ifdef USE_LANDSCAPE//使用横屏模式
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start+3);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end+3);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start+2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end+2);

#else//竖屏模式	
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start+2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end+2);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start+3);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end+3);	
#endif
	Lcd_WriteIndex(0x2c);
}

void Lcd_ClearScreen(u16 color)
{
 	u8 i, j;

	Lcd_SetRegion(0, 0, C_LCD_Height-1, C_LCD_Width-1);
	Lcd_CS_Low();
	Lcd_RS_High();
 	for (i = 0; i < C_LCD_Height; i++)
	{
    	for (j = 0; j < C_LCD_Width; j++)
		{
			Lcd_SPI_Write(color>>8); 		//写入高8位数据
			Lcd_SPI_Write(color); 			//写入低8位数据
		}
	}
	Lcd_CS_High();
}

/*Refresh Screen*/
void Lcd_RefreshScreen(u16 *buffer)
{
 	u32 i, max;

	max = C_LCD_Height*C_LCD_Width;
	Lcd_SetRegion(0, 0, C_LCD_Height-1, C_LCD_Width-1);
	Lcd_CS_Low();
	Lcd_RS_High();
 	for (i = 0; i < max; i++)
	{
		Lcd_SPI_Write(buffer[i]>>8);
		Lcd_SPI_Write(buffer[i]);
	}
	Lcd_CS_High();
}
