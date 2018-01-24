
#include "Battery.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "misc.h"
#include "CMOS.h"


void Bat_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//20161209	GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//20161209	GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 	 //禁止扫描模式，扫描模式用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//开启连续转换模式，即不停地进行ADC转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	 	//要转换的通道数目1
	ADC_Init(ADC2, &ADC_InitStructure);
	
	/*配置ADC时钟，为PCLK2的6分频，即12MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
	/*配置ADC2的通道10为55.	5个采样周期，序列为1 */ 
	ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);
	
	/* Enable ADC2 */
	ADC_Cmd(ADC2, ENABLE);
	
	/*复位校准寄存器 */   
	ADC_ResetCalibration(ADC2);
	/*等待校准寄存器复位完成 */
	while(ADC_GetResetCalibrationStatus(ADC2));
	
	/* ADC校准 */
	ADC_StartCalibration(ADC2);
	/* 等待校准完成*/
	while(ADC_GetCalibrationStatus(ADC2));
	
	/* 由于没有采用外部触发，所以使用软件触发ADC转换 */ 
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);
}

u8 Bat_GetChargeState(void)
{
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0)
		return 1;
	else
		return 0;
}

u8 Bat_GetStandbyState(void)
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0)
		return 1;
	else
		return 0;
}

u16 Bat_GetAD()
{
	return ADC_GetConversionValue(ADC2);
}

u8 Bat_GetLevel()
{
	u16 value=Bat_GetAD();
	if(value<=C_POWER_LOW)
		return 0;
	else if(value<=C_POWER_WAR)
		return 1;
	return 2;
}

float Bat_GetVoltage(void)
{
	float voltage;
	u16 val;
	
	val = ADC_GetConversionValue(ADC2);
	
	voltage = (float)val * 2.00 * 3.30 / 4095.0;
	
	return voltage;
}

u8 Bat_GetVoltagePercentage(void)
{
	float voltage;
	u8 percent;
	int tmp;
	
	voltage = Bat_GetVoltage();
	voltage = 4.2 * voltage / 3.7;
	
	if (Bat_IsChargeDone()) {
		percent = 100;
	} else if (voltage < 3.5) {
		percent = 0;
	} else {
		if (voltage > 4.2)
			voltage = 4.1;
			
		tmp = (int)(voltage * 100);
		tmp = (tmp + 5) / 10;
		voltage = (float)tmp;
		voltage = voltage / 10;

		percent = (u8)(((voltage - 3.5) * 100.0 / 0.7));    // (voltage - 3.50) * 100.00 / (4.20 - 3.50)
	}
	
	return percent;	
}

u8 Bat_IsChargeDone(void)
{
	u8 charge_state, standby_state;
	
	charge_state = Bat_GetChargeState();
	standby_state = Bat_GetStandbyState();
	
	if ((charge_state == 0) && (standby_state == 1))
		return 1;
	else
		return 0;
}

u8 Bat_IsCharging(void)
{
	u8 charge_state, standby_state;
	
	charge_state = Bat_GetChargeState();
	standby_state = Bat_GetStandbyState();
	
	if ((charge_state == 1) && (standby_state == 0))
		return 1;
	else
		return 0;
}

extern u8 powerisfull;
static u32 cache=0;
u8 Bat_IsChargForSlave(void)
{
	if(powerisfull==1)
	{
		cache=CMOS_GetTimer();
		return 0;
	}else if(CMOS_GetTimerCount(cache)<1000)      ////开机后这个值在变大，走这不显示闪电
	{
		return 0;
	}
	return Bat_IsCharging();                   //检测是否充电
}
