
#include "Motor.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "misc.h"
#include "CMOS.h"
#include "GEngineCfg.h"

static u8 MotorFlag;
static u32 motor_timer = 1000;

u8 getMotorFlag()
{
	return MotorFlag;
}

void Motor_On(void)
{
	MotorFlag=1;
	GPIOB->BSRR = GPIO_Pin_11;
}

void Motor_Off(void)
{
	MotorFlag=0;
	GPIOB->BRR = GPIO_Pin_11;
}

void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	Motor_Off();
}

void Motor_Action(enum Motor_Type type)
{
	switch (type) {
	case motor_on_1s:
		motor_timer = 1000;
		Motor_On();
		break;
		
	default:;
	}
}

void Motor_Check(void)
{
	if (motor_timer != 0) {
		motor_timer--;
	}else
	{	
		Motor_Off();
	}
}

int GEA_MotorCtrl(PMotor_Arg args)
{
	if (Motor_Stop==args->mode) {
		Motor_Off();
	}
	else if(args->mode<Motor_TypeMax) {
		switch(args->mode)
		{	
			case Motor_SoftShort:
				motor_timer=600;
				break;	
			default:
				motor_timer=1200;
				break;
		}
		Motor_On();
	}
	return 0;
}


