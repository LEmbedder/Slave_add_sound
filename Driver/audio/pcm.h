#ifndef __PCM_H
#define __PCM_H	 
#include "stm32f10x.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef s16 PCM_DATA;
#define C_PCM_BUF_SIZE 2048	
#define	C_SILENT	(0x800)

#define C_pcm_playing (1)
#define C_pcm_stop (0)		
	
int pcm_Init(void);
int pcm_checkbuf(void);
void pcm_fillbuf(PCM_DATA *pcm_buf);
void pcm_open(void);	
void pcm_close(void);	
	
#define REALDATAFROM 1	
	
#ifdef __cplusplus
}
#endif


#endif
