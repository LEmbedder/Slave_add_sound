/*
 * Filename:	Test.h
 * Author:		ZouChao, C.M.16
 * Date:		2015-07-04
 * Version:		v0.2
 * Discription:	test app
 */

#include "TestMode.h"
#include "CMOS.h"
#include "USART.h"
#include "Display.h"
#include "ADXL345.h"
#include "DAC.h"
#include "lcd.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "pcm.h"
#include "Power.h"
#include "NRF24L01.h"  
#include "Detector.h"
#include "g-sensor.h"
#include "nRF.h"
#include "Flash.h"
#include "GEngineCfg.h"
#include "MMA7660FC.h"
#include <stdlib.h>
#include <string.h>

int Tst_Power_Timer;

/*Lcd test*/
static void TstMode_Lcd(void);
/*G Sensor test*/
static void TstMode_GSensor(void);
/*Dac test*/
static void TstMode_DAC(void);
/*Usart Test*/
static void TstMode_USART(void);

static void TstMode_Motor(void);

static void TstMode_Pcm(void);

static void TstMode_Power(void);

static void TstMode_nRF(void);

static void TstMode_Detector(void);

static void TstMode_GSensor2(void);

static void TstMode_24G_Master(void);

static void TstMode_24G_Slave(void);

static void TstMode_Power_Time(void);

static void TstMode_MS_Master(void);

static void TstMode_GSensor_Key(void);

static void TstMode_MMA7660FC(void);

static void MS_PutGraph(u16 id, s16 x, s16 y, u8 layer);
static void MS_ClearLayer(u8 layer);
static u8 MS_GetJoyStickStatus(void);
static u8 MS_GetShakeStatus(void);

/*************************************** function *********************************************/
/*Test Mode Main Function*/
u8 TestMode(u8 para)
{
//	para = C_TstMode_USART;
	switch(para)
	{
		case C_TstMode_Lcd:
			TstMode_Lcd();
			break;

		case C_TstMode_GSensor:
			TstMode_GSensor();
			break;

		case C_TstMode_DAC:
			TstMode_DAC();
			break;

		case C_TstMode_USART:
			TstMode_USART();
			break;

		case C_TstMode_Motor:
			TstMode_Motor();
			break;

		case C_TstMode_Pcm:
			TstMode_Pcm();
			break;
			
		case C_TstMode_Power:
			TstMode_Power();
			break;
			
		case C_TstMode_nRF:
			TstMode_nRF();
			break;
			
		case C_TstMode_Detector:
			TstMode_Detector();
			break;
			
		case C_TstMode_GSensor2:
			TstMode_GSensor2();
			break;
			
		case C_TstMode_24G_Master:
			TstMode_24G_Master();
			break;
			
		case C_TstMode_24G_Slave:
			TstMode_24G_Slave();
			break;
			
		case C_TstMode_Power_Time:
			TstMode_Power_Time();
			break;
			
		case C_TstMode_MS_Master:
			TstMode_MS_Master();
			break;
			
		case C_TstMode_GSensor_Key:
			TstMode_GSensor_Key();
			break;
			
		case C_TstMode_MMA7660FC:
			TstMode_MMA7660FC();
			break;

		default:
			break;
	}
	return 0;
}

static void TstMode_MMA7660FC(void)
{
	u8 buf[3];
	int i;
	
	MMA7660FC_Init();
	
	while(1) {
		buf[0] = MMA7660FC_X();
		buf[1] = MMA7660FC_Y();
		buf[2] = MMA7660FC_Z();
		
		if (buf[0] || buf[1] || buf[2])
			i++;
		
		RS_SendBlock(buf, 3);
	}
}

#define KEY_MIN_ANGLE     30

