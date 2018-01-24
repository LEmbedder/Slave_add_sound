
#include "GEngineCfg.h"
#include "Display.h"
#include "g-sensor.h"
#include "Detector.h"
#include "Motor.h"
#include "pcm.h"
#include "CMOS.h"
#include "Config.h"

int GEA_PutGraph(PGraph_Arg in)
{
	u8 i, sum;	
	sum = in->graphCmd&0x0f;
	for (i = 0; i < sum; i++) {
		AddGraph(in->graphID[i], in->x[i], in->y[i], in->layer[i]);
	}
	
	if ((in->graphCmd&0x80) != 0)
		RefreshGraphs();
	return 0;
}




int GEA_ClearLayer(PLayer_Arg in)
{
	u8 i;
	for (i = 0; i < C_Layer_Max; i++) {
		if (in->layer[i] == 1) {
			ClearGraphLayer(i);
		}
	}
	return 0;
}

int GEA_ClePutRef(PClePutRef_Arg in)
{
	u8 i, sum;	
	sum = in->graphnum;
	for (i = 0; i < sum; i++) {
		ClearGraphLayer(in->layer[i]);
	}
	for (i = 0; i < sum; i++) {
		AddGraph(in->graphID[i], in->x[i], in->y[i], in->layer[i]);
	}
	RefreshGraphs();
	return 0;
}


#define KEY_MIN_ANGLE     25
extern u32 rockcnt;
int GEA_GetJoyStickStatus(PCom_Arg in,PJoystick_Res out)
{
	s8 x, y, z;
	s8 x_abs, y_abs, z_abs;
	u32 _id;
	
	_id=readSlaverID();
	out->slaverID = _id&0x00ff;
	out->key = Key_Null;
	
	if (gSensor_WaveRank() == g_wave_static)
	{
		
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
					out->key = Key_Left;
				else
					out->key = Key_Right;
			}
		} else {
			if ((y_abs >= KEY_MIN_ANGLE) && (z_abs >= KEY_MIN_ANGLE)) {
				if (y > 0)
					out->key = Key_Down;
				else
					out->key = Key_Up;
			}
		}
	}
	out->rock = gSensor_WaveRank();
	if (out->rock == 0)
		out->rock = Rock_Invalid;
	else
		out->rock = out->rock - 1;
	
	out->rockcnt=rockcnt;
	
	out->Surface = gSensor_TopSurface();
	
	return 0;
}
#include "SingleWireSlave.h"
#include "lcd.h"
void resetupdate(void);
void updateend(void);
int GEA_Transport(PCom_Arg in);
int GEA_SetMode(PReserve in)
{
	switch(in->Reserve)
	{
		case Reserve_01:
			SWS_Init();
			break;
		case Reserve_02:
			resetupdate();
			break;
		case Reserve_03:
			updateend();
			break;
		case Reserve_04:
			GEA_Transport((PCom_Arg)in);
			break;
		case Reserve_05:
			Lcd_Off();
			break;
		case Reserve_06:
			Lcd_On();
			break;
		
		default:
			break;
	}
	return 0;
}

extern u8 powerLevel;
u8 play_status(void);
int GEA_SlaverState(PCom_Arg in,PGeneralStatus_Res out)
{
//typedef struct
//{
//	u8 slaverID;
//	u8 powerValue;
//	u8 audioStatus;
//	u8 animationStatus;
//	u8 joystickStatus;
//	u8 locationStatus;
//}GeneralStatus_Res, *PGeneralStatus_Res;
	Com_Res ret;
	
	GEA_GetSpeechStatus(0,&ret);
	out->audioStatus=(u8)ret.result;
	
	out->animationStatus = play_status();
	out->powerValue=powerLevel;
	
	return 0;
}
