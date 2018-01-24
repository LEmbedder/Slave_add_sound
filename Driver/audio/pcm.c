#include "pcm.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dac.h"
#include "misc.h"
#include "CMOS.h"

#define CR_CLEAR_Mask              ((unsigned int)0x00000FFE)
#define SWTRIGR_SWTRIG_Set         ((unsigned int)0x00000001)
#define CR_EN_Set                  ((unsigned int)0x00000001)

static PCM_DATA *pcm_buf1;
static PCM_DATA *pcm_buf2;
static PCM_DATA *pcm_bufp;
static short pcm_index;	//if pcm_index>=0 use pcm_buf1,else use pcm_buf2.
static int pcm_isopen;

static void pcm_Init_Shutdown_Pin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIOB->BSRR = GPIO_Pin_10;
}

int pcm_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//--------H init
	unsigned int tmpreg1=0,tmpreg2=0;
	NVIC_InitTypeDef NVIC_InitStructure;  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
	
	pcm_Init_Shutdown_Pin();
	
	/* Enable peripheral clocks ------------------------------------------------*/
	/* GPIOA Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* DAC Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	//POA4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	tmpreg1=DAC->CR;//Get the DAC CR value  
	tmpreg1&=~(CR_CLEAR_Mask<<DAC_Channel_1);//Clear BOFFx, TENx, TSELx, WAVEx and MAMPx bits  
	tmpreg2=(DAC_Trigger_None|DAC_WaveGeneration_None|DAC_LFSRUnmask_Bit0|DAC_OutputBuffer_Enable);
	tmpreg1|=tmpreg2<<DAC_Channel_1;//Calculate CR register value depending on DAC_Channel 
	DAC->CR=tmpreg1;//Write to DAC CR 
	DAC->CR|=CR_EN_Set<<DAC_Channel_1;//DAC Channel1使能,PA4自动连接到DAC

	DAC->DHR12R1=0x00;//通道1的12位右对齐数据	
	DAC->SWTRIGR|=0x01;//软件启动两个通道的转换
		
	//TIM3 10KHZ  
	TIM_DeInit(TIM3);                                          
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);       
	TIM_TimeBaseStructure.TIM_Period = 100;                    
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;                       
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;             
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);   
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);                 

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	PCM_Priority();  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure); 
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);               /* Enable TIM3  interrupt TIM3溢出中断允许*/ 

	//--------S init
	pcm_buf1=0;
	pcm_buf2=0;
	pcm_bufp=0;
	pcm_index=0;
	pcm_isopen=0;
	return 0;
}


int pcm_checkbuf(void)
{
	if((0==pcm_buf1)||(0==pcm_buf2))
		return 1;
	return 0;
}

void pcm_fillbuf(PCM_DATA *pcm_buf)
{
	if(0!=pcm_buf)
	{
		if(0==pcm_buf1)
			pcm_buf1=pcm_buf;
		else if(0==pcm_buf2)
			pcm_buf2=pcm_buf;
		
		if(0==pcm_bufp)
			pcm_bufp=pcm_buf1;
	}
}

int pcm_getstatus()
{
	return pcm_isopen;
}

void pcm_open()
{
	if(C_pcm_playing==pcm_isopen)
		return;
	if((0==pcm_buf1)&&(0==pcm_buf2))
		return;
//	DAC->DHR12R1=C_SILENT;
	GPIOB->BRR = GPIO_Pin_10;//...openspeak
	
	TIM3->CR1 &= ~TIM_CR1_CEN;	/*关闭Timer2计数*/  
	TIM3->SR&=~(1<<0);//清除中断标志位 	
	TIM3->CR1 |= TIM_CR1_CEN;	/*开启Timer2计数*/
	pcm_isopen=C_pcm_playing;
}

void pcm_close()
{
//	GPIOB->BRR = GPIO_Pin_10;//...openspeak
	GPIOB->BSRR = GPIO_Pin_10;//...close speak
	TIM3->CR1 &= ~TIM_CR1_CEN;	/*关闭Timer3计数*/ 
//	DAC->DHR12R1=0x0000;	//通道1的12位右对齐数据
//	DAC->DHR12R1=0x800;	//通道1的12位右对齐数据
	DAC->SWTRIGR|=0x01;//软件启动两个通道的转换	
	pcm_isopen=C_pcm_stop;
}

static u16 testdata=0;

void TIM3_IRQHandler(void)  
{  
	PCM_DATA data;
	s16 _data=0x800;
	if(TIM3->SR&0X0001)//溢出中断
	{
		if(0!=pcm_bufp)
		{
			data=*(pcm_bufp+pcm_index);
			#if 0
			if(data>=0)
				data+=32;
			else
				data-=32; 
			data=data/64;
			#else
			if(data>=0)
				data+=8;
			else
				data-=8;
			data=data/16;
			#endif
			if(data>=0x800)
				data=0x7ff;
			if(data<-0x800)
				data=-0x800;
			
			_data+=data;
			
			_data&=0x0fff;
		}else
		{
			_data=C_SILENT;
		}
//		DAC->DHR12R1=(u16)(_data);	
		DAC->DHR12R1=(u16)(_data*0.2);
		
//		DAC->DHR12R1=testdata;
//		testdata+=50;
//		if(testdata>=0xfff)
//			testdata=0;
		
		
//		DAC->SWTRIGR|=0x01;
		if((0x00==pcm_buf1)&&(0x00==pcm_buf2))																																										
			pcm_close();
		pcm_index++;
		if(pcm_index>=C_PCM_BUF_SIZE)
		{
			if(pcm_bufp==pcm_buf1)
			{
				pcm_buf1=0;
				pcm_bufp=pcm_buf2;
			}
			else
			{
				pcm_buf2=0;
				pcm_bufp=pcm_buf1;
			}
			pcm_index=0;
		}
		TIM3->SR&=~(1<<0);//清除中断标志位 	
	}
}  
//void TIM3_IRQHandler(void)  
//{  
//	PCM_DATA _data;
//	u16 data;
//	if(TIM3->SR&0X0001)//溢出中断
//	{
//		data=(u8)*(pcm_bufp+pcm_index);
//		data=(u16)((data << 4 | data>>4)*0.3);
//		data&=0x0fff;
//		DAC->DHR12R1=data;	//通道1的12位右对齐数据
//		DAC->SWTRIGR|=0x01;//软件启动两个通道的转换
//		if((0x00==pcm_buf1)&&(0x00==pcm_buf2))
//			pcm_close();
//		pcm_index++;
//		if(pcm_index>=C_PCM_BUF_SIZE)
//		{
//			if(pcm_bufp==pcm_buf1)
//			{
//				pcm_buf1=0;
//				pcm_bufp=pcm_buf2;
//			}
//			else
//			{
//				pcm_buf2=0;
//				pcm_bufp=pcm_buf1;
//			}
//			pcm_index=0;
//		}
//		if(0==pcm_bufp)
//			while(1)
//				;
//		TIM3->SR&=~(1<<0);//清除中断标志位 	
//	}
//}  
