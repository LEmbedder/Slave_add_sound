/*
mix
*/
#ifndef __MIX_H
#define __MIX_H	 
#include "stm32f10x.h"
#include "pcm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mix_tag
{
	PCM_DATA mix_buf[2][C_PCM_BUF_SIZE];
	int index;
}mix_data,pmix_data;	

void mixchanel(PCM_DATA* mixbuf,PCM_DATA* chanel1,int length1,PCM_DATA* chanel2,int length2);

#ifdef __cplusplus
}
#endif
#endif