static u8 GetKey(void)
{
	s8 x, y, z;
	s8 x_abs, y_abs, z_abs;
	u8 key;
	
	key = Key_Null;
	
	if (gSensor_WaveRank() >= g_wave_2)
		return key;
		
	x = gSensor_Box_X_Angle();
	y = gSensor_Box_Y_Angle();
	z = gSensor_Box_Z_Angle();
	
	if (x >= 0)
		x_abs = x;
	else
		x_abs = -x;
		
	if (y >= 0)
		y_abs = y;
	else
		y_abs = -y;
		
	if (z >= 0)
		z_abs = z;
	else
		z_abs = -z;
		
	if (x_abs >= y_abs) {
		if ((x_abs >= KEY_MIN_ANGLE) && (z_abs >= KEY_MIN_ANGLE)) {
			if (x > 0)
				key = Key_Left;
			else
				key = Key_Right;
		}
	} else {
		if ((y_abs >= KEY_MIN_ANGLE) && (z_abs >= KEY_MIN_ANGLE)) {
			if (y > 0)
				key = Key_Down;
			else
				key = Key_Up;
		}
	}
	
	return key;
}

extern int demo_timer;

static void TstMode_GSensor_Key(void)
{
	u8 status;
	int i;
	s16 x, y;
	
	while (1) {
		gSensor_Main();
		if (demo_timer > 300) {
			gSensor_Analyse();
			
			if (gSensor_WaveRank() >= g_wave_2) {
				i++;
				if (i >= 10)
					i = 0;
				x = 0;
				y = 0;
			}
		}
		
		status = GetKey();
		switch (status) {
		case Key_Left:
			x = x -5;
			break;
		case Key_Right:
			x = x + 5;
			break;
		case Key_Up:
			y = y - 5;
			break;
		case Key_Down:
			y = y + 5;
			break;
		default:;
		}
		
		FrushDspBuffer(WHITE);
		PutGraph(i, x, y);
		RefreshScreen();
		mDelay(50);
	}
}

static void TstMode_MS_Master(void)
{
	u8 buf[32];
	int i;
	u8 status;
	s16 x, y;
	
	i = 0;
	
	while (1) {
		status = MS_GetJoyStickStatus();
		switch (status) {
		case Key_Left:
			x = x -5;
			break;
		case Key_Right:
			x = x + 5;
			break;
		case Key_Up:
			y = y - 5;
			break;
		case Key_Down:
			y = y + 5;
			break;
		default:;
		}
	
		status = MS_GetShakeStatus();
		if (status >= g_wave_2) {
			i++;
			if (i >= 10)
				i = 0;
			x = 0;
			y = 0;
		}
	
		MS_ClearLayer(0);
		MS_PutGraph(i, x, y, 0);
			
		mDelay(100);
	}
}

static void TstMode_Power_Time(void)
{
	unsigned int addr = 0x800000;
	unsigned int sec = 0;
	u8 buf[256];
	int flag = 1;
	int i = 0;
	
	Tst_Power_Timer = 0;
	
	pcm_Init();
	pcm_Play(0);
	
	while (1) {
		pcm_checkmsg();
		if (pcm_IsPlaying() == 0) {
//			pcm_Rdown();
			while(0!=pcm_IsPlaying());
			mDelay(100);
			pcm_Play(0);
		}
		
		nRF_SendMessage(buf, 32);
	
		if (Tst_Power_Timer >= 1000) {
			Tst_Power_Timer = 0;
			
			if (flag) {
				flag = 0;
				addr = addr + 8 * 1024;
			} else {
				flag = 1;
				addr = addr - 8 * 1024;
			}
			
			sec++;
			
			Flash_4K_Erase(addr);
			
			buf[0] = *((u8 *)(&sec) + 0);
			buf[1] = *((u8 *)(&sec) + 1);
			buf[2] = *((u8 *)(&sec) + 2);
			buf[3] = *((u8 *)(&sec) + 3);
			
			Flash_Write_Page(addr, buf, 256);
			
			FrushDspBuffer(WHITE);
			PutGraph(i, 0, 0);
			RefreshScreen();
			i++;
			if (i >= File_GetGraphSum())
				i = 0;
		}
	}
	
	
	Flash_Read(0x800000, buf, 4);
	*((u8 *)(&sec) + 0) = buf[0];
	*((u8 *)(&sec) + 1) = buf[1];
	*((u8 *)(&sec) + 2) = buf[2];
	*((u8 *)(&sec) + 3) = buf[3];
	sec++;
	
	Flash_Read(0x800000 + 8 * 1024, buf, 4);
	*((u8 *)(&sec) + 0) = buf[0];
	*((u8 *)(&sec) + 1) = buf[1];
	*((u8 *)(&sec) + 2) = buf[2];
	*((u8 *)(&sec) + 3) = buf[3];
	sec++;
}

