
#include "demo.h"
#include "stm32f10x.h"
#include "CMOS.h"
#include "USART.h"
#include "NRF24L01.h"  
#include "lcd.h"
#include "Display.h"
#include "pcm.h"
#include "ADC.h"
#include "IC_SPI.h"
#include "Commu.h"
#include "DAC.h"
#include "ADXL345.h"
#include "Flash.h"
#include "File.h"
#include "nRF.h"
#include "g-sensor.h"
#include "data_bmp.h"
#include "voice.h"
#include "Motor.h"
#include "Detector.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_dbgmcu.h"

#include <stdlib.h>
#include "GEngineCfg.h"
#include "FLASHROM.h"

int demo_timer;


static u8 R_nRF_Addr[5] = {0x16, 0x35, 0x00, 0x36, 0x03};


static void gSensor_demo(void);
static void Detector_demo(void);


#define SLAVER_APP          0


void demo_main(void)
{
	u32 _msg_buf[C_nRF_CtrlCmdSize/4+1];
	u8 *msg_buf=(u8 *)&_msg_buf;
	msg_buf+=3;
	
	CMOS_Initialization();
	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_128);
	IWDG_SetReload(1565);   // 3.2ms * 1565 = 5.008s
	IWDG_Enable();
	DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE);
	
	nRF_Initialization();
	Display_Initialization();
//	RS_Init(14400);
	Flash_Init();
	File_SetBaseAddr(FLASH_SYSTEM_ADDR);
//	File_SetGraphBaseAddr(FLASH_SYSTEM_ADDR);
	gSensor_Init();
	
//	Commu_Init(Commu_Master);
	pcm_Init();
	
	nRF_SetAddress(R_nRF_Addr);
	nRF_RecieveMode(C_nRF_CtrlCmdSize);
	
	demo_timer = 0;
	
	while (1) {
//		Commu_Main();
		gSensor_Main();
		
		if (demo_timer >= 300) {
			demo_timer = 0;
			gSensor_Analyse();
//			Detector_Analyse();
		}
		
		pcm_checkmsg();
		
		
#if SLAVER_APP

		if (nRF_GetMessage(msg_buf) == TRUE) {
			G_CallFuntion(msg_buf);
			nRF_RecieveMode(C_nRF_CtrlCmdSize);
		}
		
		
#else

		FrushDspBuffer(WHITE);
		gSensor_demo();
		Detector_demo();
		RefreshScreen();
	
#endif

		IWDG_ReloadCounter();
	}
}



#define RADIUS     34
static s16 x = 0, y = 0;
static u16 graph_id = C_Bmp_blackball;
static int wave_count = 0;

static void gSensor_demo(void)
{
	s32 x_angle, y_angle, z_angle;
	enum gWaveRank rank;
	
	gSensor_Analyse();
	rank = gSensor_WaveRank();
	
	if (wave_count > 0)
		wave_count--;
	
	if (rank == g_wave_2) {
		if (wave_count == 0)
			graph_id = C_Bmp_blackball;
	} else if (rank == g_wave_3) {
		graph_id = C_Bmp_redball;
		wave_count = 40;
	} else {
		Get_Box_XYZ(g_z_negative, g_y_negative, gSensor_X_Angle(),
		            gSensor_Y_Angle(), gSensor_Z_Angle(), &x_angle, &y_angle, &z_angle);
					
		x_angle = x_angle / 5;
		x_angle = -x_angle;
		y_angle = y_angle / 5;
		
		x = x + (s16)(x_angle * 1);
		y = y + (s16)(y_angle * 1);
		
		if (x < 0) {
			x = 0;
		} else if (x + RADIUS >= C_LCD_Width) {
			x = C_LCD_Width - RADIUS;
		}
		
		if (y < 0) {
			y = 0;
		} else if (y + RADIUS >= C_LCD_Height) {
			y = C_LCD_Height - RADIUS;
		}
		
		PutGraph(graph_id, x, y);
	}
}

static void Detector_demo(void)
{
	u8 up, down, left, right;
	
//	Detector_Analyse();
	
//	up = Detector_Up_Status();
//	down = Detector_Down_Status();
//	left = Detector_Left_Status();
//	right = Detector_Right_Status();
	
	if ((up == 1) || (down == 1) | (left == 1) | (right == 1)) {
		Motor_On();
		if (pcm_IsPlaying() == 0)
			pcm_Play(C_electric2);
		
		if (up == 1)
			FrushRectangle(GREEN, 24, 0, 80, 20);
		if (down == 1)
			FrushRectangle(GREEN, 24, 108, 80, 20);
		if (left == 1)
			FrushRectangle(GREEN, 0, 24, 20, 80);
		if (right == 1)
			FrushRectangle(GREEN, 108, 24, 20, 80);
			
	} else {
		Motor_Off();
	}
}


