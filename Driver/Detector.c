#include "Detector.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "misc.h"
#include "CMOS.h"
#include "CMLIB.h"
#include "GEngineCfg.h"
#include "Config.h"

static Location_Res nearsensor;
static enum Box_Side side=box_up;
static u32 nstcnt;

void NearSensorInit(void)
{
	u32 _id;
	
	_id=readSlaverID();	
	nearsensor.slaverID=_id&0x00ff;
	nearsensor.dir[0]=0;
	nearsensor.dir[1]=0;
	nearsensor.dir[2]=0;
	nearsensor.dir[3]=0;
	nearsensor.systime=CMOS_GetTimer();
	nearsensor.time[0]=nearsensor.systime;
	nearsensor.time[1]=nearsensor.systime;
	nearsensor.time[2]=nearsensor.systime;
	nearsensor.time[3]=nearsensor.systime;
	side=box_up;
	nstcnt=CMOS_GetTimer();
}

static void NearSensorUpdate(enum Box_Side side)
{
	u8 ret;
	u8 index=side;
	
	Detector_Init(side);
	ret = Detector_GetAverageValue();
	Detector_Close();
	
	if(ret!=nearsensor.dir[index])
	{
		nearsensor.dir[index]=ret;
		nearsensor.time[index]=CMOS_GetTimer();
		
		//for test
//		if(1==ret)
//		{
//			AddGraph(32773,side*32,16,7);
//		}else
//		{
//			ClearGraphLayer(7);
//		}
//		SetDisplayUpdate();
		
	}	
}
	
void NearSensorServer(void)
{
	if((side>=box_invaliad) &&(nstcnt-CMOS_GetTimer()>=60))	//201609002 100))
	{
		nstcnt=CMOS_GetTimer();
		side=box_up;
	}
	if(side>=box_invaliad)
		return;
	NearSensorUpdate(side);
	side++;	
	
	//added 20160902
	NearSensorUpdate(side);
	side++;	
	NearSensorUpdate(side);
	side++;	
	NearSensorUpdate(side);
	side++;	
	
}

int GEA_GetNearStatus(PCom_Arg in,PLocation_Res out)
{
	nearsensor.systime=CMOS_GetTimer();
	ccopy((u8*)&nearsensor,(u8*) out,sizeof(Location_Res));
	return 0;
}

static void ADC2_GPIO_Config(u16 pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);				// PC1,输入时不用设置速率
}

static void ADC2_Mode_Config(u8 channel)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	
	/* ADC2 configuration */
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
	ADC_RegularChannelConfig(ADC2, channel, 1, ADC_SampleTime_1Cycles5);
	
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

void Detector_Init(enum Box_Side side)
{
	u16 gpio_pin;
	u8 adc_channel;
	
	switch (side) {
//	case box_up:
//		gpio_pin = GPIO_Pin_3;
//		adc_channel = ADC_Channel_13;
//		break;
//		
//	case box_left:
//		gpio_pin = GPIO_Pin_2;
//		adc_channel = ADC_Channel_12;
//		break;
//		
//	case box_right:
//		gpio_pin = GPIO_Pin_1;
//		adc_channel = ADC_Channel_11;
//		break;
//		
//	case box_down:
//		gpio_pin = GPIO_Pin_0;
//		adc_channel = ADC_Channel_10;
//		break;
	case box_up:
		gpio_pin = GPIO_Pin_1;
		adc_channel = ADC_Channel_11;	
		break;
		
	case box_left:
		gpio_pin = GPIO_Pin_0;
		adc_channel = ADC_Channel_10;
		break;
		
	case box_right:
		gpio_pin = GPIO_Pin_2;
		adc_channel = ADC_Channel_12;
		break;
		
	case box_down:
		gpio_pin = GPIO_Pin_3;
		adc_channel = ADC_Channel_13;
		break;
		
	default:
		gpio_pin = GPIO_Pin_1;
		adc_channel = ADC_Channel_11;	
		break;
	}
	
	ADC2_GPIO_Config(gpio_pin);
	ADC2_Mode_Config(adc_channel);
}

void Detector_Close(void)
{
	ADC_Cmd(ADC2, DISABLE);
}

void dipdots(u16 *dots);

u8 Detector_GetAverageValue()
{
	u16 value;
	u16 min=4096;
	u16 max=0;
	u16 m;
	int i=0;
	
//	u16 sample[25];
//	int k=0;
//	
//	u32 sum = 0;
	
	
	while(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET);
	min=max = ADC_GetConversionValue(ADC2);
	for(;i<25;i++)
	{
		while(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET);
		value = ADC_GetConversionValue(ADC2);
		if(value>max)
		{
			max=value;
		}
		if(value<min)
		{
			min=value;
		}
		
//		sample[k++]=value;
//		
//		sum = sum + value;
		
	}
	
//	u32 avg = 0;
//	avg = sum / 25;
//	
//  u32 squer = 0;
//	sum = 0;
//	for(i=0;i<25;i++)
//	{
//		if(sample[i]>=avg)
//			sum = sum + (sample[i] -avg)*(sample[i] -avg);
//		else
//			sum = sum + (avg-sample[i])*(avg-sample[i]);
//	}
//	
//	
//	//128*128
//	Console_Open("");
//	dipdots(sample);
//	
////	RefreshScreen();
//	Console_Add_Hex((u8 *)sum,4);	
//	
//	
//	k=0;
//	while(k<3)
//	{
//		stCheck();
//		mDelay(100);
//		IWDG_ReloadCounter();
//		k++;
//	}
//	
//	
//	
	
	m=max-min;
//	if(m>=0x700 )	//0.6V振幅  0.8V 992；
	if(m>=0x880 )	//0.6V振幅  0.8V 992；
	{
		return 1;
	}else
	{
		return 0;
	}
}

u8 * Getdisplaybuf(void);

void putpixcel(s16 x,s16 y)
{
	u16 *p=(u16 *)Getdisplaybuf();
	int seggoff=y*128+x;
	*(p+seggoff)=0x0000;
}

void dipdots(u16 *dots)
{
	u16 x,y;
	u16 *p=dots;
	x=0;
	for(int i=0;i<25;i++)
	{
		y=*p*127/4096;
		putpixcel(x,y);
		p++;
		x+=4;
	}
}




