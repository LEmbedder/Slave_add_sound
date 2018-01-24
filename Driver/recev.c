#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "misc.h"
#include "CMLIB.h"
#include "recev.h"
#include "CMOS.h"

#include "GEngineCfg.h"
#include "Config.h"

static recev myrecev;
static Location_Res nearsensor;

static void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

	//PC0/1/2/3 ��Ϊģ��ͨ����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}


static void ADC1_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); //72M/8=9,ADC���ʱ�䲻�ܳ���14M

	ADC_DeInit(ADC1); //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC����ģʽ:ADC1��ADC1�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode =ENABLE; //ģ��ת��������ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //ģ��ת������������ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //�ⲿ����ת���ر�
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = M_chanel; //˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure); //����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���

	
	//����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
	//ADC1,ADCͨ��x,�������˳��ֵΪy,����ʱ��Ϊ239.5����	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_239Cycles5);//ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 2, ADC_SampleTime_239Cycles5);//ADC_SampleTime_239Cycles5 );	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5);//ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5);//ADC_SampleTime_239Cycles5 );


	// ����ADC��DMA֧�֣�Ҫʵ��DMA���ܣ������������DMAͨ���Ȳ�����
	ADC_DMACmd(ADC1, ENABLE);


	ADC_Cmd(ADC1, ENABLE); //ʹ��ָ����ADC1

	ADC_ResetCalibration(ADC1); //��λָ����ADC1��У׼�Ĵ���

	while(ADC_GetResetCalibrationStatus(ADC1)); //��ȡADC1��λУ׼�Ĵ�����״̬,����״̬��ȴ�


	ADC_StartCalibration(ADC1); //��ʼָ��ADC1��У׼״̬

	while(ADC_GetCalibrationStatus(ADC1)); //��ȡָ��ADC1��У׼����,����״̬��ȴ�

}


static void DMA_Configuration(u32 addr)
{

	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //ʹ��DMA����
	
	DMA_DeInit(DMA1_Channel1); //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = addr;	//&AD_Value; //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //�ڴ���Ϊ���ݴ����Ŀ�ĵ�
	DMA_InitStructure.DMA_BufferSize = N_sample*M_chanel; //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	//DMA_Mode_Circular; //������ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��
	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	DMA_Priority()
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
#define UN (4.5)
#define SEGOFF 2

static u8 DMAIsDONE=0;

static void getsample()
{
	u32 addr;
	u8 sub=myrecev.windex-myrecev.rindex;
	if(sub<2)
	{
		if( myrecev.windex&0x01)
			addr=(u32)&myrecev.DMABUF;
		else
			addr=(u32)&myrecev.AD_Value;
		myrecev.writing=1;
		ADC1_Configuration();
		DMA_Configuration(addr);
		DMA_Cmd(DMA1_Channel1, ENABLE);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);		
	}
}

static void sort(vu16 *p,u8 arr[],int length)
{
	int i,j;
	vu16 *point;
	u8 value;
	for(i=0;i<length;i++)
	{
		for(j=i+1;j<length;j++)
		{
			if(*(p+4*arr[j])>(*(p+4*arr[i])))
			{
				value=arr[i];
				arr[i]=arr[j];
				arr[j]=value;
			}
		}
	}
}

#define maxNum 13
#define midIndex 6

