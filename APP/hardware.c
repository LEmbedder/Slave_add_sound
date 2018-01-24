#include "hardware.h"
#include "CMOS.h"
#include "NRF24L01.h"  
#include "lcd.h"
#include "Display.h"
#include "pcm.h"
#include "ADC.h"
#include "IC_SPI.h"
#include "Commu.h"
#include "DAC.h"
#include "Flash.h"
#include "File.h"
#include "nRF.h"
#include "g-sensor.h"
#include "voice.h"
#include "Motor.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_dbgmcu.h"
#include "stm32f10x_crc.h"
#include <stdlib.h>
#include "GEngineCfg.h"
#include "FLASHROM.h"
#include "sentence.h"

#include "Config.h"

void NearSensorServer(void);
void NearSensorServer(void)
{
	;
}

uint32_t CRC_MCU(void);
uint32_t CRC_SPI(void);
int slaverpowerOff(PCom_Arg in,int mode);

#define C_Y_SegOff 40
void hardware_main(void)
{
	u32 crc;
	u32 timecnt;
	int exit=1;
	int i;
	Layer_Arg	layers;
	Joystick_Res joyk;
	Location_Res loca;
	Speech_Arg speech;
	speech.voiceID=0x8004;
	u8 flags[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	s16 locations[30]={
		4,4,24,4,44,4,64,4,84,4,104,4,
		24,24,44,24,64,24,84,24,
		24,44,44,44,64,44,84,44,
		54,64
	};
	Com_Res ret;
	#ifdef _SEND_
	flags[10]=1;
	flags[11]=1;
	flags[12]=1;
	flags[13]=1;
	#endif
	for(i=0;i<C_Layer_Max;i++)
	{
		layers.layer[i]=1;
	}
	GEA_ClearLayer(&layers);
	ClearScreen();
	PutString(1,1,(u8 *)"MCU:");//mcu
	
	PutString(1,20,(u8 *)"SPI:");//spi
	
	PutString(128-32,128-16,(u8 *)"V3.0");//version
	
	for(i=0;i<30;i+=2)
	{
	#ifdef _SEND_
		if(44==locations[i+1])
			continue;
	#endif	
		FrushRectangle(RED, locations[i], locations[i+1]+C_Y_SegOff, 16, 16);
	}
	RefreshScreen();
	
	crc = CRC_MCU();//mcu value
	PutHexBlock(1+32,1,4,(u8 *)&crc,4);
	RefreshScreen();
	
	crc = CRC_SPI();//spi value
	PutHexBlock(1+32,20,4,(u8 *)&crc,4);
	RefreshScreen();
	
	GEA_PlaySpeech(&speech);
	
	exit=500;
	while(exit>1)
	{
		GEA_GetJoyStickStatus(0,&joyk);
		
#ifdef _RECEV_
		recevcheck();
		GEA_GetNearStatus(0,&loca);
#endif

		if (timecnt-CMOS_GetTimer() >= 200) {
			timecnt = CMOS_GetTimer();
			gSensor_Update();		
		}		

		stCheck();
		IWDG_ReloadCounter();
		mDelay(2);
		exit--;
	}
	
	crc=CMOS_GetTimer();
	while(exit)
	{
		GEA_GetJoyStickStatus(0,&joyk);
		if(joyk.Surface>=1)
			flags[joyk.Surface-1]=1;
		if(joyk.key>=1)
			flags[joyk.key+5]=1;
		if(joyk.rock>=2)
		{
			GEA_GetSpeechStatus(0,&ret);
			if(C_SpeechEnd==ret.result)
			{
				GEA_PlaySpeech(&speech);
			}
			flags[14]=1;	
		}
#ifdef _RECEV_
		GEA_GetNearStatus(0,&loca);
		if(0!=loca.dir[0])
			flags[10]=1;	
		if(0!=loca.dir[1])
			flags[11]=1;	
		if(0!=loca.dir[2])
			flags[12]=1;	
		if(0!=loca.dir[3])
			flags[13]=1;	
#endif
				
		for(i=0;i<15;i++)
		{
			#ifdef _SEND_
				if(44==locations[i*2+1])
					continue;
			#endif
				
			if(1==flags[i])
				FrushRectangle(GREEN, locations[i*2], locations[i*2+1]+C_Y_SegOff, 16, 16);
		}
		RefreshScreen();

		for(i=0;i<15;i++)
		{
			if(0==flags[i])
				break;
		}
		if(i>=15)
			break;					
		if (timecnt-CMOS_GetTimer() >= 200) {
			timecnt = CMOS_GetTimer();
			gSensor_Update();		
		}		
		
#ifdef _RECEV_
		recevcheck();
#endif		
		
		stCheck();
		IWDG_ReloadCounter();
		if (crc-CMOS_GetTimer() >= 1000*60*3) {
			slaverpowerOff(0,2);
		}		
	}
	
	writeconfig(getConfig());
	
}

uint32_t CRC_MCU(void)
{
	u32 crc;
	IWDG_ReloadCounter();
	CRC_ResetDR();
	crc=CRC_CalcBlockCRC((u32 *)0x8000000,0x20000/4);
	IWDG_ReloadCounter();
	return crc;
}

uint32_t CRC_SPI(void)
{
	u32 spiaddr=FLASH_SYSTEM_ADDR;
	int spiromsize=FLASH_SYSTEM_SIZE;
	u8 buf[256];	
	CRC_ResetDR();
	for(int i=0;i<FLASH_SYSTEM_SIZE/256;i++)
	{
		Flash_Read(spiaddr, buf, 256);
		CRC_CalcBlockCRC((u32 *)buf,256/4);
		spiaddr+=256;
		IWDG_ReloadCounter();
	}
	return CRC_GetCRC();
}
