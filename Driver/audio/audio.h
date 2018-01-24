/*
audio
*/
#ifndef __AUDIO_H
#define __AUDIO_H	 
#include "stm32f10x.h"

#ifdef __cplusplus
extern "C" {
#endif
	
typedef struct
{
	u32 segoff;
}PLHead;
	
	
void audio_Init(void);	
void audio_checkmsg(void);	
void pcm_Play(u16 index);
void pcm_checkmsg(void);
u16 IsVoiceBusy(void);
void PlayVoice(u16 index);
void StopVoice(void);	
//void pcm_SetLoop(void);		
int playbgm(u16 index);
void StopBGM(void);
	
#ifdef __cplusplus
}
#endif
	
#endif