static void TstMode_24G_Master(void)
{
	u8 addr[5] = {0x16, 0x35, 0x00, 0x36, 0x04};
	u8 buf[8];
	int i;
	
	nRF_Initialization();
	nRF_SetAddress(addr);
	
	while (1) {
		for (i = 0; i < 8; i++)
			buf[i] = 0xaa;
		
		nRF_IrqInterruptDisable();
		
		if (nRF_SendMessage(buf, 8) == TRUE) {
				
			nRF_RecieveMode(8);
			nRF_IrqInterruptEnable();
			
			mDelay(200);
			
			if (nRF_GetMessage(buf) == TRUE) {
				Console_Add_Hex(buf, 4);
			}
		}
	}
}

static void TstMode_24G_Slave(void)
{
	u8 addr[5] = {0x16, 0x35, 0x00, 0x36, 0x04};
	u8 buf[8];
	int i;
	int count;
	
	nRF_Initialization();
	nRF_SetAddress(addr);
	
	while (1) {
		nRF_RecieveMode(8);
		nRF_IrqInterruptEnable();
		
		while (nRF_GetMessage(buf) == FALSE);
		
		Console_Add_Hex(buf, 8);
		
		for (i = 0; i < 8; i++)
			buf[i] = buf[i] + 1;
		count = 10;
		nRF_IrqInterruptDisable();
		
		while (count) {
			if (nRF_SendMessage(buf, 8) == TRUE)
				break;
				
			count--;
		}
	}
}

static void TstMode_GSensor2(void)
{
	struct variance {
		s32 x2;
		s32 y2;
		s32 z2;
		u8 rank;
		u8 side;
		s8 x_angle;
		s8 y_angle;
		s8 z_angle;
	} vv;
	
	int i;
	
	gSensor_Init();
	
	while (1) {
		for (i = 0; i < 2; i++) {
			gSensor_Main();
			mDelay(2);
		}
		
		gSensor_Variance(&vv.x2, &vv.y2, &vv.z2);
		
		gSensor_Analyse();
		
		vv.rank = (u8)gSensor_WaveRank();
		vv.side = (u8)gSensor_TopSurface();
		vv.x_angle = gSensor_X_Angle();
		vv.y_angle = gSensor_Y_Angle();
		vv.z_angle = gSensor_Z_Angle();
		
		RS_SendBlock((u8 *)&vv, 17);
		
		FrushDspBuffer(Color_BackGround);
		PutString(10, 0, "X:");
		PutString(10, 15, "Y:");
		PutString(10, 30, "Z:");
		PutString(10, 45, "rank:");
		PutString(10, 60, "side:");
		PutString(10, 75, "x_off:");
		PutString(10, 90, "y_off:");
		PutString(10, 105, "z_off:");
		PutNum(80, 0, (s32)vv.x2);
		PutNum(80, 15, (s32)vv.y2);
		PutNum(80, 30, (s32)vv.z2);
		PutNum(80, 45, (s32)vv.rank);
		PutNum(80, 60, (s32)vv.side);
		PutNum(80, 75, (s32)vv.x_angle);
		PutNum(80, 90, (s32)vv.y_angle);
		PutNum(80, 105, (s32)vv.z_angle);
		RefreshScreen();
	}
}

static void TstMode_Detector(void)
{
	u16 value1, value2;
	u16 color1, color2;
	
//	ADC1_GPIO_Cmd(ENABLE);
	
	while (1) {
		Detector_Init(box_up);
		value1 = Detector_GetAverageValue(20, 5);
		Detector_Close();
		if (value1 > 0x4d8) {   // 1.0v--0x4d8, 0.75v--0x3a2  0.5v--26c  0.3v--0x174
			value1 = 1;
			color1 = RED;
		} else {
			value1 = 0;
			color1 = GREEN;
		}
		
		Detector_Init(box_down);
		value2 = Detector_GetAverageValue(20, 5);
		Detector_Close();
		if (value2 > 0x4d8) {
			value2 = 1;
			color2 = RED;
		} else {
			value2 = 0;
			color2 = GREEN;
		}
		
		FrushDspBuffer(Color_BackGround);
	//	PutString(10, 20, "Up:");
	//	PutString(10, 50, "Down:");
		PutNum(65, 25, value1);
		PutNum(65, 85, value2);
		FrushRectangle(color1, 6, 9, 50, 50);
		FrushRectangle(color2, 6, 69, 50, 50);
		RefreshScreen();
	}
	
//	ADC1_GPIO_Cmd(DISABLE);
}

