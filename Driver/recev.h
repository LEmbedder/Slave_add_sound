
#ifndef __RECEV_H_
#define __RECEV_H_

#include "stm32f10x.h" //这个头文件包括STM32F10x所有外围寄存器、位、内存映射的定义

#define N_sample 147 //每通道采 (33*0.5ms)/112us 次   (1/9M)*(12.5+239.5)*4
#define M_chanel 4 //为4个通道
#define thresholdDefault (0x400)
#define hthrDefault (0x600)

typedef struct _recevtag{
	u16 threshold;	//高低门限
	u16 hthr[M_chanel*M_chanel];	//高有效门限
	u8 dir[M_chanel];
	vu16 AD_Value[N_sample*M_chanel]; //用来存放ADC转换结果，也是DMA的目标地址
	vu16 DMABUF[N_sample*M_chanel]; //用来存放ADC转换结果，也是DMA的目标地址
	u8 windex;
	u8 rindex;
	u8 writing;
}recev,*Precev;

typedef struct _Hholdtag{
	u8 start;
	u8 end;
	u8 length;
}Hhold,*pHhold;

int recev_Init(void);
void recevcheck(void);
#endif /*__IC_SPI_H_*/
