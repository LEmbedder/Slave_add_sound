/*
 * Filename:	DAC.h
 * Author:		ZouChao, C.M.16
 * Date:		2015-07-15
 * Version:		v0.2
 * Discription:	dac driver
 */

#include "DAC.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_dac.h"

/*DAC Config*/
static void DAC_Reg_Config(void);

/************************************** Function ***********************************************/
/*DAC Initialization*/
void DAC_Initialization(void)
{
	DAC_Reg_Config();
}

/*DAC Output*/
void DAC_OutPut(u16 Data)
{
	DAC_SetChannel1Data(DAC_Align_8b_R, Data);
}

/*DAC Config*/
static void DAC_Reg_Config(void)
{
	DAC_InitTypeDef DAC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None; 
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None; 
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; 
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	
	DAC_Cmd(DAC_Channel_1, ENABLE); 
}

