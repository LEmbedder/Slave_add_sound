#include "stm32f10x.h"
#include "TestMode.h"
#include "CMOS.h"
#include "NRF24L01.h"  
#include "lcd.h"
#include "Display.h"
#include "sentence.h"
#include "ADC.h"
#include "IC_SPI.h"
#include "Commu.h"
#include "DAC.h"
#include "Flash.h"
#include "File.h"
#include "demo.h"
#include "GEngineCfg.h"
#include "g-sensor.h"
#include "nRF.h"
#include "stm32f10x_pwr.h"
#include <string.h>
#include "FLASHROM.h"
#include "Motor.h"
#include "xiaomu.h"

#define slaver_prj			1
#define slaverDebug_prj		2
#define	shownCool_prj		3

#define PrjType         slaver_prj

#if PrjType==slaverDebug_prj
#include "Config.h"
#include "hdr.h"
void slaverDebug_main()
{
	int i=0;
	s16 x=0;
	Com_Res ret;
	Speech_Arg speech;
	Animation_Arg ani;
	Motor_Arg motor;
	HDRLayer_Arg layer;
	Location_Res location;
	u32 tcnt;
	GeneralStatus_Res status;
	Joystick_Res joy;
	
	
	
	Graph_Arg graph;
	Layer_Arg gplayer;
	
	CMOS_Initialization();
	
	
	Display_Initialization();
	
	nRF_Initialization();
	i=nRF_updateAddr();	
//	if(1==i)
//		hardware_main();//enter test mode
	//这里配置 TX_ADDR,RX_ADDR_P0及其他
	nRF_InitReg();
	nRF_EXTIcfg();
	nRF_RecieveMode();
	
	
	Flash_Init();
	File_SetBaseAddr(FLASH_SYSTEM_ADDR,FLASH_GAME_ADDR);
	sndInit();
	
	InitConfig();
//	nRF_Initialization();
//	nRF_updateAddr();
	
	Motor_Init();
	
	hdr_Init();
	
	gSensor_Init();
	
//	typedef struct
//{
//	u8 slaverID;
//	u8 graphCmd;	//bit7: refresh or not. bit1-bit0: graph sum
//	u8 layer[C_GraphMax];
//	u16 graphID[C_GraphMax];
//	s16 x[C_GraphMax];
//	s16 y[C_GraphMax];
//}Graph_Arg, *PGraph_Arg;
	
	graph.graphID[0]=2;
	graph.graphCmd=0x81;
	graph.layer[0]=4;
	graph.x[0]=0;
	graph.y[0]=0;
	
	GEA_PutGraph(&graph);
	
	RefreshGraphs();
	
//	typedef struct
//{
//	u8 slaverID;
//	u8 layer[C_Layer_Max];
//}Layer_Arg, *PLayer_Arg;

	gplayer.layer[0]=1;
	gplayer.layer[1]=1;
	gplayer.layer[2]=1;
	gplayer.layer[3]=1;
	gplayer.layer[4]=1;
	gplayer.layer[5]=1;
	gplayer.layer[6]=1;
	gplayer.layer[7]=1;
	
	GEA_ClearLayer(&gplayer);
	
	RefreshGraphs();
////	
////	PutGraph(2, 0, 0);
////	RefreshScreen();
////	
	speech.voiceID=0x8000;	//0x8026;
//	GEA_LoopSpeech(&speech);
	speech.voiceID=0x8001;	//0x801E;
////	
////	ani.animationID=0;
////	ani.layer=C_HDRLayer_1;

//////	GEA_PlayAnimation(&ani);

////	GEA_AddAnimation(&ani);
	
	ani.animationID=0x000;
	ani.layer=C_HDRLayer_2;
	GEA_AddAnimation(&ani);
	
	layer.layers=C_HDRLayer_2;
//	GEA_ClearAnimationFifo(&layer);
	
	motor.mode=1;
	GEA_MotorCtrl(&motor);
	
	tcnt=CMOS_GetTimer();
	i=0x8045;
	while(1)
	{		
		play_check();
		Display_CheckUpdate();
		
		IWDG_ReloadCounter();
		stCheck();

		
		GEA_SlaverState(0,&status);
		
//		if(0==status.animationStatus)
//		{
////			GEA_LoopSpeech(&speech);
//			
//			GEA_GetJoyStickStatus(0,&joy);
//			PutNum(0,0,joy.rockcnt);
//			RefreshScreen();
//			
//			mDelay(1000);			
//			
//			GEA_AddAnimation(&ani);
//			play_check();
//			Display_CheckUpdate();
//		}
		
//		GEA_GetSpeechStatus(0,&ret);
//		if(SND_STATUS_STOP==ret.result)
//		
//		if(tcnt-CMOS_GetTimer()>=10000)
//		{
//			GEA_DeleteAnimation(&layer);
//			RefreshGraphs();
//		}

//		NearSensorServer();
//		
//		GEA_GetNearStatus(0,&location);
//		if(1==location.dir[2])
//		{
//			GEA_GetSpeechStatus(0,&ret);
//			if(C_SpeechEnd==ret.result)
//			{
//				GEA_PlaySpeech(&speech);
////				GEA_MotorCtrl(&motor);
//			}
//	{
//		u8 output[32];
//		SendMessage((u8 *)output,32);
//	}
//		}		
		
		GEA_ClearLayer(&gplayer);
		
		graph.graphID[0]=i;
		graph.graphCmd=0x81;
		graph.layer[0]=4;
		graph.x[0]=0;
		graph.y[0]=0;
	
		GEA_PutGraph(&graph);		
		RefreshGraphs();	
		mDelay(600); 
		
		graph.graphID[1]=0xfffe;
		graph.layer[1]=4;
		graph.graphCmd=0x82;	//RGB2Y();
		GEA_PutGraph(&graph);		
		RefreshGraphs();		//RefreshScreen();
		mDelay(600);
		
		i++;
		if(i>32986)
			i=0x8000;
		
	}
}
#endif

int main(void)
{
	SCB->VTOR = FLASH_BASE | 0x004000;
	
#if PrjType==RS232Tool_prj 
	DownloadData();
#elif PrjType==slaver_prj
	slaver_main();
#elif PrjType==slaverDebug_prj
	slaverDebug_main(); 
#elif PrjType==shownCool_prj
	demo_main();
#endif	

	return 0;
}		
	

