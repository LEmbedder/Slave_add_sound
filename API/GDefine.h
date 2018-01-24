/*
 * Filename:	GDefine.h
 * Author:		ZouChao, C.M.16,Z.C.Liang
 * Date:		2015-06-17
 * Version:		v0.1
 * Discription:	nRF api
 */

#ifndef __Game_Define_H_
#define __Game_Define_H_

#include "stm32f10x.h"

#ifdef __cplusplus
extern "C" {
#endif
		
#ifndef Enable
#define Enable			1
#define Disable			0
#endif

#define C_SlaverID_1	0
#define C_SlaverID_2	1
#define C_SlaverID_3	2
#define C_SlaverID_4	3
#define C_SlaverID_Max	2	//20160515	

	
/*Graph*/
#define C_Layer_Max		8
#define C_Layer_1 		(1<<0)
#define C_Layer_2 		(1<<1)
#define C_Layer_3 		(1<<2)
#define C_Layer_4 		(1<<3)
#define C_Layer_5 		(1<<4)
#define C_Layer_6 		(1<<5)
#define C_Layer_7 		(1<<6)
#define C_Layer_8 		(1<<7)	
#define C_Layer_All		(0xff)
	
#define C_HDRLayer_Max		4	
#define C_HDRLayer_1 C_Layer_2
#define C_HDRLayer_2 C_Layer_4
#define C_HDRLayer_3 C_Layer_6
#define C_HDRLayer_4 C_Layer_8	
#define C_HDRLayer_All (C_HDRLayer_1 | C_HDRLayer_2 | C_HDRLayer_3 | C_HDRLayer_4)

/*Timer*/
#define Timer_ID_Error		(u32)(-1)
#define Timer_Type_Loop		1
#define Timer_Type_Once		0

/*Joy*/
typedef enum
{
	Key_Null = 0,
	Key_Up,
	Key_Down,
	Key_Left,
	Key_Right,
	Key_UpLeft,
	Key_UpRight,
	Key_DownLeft,
	Key_DownRight,
	Key_Invalid
}Enum_JoyStick_Key;

typedef enum
{
	Rock_Null = 0,
	Rock_Level1,
	Rock_Level2,
	Rock_Level3,
	Rock_Invalid
}Enum_JoyStick_Rock;

typedef enum{
	Dir_Up = 0,
	Dir_Left,
	Dir_Right,
	Dir_Down,
}Enum_Location_Direction;

//typedef enum{
//	Dir_Right = 0,
//	Dir_Down,
//	Dir_Left,
//	Dir_Up,
//}Enum_Location_Direction;

typedef enum{
	Motor_Stop = 0,
	Motor_SoftShort,
	Motor_SoftLong,
	Motor_StrongShort,
	Motor_StrongLong,
	Motor_TypeMax
}Enum_Motor_Shack;

/*Public Argument*/
typedef struct
{
	u8 slaverID;
}Com_Arg, *PCom_Arg;

typedef struct
{
	u8 slaverID;
	u32 result;
}Com_Res, *PCom_Res;

/*Graph & Animation*/
#define C_GraphMax		4
#define C_GraphRef		(1<<7)
typedef struct
{
	u8 slaverID;
	u8 graphCmd;	//bit7: refresh or not. bit1-bit0: graph sum
	u8 layer[C_GraphMax];
	u16 graphID[C_GraphMax];
	s16 x[C_GraphMax];
	s16 y[C_GraphMax];
}Graph_Arg, *PGraph_Arg;

typedef struct                     // 声音数据传送的结构体
{
	u8 slaverID;
	u16 data;	           
}Gvolume_Arg, *Pvolume_Arg;

typedef struct
{
	u8 slaverID;
	u8 layer[C_Layer_Max];
}Layer_Arg, *PLayer_Arg;

typedef struct
{
	u8 slaverID;
	u8 graphnum;
	u8 layer[C_GraphMax];
	u16 graphID[C_GraphMax];
	s16 x[C_GraphMax];
	s16 y[C_GraphMax];
}ClePutRef_Arg,*PClePutRef_Arg;

typedef struct
{
	u8 slaverID;
	u8 layer;
	u16 animationID;
	s16 x;
	s16 y;
}Animation_Arg, *PAnimation_Arg;

typedef struct
{
	u8 slaverID;
	u8 layers;
}HDRLayer_Arg, *PHDRLayer_Arg;

//=====================Audio========================
#define C_SpeechListMax		8u 
#define C_SpeechList_End	0xffff
#define C_PlayList				0xffff

#define	C_SpeechEnd		(1<<0)				//0--not end,1--end	
#define	C_SpeechPause	(1<<1)				//0--not pause,1--pause	
#define	C_SpeechPauseNextVoice	(1<<2)		//0--not pause,1--pause	

#define C_StopBGMIndex (0xffff)

typedef struct
{
	u8 slaverID;
	u16 voiceID[C_SpeechListMax];
}SpeechList_Arg, *PSpeechList_Arg;

typedef struct
{
	u8 slaverID;
	u16 voiceID;
}Speech_Arg, *PSpeech_Arg;

//===================Joystick======================
enum BoxSurface {
	Box_unknown = 0,
	Box_up,
	Box_down,
	Box_left,
	Box_right,
	Box_front,
	Box_back
};

typedef struct
{
	u8 slaverID;
	u8 key;
	u8 rock;
	u8 Surface;	//add by 2016/03/11
	u32 rockcnt;
}Joystick_Res, *PJoystick_Res;


//===================Location======================
#define C_Loc_Nei_Far			0
#define C_Loc_Nei_Near			1
#define C_Loc_Nei_Invalid		2
typedef struct
{
	u8 slaverID;
	u8 dir[4];
	u32 time[4];
	u32 systime;
}Location_Res, *PLocation_Res;


//====================Motor========================
typedef struct
{
	u8 slaverID;
	u8 mode;
}Motor_Arg, *PMotor_Arg;


//===================General Status================
typedef enum{
	BAT_Empty = 0,
	BAT_Low,
	BAT_Middle,
	BAT_Full,
	BAT_Max
}Enum_BAT_Status;

#define C_Status_Pwr_Low		(1<<0)
#define C_Status_Voice			(1<<1)
#define C_Status_Animation		(1<<2)
typedef struct
{
	u8 slaverID;
	u8 powerValue;	//empty 0,1,2,3 full
	u8 audioStatus;
	u8 animationStatus;
	u8 joystickStatus;
	u8 locationStatus;
}GeneralStatus_Res, *PGeneralStatus_Res;

typedef struct
{
	u8 slaverID;
	u32 timcnt;
}TimeCnt, *PTimeCnt;

typedef enum{
	Reserve_01=0x03000003,
	Reserve_02=0x05000005,
	Reserve_03=0x0a00000a,
	Reserve_04=0x0f00000f,
	Reserve_05=0x08000008,
	Reserve_06=0x09000009,
	
}Enum_Reserve;
typedef struct
{
	u8 slaverID;
	u32 Reserve;
}Reserve, *PReserve;

typedef struct
{
	u8 slaverID;
	u8 gname[16];
}GName, *PGName;

//===================General Status================
typedef enum
{
	FAULT_LowPower,
	FAULT_GameLock,
	FAULT_LossSlaver,
}Enum_FAULT;
#ifdef __cplusplus
}
#endif

#endif /*__Game_Define_H_*/



