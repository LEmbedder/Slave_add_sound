#include "send.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "CMOS.h"
#include "CMLIB.h"
#include "GEngineCfg.h"
#include "Config.h"

static Location_Res nearsensor;

static void NearSensorInit(void)
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
}

int GEA_GetNearStatus(PCom_Arg in,PLocation_Res out)
{
	nearsensor.systime=CMOS_GetTimer();
	ccopy((u8*)&nearsensor,(u8*) out,sizeof(Location_Res));
	return 0;
}

void send_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	NearSensorInit();
}

static int mscnt=0;

void sendservice(void)
{
	switch(mscnt)
	{
		case 0:
			GPIOC->BRR = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3;
			break;
		case 1:
			GPIOC->BSRR = GPIO_Pin_1 ;
			break;
		case 2:
			GPIOC->BRR = GPIO_Pin_1;
			break;
		case 3:
			GPIOC->BSRR =  GPIO_Pin_1;
			break;
		case 4:
			GPIOC->BRR = GPIO_Pin_1;
			break;
		case 6:
			GPIOC->BSRR = GPIO_Pin_0 ;
			break;
		case 7:
			GPIOC->BRR = GPIO_Pin_0 ;
			break;
		case 9:
			GPIOC->BSRR = GPIO_Pin_0 ;
			break;
		case 10:
			GPIOC->BRR = GPIO_Pin_0 ;
			break;
		
		case 13:
			GPIOC->BSRR = GPIO_Pin_2 ;
			break;
		case 14:
			GPIOC->BRR = GPIO_Pin_2 ;
			break;
		case 17:
			GPIOC->BSRR = GPIO_Pin_2 ;
			break;
		case 18:
			GPIOC->BRR = GPIO_Pin_2 ;
			break;
		
		case 22:
			GPIOC->BSRR = GPIO_Pin_3 ;
			break;
		case 23:
			GPIOC->BRR = GPIO_Pin_3 ;
			break;
		case 27:
			GPIOC->BSRR = GPIO_Pin_3 ;
			break;
			
		default:
			break;
	}
	mscnt++;
	if(mscnt>=28)
		mscnt=0;

}



