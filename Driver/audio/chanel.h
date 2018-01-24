/*
chanel
*/
#ifndef __CHANEL_H
#define __CHANEL_H	 
#include "stm32f10x.h"
#include "pcm.h"
#include "adpcm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define C_pcm_bufsize 512

typedef struct chanel_tag
{
	u32 dataAddr;
	u32 backAddr;
	u16 flags;
	PCM_DATA buffer[C_pcm_bufsize*2];
	u32 filelength;
	u32 backlength;
	adpcm_state_t state;
	adpcm_state_t backstate;
}chanelData,*pchanelData;	
	
typedef s8 PCM_CODE_DATA;	
	
#define B_chanel_RUD (1<<0)	//rupdown
#define B_chanel_buf1 (1<<1)	//1:buf1 filled 0:invalid
#define B_chanel_buf2 (1<<2)	//1:buf2 filled 0:invalid
#define B_chanel_whichbuf (1<<3)	//0:buf1  1:buf2
#define B_chanel_IsPlaying (1<<4)	//0:end 1:playing	
#define B_chanel_loop (1<<5)	//0:not loop 1:loop		

int chanel_fillbuf(pchanelData chanel);
	
#ifdef __cplusplus
}
#endif


#endif
