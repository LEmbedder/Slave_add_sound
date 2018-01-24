/*
 * Filename:	GEngineCfg.h
 * Author:		B.Q.Li, C.M.16,Z.C.Liang
 * Date:		2015-09-17
 * Version:		v0.1
 * Discription:	Game Engine
 */

#include "GEngineCfg.h"

//must be same as Enum_GameMessage
const Struct_Msg_APIInfo TB_GE_MsgApiInfo[]= 
{
	{
		.function = GEA_PutGraph,			//Enum_GE_Msg_PutGraph
		.inputSize = sizeof(Graph_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_ClearLayer,			//Enum_GE_Msg_ClearLayer,
		.inputSize = sizeof(Layer_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_ClePutRef,			//Enum_GE_Msg_ClePutRef,
		.inputSize = sizeof(ClePutRef_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	
	{
		.function = GEA_PlayAnimation,			//Enum_GE_Msg_PlayAnimation,
		.inputSize = sizeof(Animation_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_AddAnimation,			//Enum_GE_Msg_AddAnimation,
		.inputSize = sizeof(Animation_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_DeleteAnimation,			//Enum_GE_Msg_DeleteAnimation,
		.inputSize = sizeof(HDRLayer_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_ClearAnimationFifo,			//Enum_GE_Msg_ClearAnimationFifo,
		.inputSize = sizeof(HDRLayer_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_StopAnimation,			//Enum_GE_Msg_StopAnimation,
		.inputSize = sizeof(HDRLayer_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_SetSpeechList,			//Enum_GE_Msg_SetSpeechList,
		.inputSize = sizeof(SpeechList_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_PlaySpeech,			//Enum_GE_Msg_PlaySpeech,
		.inputSize = sizeof(Speech_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_LoopSpeech,			//Enum_GE_Msg_LoopSpeech,
		.inputSize = sizeof(Speech_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	
	{
		.function = GEA_GetSpeechStatus,			//Enum_GE_Msg_GetSpeechStatus,	
		.inputSize = sizeof(Com_Arg)/sizeof(u8),
		.outputSize = sizeof(Com_Res)/sizeof(u8),
	},
	{
		.function = GEA_StopSpeech,			//Enum_GE_Msg_StopSpeech,
		.inputSize = sizeof(Com_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_GetJoyStickStatus,			//Enum_GE_Msg_Joystick,
		.inputSize = sizeof(Com_Arg)/sizeof(u8),
		.outputSize = sizeof(Joystick_Res)/sizeof(u8),
	},
	{
		.function = GEA_GetNearStatus,			//Enum_GE_Msg_Location,
		.inputSize = sizeof(Com_Arg)/sizeof(u8),
		.outputSize = sizeof(Location_Res)/sizeof(u8),
	},
	{
		.function = GEA_MotorCtrl,			//Enum_GE_Msg_MotorCtrl,
		.inputSize = sizeof(Motor_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_SlaverState,			//Enum_GE_Msg_SlaverState,
		.inputSize = sizeof(Com_Arg)/sizeof(u8),
		.outputSize = sizeof(GeneralStatus_Res)/sizeof(u8),
	},
	
	{
		.function = GEA_SetTimCnt,			//Enum_GE_Msg_SetTimCnt,
		.inputSize = sizeof(TimeCnt)/sizeof(u8),
		.outputSize = sizeof(Com_Res)/sizeof(u8),
	},
	{
		.function = GEA_PowerOff,			//Enum_GE_Msg_PowerOff,
		.inputSize = sizeof(Com_Arg)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_SetMode,			//Enum_GE_Msg_SetMode,
		.inputSize = sizeof(Reserve)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_SetGName,			//Enum_GE_Msg_SetGname,
		.inputSize = sizeof(PGName)/sizeof(u8),
		.outputSize = 0,
	},
	{
		.function = GEA_GetGName,			//Enum_GE_Msg_GetGname,
		.inputSize = sizeof(Com_Arg)/sizeof(u8),
		.outputSize = sizeof(PGName)/sizeof(u8),
	},
	{
	  .function = GEA_Setvolume,			//Enum_GE_Msg_PutGraph
		.inputSize = sizeof(Gvolume_Arg)/sizeof(u8),
		.outputSize = 0,
	},
};




