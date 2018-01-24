
#ifndef __RECEV_H_
#define __RECEV_H_

#include "stm32f10x.h" //���ͷ�ļ�����STM32F10x������Χ�Ĵ�����λ���ڴ�ӳ��Ķ���

#define N_sample 147 //ÿͨ���� (33*0.5ms)/112us ��   (1/9M)*(12.5+239.5)*4
#define M_chanel 4 //Ϊ4��ͨ��
#define thresholdDefault (0x400)
#define hthrDefault (0x600)

typedef struct _recevtag{
	u16 threshold;	//�ߵ�����
	u16 hthr[M_chanel*M_chanel];	//����Ч����
	u8 dir[M_chanel];
	vu16 AD_Value[N_sample*M_chanel]; //�������ADCת�������Ҳ��DMA��Ŀ���ַ
	vu16 DMABUF[N_sample*M_chanel]; //�������ADCת�������Ҳ��DMA��Ŀ���ַ
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
