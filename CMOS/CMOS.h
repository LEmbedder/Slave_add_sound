/****************************************
Author		CM16
Data		2014/07/25
File		CMOS.h
Type		OS_Driver
Description
 - CM Operating System
*****************************************/
#ifndef __CMOS_H_
#define __CMOS_H_

#include "stm32f10x.h"

//#define PCM_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;}
//#define USART1_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;}
//#define USART3_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;}
//#define Timer2_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;}
//#define NFR_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;}
//#define SWS_Priority(){NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;}
//#define MMA7660FC_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;}



#define PCM_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;}
#define SWS_Priority(){NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;}

#define Timer2_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;}

#define MMA7660FC_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;}
#define USART3_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;}
#define NFR_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;}
#define DMA_Priority() {NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;}

/*CMOS Initialization*/
void CMOS_Initialization(void);
/*Timer Process*/
void CMOS_TimerProcess(void);


/*ms delay*/
void mDelay(u32 cnt);
/*us delay*/
void uDelay(u32 cnt);

u32 CMOS_GetTimer(void);
u32 CMOS_GetTimerCount(u32 timeStar);

u8 GetIDIndex(void);
void ClrAutoPowerOff(void);
u32 CheckAutoPowerOff(void);
extern void SystemReset(void);

void SPIREAD(u8 *buf,int size);

//#define HANGUP 0
#ifdef HANGUP
typedef struct mywdogtag
{
	u32 time;
	int trace01;
	int trace02;
}_twdog;
void checkdog(void);

#endif

#ifdef __DEBUG__  
#include <stdio.h>
#define DEBUG(format,...) printf("File: "__FILE__", Line: %05d: "format"\r\n", __LINE__, ##__VA_ARGS__)  
#else  
#define DEBUG(format,...)  

#endif  

#endif
