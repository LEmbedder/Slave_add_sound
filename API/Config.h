#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "stm32f10x.h"

#define C_ProISOK	0xaa5555aa

typedef struct configtag
{
	u32 addr2_4GL;
	u32	addr2_4GH;	
	
	u32 ID;
	u32 proIsok;	//0xaa5555aa is ok.
	
	u32 checksum;
	u32 aaaa;
	u32 x5555;
}CONFIG,*PCONFIG;

//static function

//extern function
int nRF_updateAddr(void);
void InitConfig(void);
u8 nRF_getAddr(u8 *_addr);
u32 readSlaverID(void);
void WriteSlaverID(u32 _id);

void Resetproisok(void);
void Writegsensorsegoff(s32 x,s32 y,s32 z);
void Readgsensorsegoff(s32 *x,s32 *y,s32 *z);
u32 readSlaverID(void);
u8 nRF_setAddr(u8 *_addr);
u32 readSlaverID(void);
void setgname(u8 *name);
void getgname(u8 *name);
PCONFIG getConfig(void);
u32 writeconfig(PCONFIG conf);

#endif
