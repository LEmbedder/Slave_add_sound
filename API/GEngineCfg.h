/*
 * Filename:	GEngineCfg.h
 * Author:		B.Q.Li, C.M.16, Z.C.Liang
 * Date:		2015-09-17
 * Version:		v0.1
 * Discription:	Game Engine
 */
#ifndef __GAME_ENGINE_CONFIG_H_
#define __GAME_ENGINE_CONFIG_H_

#include "GDefine.h"

typedef struct
{
	void *function;
	u8 inputSize;
	u8 outputSize;
}Struct_Msg_APIInfo;

extern const Struct_Msg_APIInfo TB_GE_MsgApiInfo[];

typedef enum
{
	Enum_GE_Msg_PutGraph = 0,
	Enum_GE_Msg_ClearLayer,
	Enum_GE_Msg_ClePutRef,
	
	Enum_GE_Msg_PlayAnimation,
	Enum_GE_Msg_AddAnimation,
	Enum_GE_Msg_DeleteAnimation,
	Enum_GE_Msg_ClearAnimationFifo,
	Enum_GE_Msg_StopAnimation,
	
	Enum_GE_Msg_SetSpeechList,
	Enum_GE_Msg_PlaySpeech,
	Enum_GE_Msg_LoopSpeech,
	Enum_GE_Msg_GetSpeechStatus,
	Enum_GE_Msg_StopSpeech,
	
	Enum_GE_Msg_Joystick,
	Enum_GE_Msg_Location,
	
	Enum_GE_Msg_MotorCtrl,
	
	Enum_GE_Msg_SlaverState,
	
	Enum_GE_Msg_SetTimCnt,
	
	Enum_GE_Msg_PowerOff,
	
	Enum_GE_Msg_SetMode,
	
	Enum_GE_Msg_SetGname,
	Enum_GE_Msg_GetGname,
	
	Enum_GE_Msg_Setvolume,
	Enum_GE_Msg_Sum
}Enum_GE_Msg_List;

//typedef struct statetag
//{
//	u8 slaveid;
//}SLAVERSTATE, *PSLAVERSTATE;

//==============================Graph====================================
extern int GEA_PutGraph(PGraph_Arg in);
extern int GEA_ClearLayer(PLayer_Arg in);
extern int GEA_ClePutRef(PClePutRef_Arg in);

//============================Animation==================================
extern int GEA_PlayAnimation(PAnimation_Arg args);
extern int GEA_AddAnimation(PAnimation_Arg args);
extern int GEA_DeleteAnimation(PHDRLayer_Arg args);
extern int GEA_ClearAnimationFifo(PHDRLayer_Arg args);
extern int GEA_StopAnimation(PHDRLayer_Arg args);


//===============================Audio===================================
extern int GEA_SetSpeechList(PSpeechList_Arg in);
extern int GEA_PlaySpeech(PSpeech_Arg in);
extern int GEA_LoopSpeech(PSpeech_Arg in);
extern int	GEA_GetSpeechStatus(PCom_Arg in,PCom_Res out);
extern int GEA_StopSpeech(PCom_Arg in);

//==============================Joystick=================================
extern int GEA_GetJoyStickStatus(PCom_Arg in,PJoystick_Res out);


//=============================Location==================================
extern int GEA_GetNearStatus(PCom_Arg in,PLocation_Res out);

//=============================Motor=====================================
extern int GEA_MotorCtrl(PMotor_Arg args);

//============================General Status=============================
extern int GEA_SlaverState(PCom_Arg in,PGeneralStatus_Res out);
extern int GEA_SetTimCnt(PTimeCnt in,PCom_Res out);

extern int GEA_PowerOff(PCom_Arg in);

extern int GEA_SetMode(PReserve in);

extern int GEA_SetGName(PGName in);
extern int GEA_GetGName(PCom_Arg in,PGName out);

extern int GEA_Setvolume(Pvolume_Arg in);
#endif /*__GAME_ENGINE_CONFIG_H_*/