void recevcheck(void)
{
	vu16 *p;
	vu16 val;
	u32 sum[M_chanel];
	u8 side;
	int i,j,cnt;
	int findlow,Hcnt;
	
	if(myrecev.windex==myrecev.rindex)
	{
		i=1;
		return;
	}

	//ѡ���ź�����ǿ���ŵ�
	cnt=(int)((float)UN*28);
	for(i=0;i<M_chanel;i++)
	{
		if( myrecev.rindex&0x01)
			p=myrecev.DMABUF;
		else
			p=myrecev.AD_Value;
		
		p+=i;
		
		sum[i]=0;
			
		for(j=0;j<cnt;j++)
		{
			sum[i]+=*p;
			p+=M_chanel;
		}
		
	}
	side=0;
	for(j=1;j<M_chanel;j++)
	{
		if(sum[j]>sum[side])
			side=j;
	}

	//ѡ���ŵ�������ǿ���ź�
	{
		if( myrecev.rindex&0x01)
			p=myrecev.DMABUF;
		else
			p=myrecev.AD_Value;
		
		p+=side;
		
		
		//��maxNum���������λ��
		
		u8 max[maxNum];
		for(i=0;i<maxNum;i++)
			max[i]=i;
		
		sort(p,max,maxNum);//sort max;
		u8 left[midIndex],right[maxNum];
		int li,ri;
		li=ri=0;
		for(j=maxNum;j<N_sample;j++)
		{
			u16 mid=*(p+4*max[midIndex]);
			if(*(p+4*j)>=mid)
			{
				left[li]=j;
				li++;
			}else 
			{
				right[ri]=j;
				ri++;
			}
			if(li>=midIndex)
			{
				ccopy(left,&max[midIndex+1],midIndex);
				sort(p,max,maxNum);//sort max
				li=ri=0;
			}else if(ri>=midIndex)
			{
				ccopy(&max[midIndex+1],&right[midIndex],midIndex);
				sort(p,right,maxNum-1);//sort right
				ccopy(right,&max[midIndex+1],midIndex);
				ri=0;
			}
			
		}
		
		ccopy(&max[midIndex+1],&right[ri],midIndex);
		for(i=ri+midIndex;i<maxNum;i++)
		{
			right[i]=max[maxNum-1];
		}
		sort(p,right,maxNum);//sort right
		ccopy(left,&max[midIndex+1],li);
		ccopy(right,&max[midIndex+1+li],midIndex-li);
		sort(p,max,maxNum);//sort max
		
		//�������ֵ�����ֵ��λ�ù�ϵ�������
		
		//3.0 ������ֵС����Ч��������Ϊ�Ǹ����ź�
		u8 ret=0;
		val=*(p+4*max[0]);
		if(val<myrecev.threshold)
		{
			goto _Update;
		}
		
		//��ǰ��9��13���ֵ�ķֲ����
		int sampleCnt=9;
		_Gethold:
		ccopy(max,right,sampleCnt);
		for(i=0;i<sampleCnt;i++)
		{
			for(j=i+1;j<sampleCnt;j++)
			{
				if(right[j]<right[i])
				{
					cnt=right[i];
					right[i]=right[j];
					right[j]=cnt;
				}
			}
		}		
		
		//3.1 ������ֵ �м�������
		Hhold lowhold[4];
		li=0;
		cnt=1;
		for(i=1;i<sampleCnt;i++)
		{
			u8 sub=right[i]-right[i-1];
			if(sub>=5)
			{
				cnt++;
				lowhold[li].start=right[i-1];
				lowhold[li].end=right[i];
				lowhold[li++].length=sub;
			}else if(sub>=4)
			{
				for(j=right[i-1]+1;j<right[i];j++)
				{
					val=*(p+4*j);
					if(val<myrecev.threshold)
					{
						cnt++;
						lowhold[li].start=right[i-1];
						lowhold[li].end=right[i];
						lowhold[li++].length=sub;
						break;
					}
				}
			}
		}
		
		if((cnt>=5)||(cnt<2))
			goto _Exit;
		if((9==sampleCnt)&&(cnt==2)&&(lowhold[0].length>28))
		{
 			sampleCnt+=2;
			goto _Gethold;
		}
		j=0;
		for(i=1;i<li;i++)
		{
			if(lowhold[i].length<lowhold[j].length)
				j=i;
		}
		
		//����length
		u16 sum=*(p+4*lowhold[j].start)+*(p+4*lowhold[j].end);
		sum>>=1;
		val=*(p+4*lowhold[j].start+1);
		for(i=lowhold[j].start+2;i<lowhold[j].end;i++)
		{
			if(*(p+4*i)<val)
				val=*(p+4*i);
		}
		sum+=val;
		sum>>=1;
		
		for(i=lowhold[j].start;i<lowhold[j].end;i++)
		{
			if(*(p+4*i)<sum)
			{
				lowhold[j].start=i;
				break;
			}
		}
		
		for(i=lowhold[j].end;i>lowhold[j].start;i--)
		{
			if(*(p+4*i)<sum)
			{
				lowhold[j].end=i;
				break;
			}
		}
		u8 length=lowhold[j].end-lowhold[j].start+1;
		
		switch(length)
		{
			case 3:
			case 4:
			case 5:
				ret =1;
				break;
			
			case 8:
			case 9:
			case 10:
				ret =2;
				break;
			
			case 13:
			case 14:
			case 15:
				ret =3;
				break;
			
			case 17:
			case 18:
			case 19:
				ret =4;
				break;
			default:
				ret=5;
				break;
		}
		if(ret>=5)
			goto _Exit;		
		
		_Update:
		if(ret!=nearsensor.dir[side])
		{
			for(i=0;i<4;i++)
				nearsensor.dir[i]=0;
			nearsensor.dir[side]=ret;
			nearsensor.time[side]=CMOS_GetTimer();
		}

		_Exit:
		;
		
	}
		
	myrecev.rindex++;
	if(0==myrecev.writing)
		getsample();
	
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1))
	{
		ADC_SoftwareStartConvCmd(ADC1, DISABLE);
		DMA_ClearITPendingBit(DMA1_IT_GL1);
		myrecev.writing=0;
		myrecev.windex++;
		getsample();
	}
}

int recev_Init(void)
{
	int i;
	myrecev.threshold=thresholdDefault;
	for(i=0;i<M_chanel;i++)
		myrecev.dir[i]=0;
	for(i=0;i<M_chanel*M_chanel;i++)
		myrecev.hthr[i]=hthrDefault;
	myrecev.windex=0;
	myrecev.rindex=0;
	myrecev.writing=0;
	
	GPIO_Configuration();
	getsample();
}

int GEA_GetNearStatus(PCom_Arg in,PLocation_Res out)
{
	nearsensor.systime=CMOS_GetTimer();
	ccopy((u8*)&nearsensor,(u8*) out,sizeof(Location_Res));
	return 0;
}