u8 nRF_Addr[5] = {0x16, 0x35, 0x00, 0x36, 0x03};

static void TstMode_nRF(void)
{
	u8 buf[32];
	u8 status;
	
	nRF_Init();
	
	if (nRF_Check()	== 0)
		PutString(0, 0, "working");
	else
		PutString(0, 20, "failed");
	
	while(1) {

		/* receive */
		nRF_FlushRxBuffer();
		nRF_RxSetReg(nRF_Addr, 8);
		nRF_RxMode();

		while(nRF_GetIRQStatus() != 0);

		nRF_StandbyMode();

		status = nRF_GetStatusRegister();
		nRF_ClearStatusRegister();

		if (status & C_nRF_StatusRx) {
		   nRF_RxGetData(buf, 8);
		   PutHex(10, 40, buf[0]);
		   PutHex(10, 60, buf[1]);
		   PutHex(10, 80, buf[2]);
		   PutHex(10, 100, buf[3]);
		   PutHex(40, 40, buf[4]);
		   PutHex(40, 60, buf[5]);
		   PutHex(40, 80, buf[6]);
		   PutHex(40, 100, buf[7]);
		   
		RefreshScreen();
		}

	}
}

static void TstMode_Power(void)
{
	u8 percent;
	float voltage;
	char buf[10];
	
	Power_Init();
	
	
	while (1) {
		FrushDspBuffer(Color_BackGround);
		
		PutString(1, 10, "State:");
		PutString(1, 50, "Voltage:");
		PutString(1, 90, "Percent:");
		
		if (Power_IsCharging()) {
			PutString(20, 30, "Charging");
		} else if (Power_IsChargeDone()) {
			PutString(20, 30, "Charge done"); 
		} else {
			PutString(20, 30, "Unconnected");
		}
		
		voltage = Power_GetVoltage();
		memset(buf, 0, 10);
		sprintf((char *)buf, "%03.5f", voltage);
		PutString(20, 70, (char *)buf);
		
		percent = Power_GetVoltagePercentage();
		memset(buf, 0, 10);
		sprintf(buf, "%d", percent);
		PutString(20, 110, (u8 *)buf);
		
		RefreshScreen();
		mDelay(1000);
	}
}

static void TstMode_Pcm(void)
{
//	pcm_Init();

//	pcm_Play(0);

//	while (1) {
//		pcm_checkmsg();

//		if (pcm_IsPlaying() == 0) {
//			pcm_Rdown();
//			while(0!=pcm_IsPlaying());
//			mDelay(100);
//			pcm_Play(0);
//		}
//	}
}

static void TstMode_Motor(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	while(1) {
		GPIOB->BSRR = GPIO_Pin_11; 
		mDelay(500);
		GPIOB->BRR = GPIO_Pin_11;
		mDelay(500);
	}	
}

/*Lcd test*/
static void TstMode_Lcd(void)
{
	u16 i;

	Display_Initialization();
	while(1)
	{
//		FrushDspBuffer(i++);
//		RefreshScreen();			
		for(i = 0xf800; i < 0xf820; i++)
		{
			FrushDspBuffer(i);
			PutString(16, 16, "Hey CM16!");
			RefreshScreen();			
		}
	}
}

