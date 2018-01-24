#ifndef __HDR_H
#define __HDR_H	 
#include "stm32f10x.h"


#ifdef __cplusplus
extern "C" {
#endif
		
void hdr_Init(void);
void play_check(void);
u8 play_status(void);
	
#ifdef __cplusplus
}
#endif

#endif
