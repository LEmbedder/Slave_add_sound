/*
decode
*/
#ifndef __DECODE_H
#define __DECODE_H	 
#include "stm32f10x.h"
#include "adpcm.h"
#include "chanel.h"

#ifdef __cplusplus
extern "C" {
#endif
	
int decode(PCM_CODE_DATA* cdata,int num, PCM_DATA* pcm,pchanelData chanel);	
	
#ifdef __cplusplus
}
#endif

#endif
