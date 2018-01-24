/****************************************
Author		CM16
Data		2014/07/25
File		CMOS.c
Type		OS_Driver
Description
 - CM Operating System
*****************************************/
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_flash.h"
#include "CMOS.h"
#include "GEngineCfg.h"

#define _CMOS_HTIMERPROC_ 	DISABLE
#define _CMOS_NTIMERPROC_ 	ENABLE

static void CMOS_Timer2Config(void);
static void CMOS_InitReg(void);

volatile u32 r_OS_TimerCount = 0;
volatile u32 AutoPowerOffCnt = 0;
u32 BkAutoPowerOff=0;

/*System Clock Config*/
static void CMOS_SetSystemClock(void);

#ifdef HANGUP
_twdog mdog;
volatile int trace01=0;
void checkdog(void)
{
	if(trace01!=mdog.trace01)
	{
		mdog.trace01=trace01;
		mdog.time=CMOS_GetTimer();
	}else if(CMOS_GetTimerCount(mdog.time)>=5000)
	{
		Console_Open("debug");
		Console_Add_Hex((u8 *)&mdog.trace01,4);
		uDelay(1000*2000);
		MMA7660FC_Init();
		MMA7660FC_Standby();
		poweroff();
		while(1);
	}
}
#endif


/* **************************************** CMOS Function ********************************************* */
/*CMOS Initialization*/
void CMOS_Initialization(void)
{
	CMOS_SetSystemClock();
	__enable_irq();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
#ifdef HANGUP	
	mdog.time=CMOS_GetTimer();
	mdog.trace01=-1;
#endif	
	CMOS_InitReg();
	CMOS_Timer2Config();
	
	
	
}

//static u8 flag = 0;

/*Timer Process*/
void CMOS_TimerProcess(void)
{
	r_OS_TimerCount--;
}

u32 CheckAutoPowerOff()
{
	u32 a=AutoPowerOffCnt;
	u32 b=CMOS_GetTimer();
	
	if(a-b>=60000*3)
		return 1;
	else if(a-b>=10*1000)
		return 2;
	return 0;
}

void ClrAutoPowerOff()
{
	AutoPowerOffCnt=r_OS_TimerCount;
	BkAutoPowerOff=0;
}

/*Set Timer*/
int GEA_SetTimCnt(PTimeCnt in,PCom_Res out)
{
	r_OS_TimerCount=in->timcnt;
	ClrAutoPowerOff();
	return 0;
}

/*ms delay*/
void mDelay(u32 cnt)
{
	u32 time;

	time = r_OS_TimerCount;
	while((time - r_OS_TimerCount) <= cnt);
}

/*us delay*/
void uDelay(u32 cnt)
{
	u32 time;

	while(cnt--)
	{
		time = 6;
		while(time--)
		{
			__asm
			{
				nop
			}
		}
	}
}

/*
*Function:		CMOS_GetmsTimer
*Description:		get ms counter value
*input:			Null
*return:		r_OS_msDelayCnt 
*/
u32 CMOS_GetTimer(void)
{
	return r_OS_TimerCount;
}

/*Get Timer Counter Value*/
u32 CMOS_GetTimerCount(u32 timeStar)
{
	return (timeStar - r_OS_TimerCount);
}

#ifdef _SEND_
#define _TIM_PRESCALER 499
#endif

#ifdef _RECEV_
#define _TIM_PRESCALER 999
#endif

/*������ʱ�жϳ�ʼ��*/
static void CMOS_Timer2Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;						//ͨ��
	Timer2_Priority();
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	//((71 + 1) / 72M) *(999 + 1) = 1ms
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);					//ʹ��ʱ��

	TIM_TimeBaseStructure.TIM_Period = 71; 							//�Զ���װ�ؼĴ�����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = _TIM_PRESCALER; 						//ʱ��Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 			//������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//�������

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2,TIM_FLAG_Update); 								//����ж������־
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2,ENABLE); 
}

/*CMOS ���ݳ�ʼ��*/
static void CMOS_InitReg(void)
{
	r_OS_TimerCount = 0;
}

/*System Clock*/
static void CMOS_SetSystemClock(void)
{
	ErrorStatus HSEStartUpStatus;

	RCC_DeInit();                                            //������RCC�Ĵ�������ΪĬ��ֵ
	RCC_HSEConfig(RCC_HSE_ON);                               //�����ⲿ���پ���HSE
	HSEStartUpStatus = RCC_WaitForHSEStartUp();                //�ȴ�HSE����
	if(HSEStartUpStatus == SUCCESS)
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);  //Ԥȡָ����ʹ��
		FLASH_SetLatency(FLASH_Latency_2);                                     //���ô�����ʱֵ 2 ��ʱ����
		RCC_HCLKConfig(RCC_SYSCLK_Div1);                            //����AHBʱ�ӣ�HCLK��RCC_HCLK_Div1 APBʱ��=ϵͳʱ��
		RCC_PCLK2Config(RCC_HCLK_Div1);                        //���ø���AHBʱ�ӣ�PCLK2��RCC_HCLK_Div1 APBʱ��=HCLK  
		RCC_PCLK1Config(RCC_HCLK_Div2);                        //���õ���AHBʱ�ӣ�PCLK1��RCC_HCLK_Div2 APB1ʱ��=HCLK/2
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);    //����PLLʱ��Դ����Ƶϵ��
		RCC_PLLCmd(ENABLE);                                    //ʹ�ܻ�ʧ��PLL
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)       //�ȴ�ָ����RCC��־λ���óɹ����ȴ�PLL��ʼ���ɹ�
		{
		}
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);             //����ϵͳʱ�䣨SYSCLK��������PLLΪϵͳʱ��Դ
		while(RCC_GetSYSCLKSource() != 0x08)                     //�ȴ�PLL�ɹ�����ϵͳʱ�ӵ�ʱ��Դ
		{                                                                                                                //0x00:HSI��Ϊϵͳʱ��
		}                                                                                                                //0x04:HSE��Ϊϵͳʱ��
	}
	else
	{
		while(1);
	}
}