/*G Sensor test*/
static void TstMode_GSensor(void)
{
	u8 deviceID;
	u8 buffer[6];
	s16 num;

	ADXL_Init();

//	PutString(10, 0, "G Sensor:");
//	PutString(10, 12, "Device ID:");
	deviceID = ADXL_GetDeviceID();
//	PutHex(100, 12, deviceID);
//	RefreshScreen();
//	PutString(10, 30, "X:");
//	PutString(10, 45, "Y:");
//	PutString(10, 60, "Z:");
	while(1)
	{
		ADXL_GetXYZValue(buffer);
	FrushDspBuffer(Color_BackGround);
	PutString(10, 0, "G Sensor:");
	PutString(10, 12, "Device ID:");
	deviceID = ADXL_GetDeviceID();
	PutHex(100, 12, deviceID);
	PutString(10, 30, "X:");
	PutString(10, 45, "Y:");
	PutString(10, 60, "Z:");
		RS_SendBlock(buffer, 6);
		num = *(s16 *)(&buffer[0]) - (-21);
		PutNum(30, 30, (s32)num);
		num = *(s16 *)(&buffer[2]) - (130);
		PutNum(30, 45, (s32)num);
		num = *(s16 *)(&buffer[4]) - (1209);
		PutNum(30, 60, (s32)num);
//		PutHex(30, 30, buffer[1]);
//		PutHex(50, 30, buffer[0]);
//		PutHex(30, 45, buffer[3]);
//		PutHex(50, 45, buffer[2]);
//		PutHex(30, 60, buffer[5]);
//		PutHex(50, 60, buffer[4]);
		RefreshScreen();
		
		mDelay(10);

	}
}

/*Dac test*/
static void TstMode_DAC(void)
{
//	u16 dac = 0;
//
//	Display_Initialization();
//	DAC_Initialization();
//
//	PutString(10, 0, "DAC:");
//	PutString(10, 12, "Value:");

//	while(1)
//	{
//		dac++;
//		if(dac > 0x0fff)
//		{
//			dac = 0;
//		}
//		DAC_OutPut(dac);
//		PutNum(30, 30, (s32)dac);
//		RefreshScreen();
//	}

	u32 dly;
	u32 times;

	Display_Initialization();
	DAC_Initialization();

	PutString(10, 0, "DAC Wave:");
	RefreshScreen();

	dly = 1;
	while(1)
	{
		for(times = 0; times < 255; times++)
		{
			DAC_OutPut(0);
			uDelay(dly);
			DAC_OutPut(0x040);
			uDelay(dly);
			DAC_OutPut(0x080);
			uDelay(dly);
			DAC_OutPut(0x040);
			uDelay(dly);
		}
		dly += 10;
		if(dly >= 200)
		{
			dly = 1;
		}
	}
}

/*Usart Test*/
static void TstMode_USART(void)
{
	u8 msg;

	RS_Init(115200);
	for(msg = 0; msg < 5; msg++)
	{
		RS_SendString("Hey~ CM16!\n");
		mDelay(1000);
	}
	while(1)
	{
		if(RS_GetMessage(&msg) == TRUE)
		{
			msg++;
			RS_SendMessage(msg);
		}
	}
}


static void MS_PutGraph(u16 id, s16 x, s16 y, u8 layer)
{
	u8 buf[32];
	
	buf[0] = Enum_GE_Msg_PutGraph;
	buf[1] = 1;
	buf[2] = *((u8 *)(&id) + 0);
	buf[3] = *((u8 *)(&id) + 1);
	buf[4] = *((u8 *)(&x) + 0);
	buf[5] = *((u8 *)(&x) + 1);
	buf[6] = *((u8 *)(&y) + 0);
	buf[7] = *((u8 *)(&y) + 1);
	buf[8] = layer;
	buf[31] = 1;
	
	nRF_SendMessage(buf, 32);
}

static void MS_ClearLayer(u8 layer)
{
	u8 buf[32];
	
	buf[0] = Enum_GE_Msg_ClearLayer;
	buf[layer + 1] = 1;

	nRF_SendMessage(buf, 32);
}

static u8 MS_GetJoyStickStatus(void)
{
	u8 buf[32];
	
	buf[0] = Enum_GE_Msg_Joystick;
	nRF_SendMessage(buf, 32);
	
	nRF_RecieveMode(C_nRF_CtrlCmdSize);
	nRF_IrqInterruptEnable();
	
	mDelay(100);
	
	if (nRF_GetMessage(buf) == TRUE) {
		return buf[1];
	} else {
		return Key_Null;
	}
}

static u8 MS_GetShakeStatus(void)
{
	u8 buf[32];
	
	buf[0] = Enum_GE_Msg_Joystick;
	nRF_SendMessage(buf, 32);
	
	nRF_RecieveMode(C_nRF_CtrlCmdSize);
	nRF_IrqInterruptEnable();
	
	mDelay(100);
	
	if (nRF_GetMessage(buf) == TRUE) {
		return buf[1];
	} else {
		return 0;
	}
}

