#include "xiaomu.h"
#include "CMOS.h"
#include "NRF24L01.h"  
#include "Display.h"
#include "pcm.h"
#include "IC_SPI.h"
#include "Commu.h"
#include "Flash.h"
#include "File.h"
#include "Battery.h"
#include "nRF.h"
#include "g-sensor.h"
#include "Detector.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_dbgmcu.h"
#include "GEngineCfg.h"
#include "FLASHROM.h"
#include "Motor.h"
#include "sentence.h"
#include "Config.h"
#include "hdr.h"
#include "g-sensor.h"
#include "MMA7660FC.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"
#include "Config.h"
#include "hardware.h"
#include "SingleWireSlave.h"
#include "OneLine.h"
#include "Config.h"
#include "GD62429.h"
#include "USART.h"

#ifdef _SEND_
#include "send.h"
#endif

#ifdef _RECEV_
#include "recev.h"
#endif

#define RSSI_low 32977

typedef  void (*Callfun)(void *);
typedef  void (*Callfunreturn)(void *,void *);

void G_CallFuntion(void * input)
{
	u8 *fun=(u8*)input;
	u8 funname=*fun++;
	
	if(funname>=Enum_GE_Msg_Sum)
		return;
	
	Callfun funtion;
	if(0==TB_GE_MsgApiInfo[funname].outputSize)
	{
		funtion=(Callfun)TB_GE_MsgApiInfo[funname].function;
		funtion(fun);
	}
}

extern void poweron(void);
extern void poweroff(void);
uint32_t statusreg;
void shutdown(void)
{
	MMA7660FC_Init();
	MMA7660FC_Standby();
#ifdef HANGUP
	Display_Initialization();
	Console_Open("shutdown");
	Console_Add_Hex((u8 *)&statusreg,4);
	mDelay(3000);
#endif
	IWDG_ReloadCounter();	//20170327
//	PWR_EnterSTANDBYMode();
	while(1)
	{
		poweroff();
		mDelay(3000);
		MMA7660FC_Init();
		MMA7660FC_Standby();
	}
}
#ifdef HANGUP
extern volatile int trace01;
#endif
int slaverpowerOff(PCom_Arg in,int mode)
{
	int i;
	HDRLayer_Arg stopani;
	Layer_Arg clrgraph;
	Com_Arg comarg;
	Animation_Arg ani;
	Speech_Arg speech;
	
	Motor_Off();
	GEA_StopSpeech(&comarg);	
	stopani.layers=(u8)C_Layer_All;	
	GEA_StopAnimation(&stopani);
	for(i=0;i<C_Layer_Max;i++)
		clrgraph.layer[i]=1;
	GEA_ClearLayer(&clrgraph);

	i=readSlaverID();
	if(i)
		ani.animationID= 0x8061; //804c
	else
		ani.animationID= 0x8062;//8005
	
	ani.x=ani.y=0;
	ani.layer=C_HDRLayer_1;
	GEA_AddAnimation(&ani);
	
	while (1) {
		play_check();
		Display_CheckUpdate();
		if(0==play_status())
			break;
		IWDG_ReloadCounter();
#ifdef HANGUP
	trace01=20;
#endif
	}
	
	//here should close hardwar
	
	shutdown();	//201170327
	
/*20170327
	if(2==mode)
	{
		MMA7660FC_Init();
		MMA7660FC_Standy();
	}else
	{
		MMA7660FC_Standby();
	}
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_WakeUpPinCmd(ENABLE);
	MMA7660FC_ClearIntFlag();
	
	#if 1	//关机关电
		poweroff();
	#endif
	PWR_EnterSTANDBYMode();
	while(1)
		IWDG_ReloadCounter();
*/	
	return 0;
}

int GEA_PowerOff(PCom_Arg in)
{
	return slaverpowerOff(in,0);
}

int GEA_Transport(PCom_Arg in)
{
	return slaverpowerOff(in,1);
}



extern u8 Bat_IsChargForSlave(void);
int powersum;
int powervalue;
u8 powerLevel;
u8 powerisfull=0;
void checkpower()
{
	int value=Bat_GetAD();
	powersum-=powervalue;
	powersum+=value;
	powervalue=powersum/C_POWERSAMPLE;
	u32 timecnt;
	if(powervalue<C_SHUTDOWNPOWER)
	{
		Display_Initialization();
		Console_Open((u8 *)"low voltage");//充电画面
		IWDG_ReloadCounter();
		mDelay(1000);
		timecnt=CMOS_GetTimer();
		while(1)
		{
			if(1!=Bat_IsChargForSlave())
			{
				if(CMOS_GetTimerCount(timecnt)>=1000)
					break;
			}else
			{
				timecnt=CMOS_GetTimer();
			}
			IWDG_ReloadCounter();	//20170327
			value=Bat_GetAD();
			powersum-=powervalue;
			powersum+=value;
			powervalue=powersum/C_POWERSAMPLE;
			if(powervalue>=C_LOWPOWER)
				break;
		}
		if(powervalue<C_LOWPOWER)
			shutdown();	//20170408
	}
		
	if(Bat_IsCharging() == 1){                 //充电
		
//		if(powervalue<C_LOWPOWER)
//		{
//			powerLevel=0;
//		}else 
		
		if(powervalue >= C_LOWPOWER && powervalue<C_LEVEL1_POWER)
		{
			powerLevel=1;
		}else if(powervalue>=C_LEVEL1_POWER && powervalue<C_LEVEL2_POWER)
		{
			powerLevel=2;
		}else if(powervalue >= C_LEVEL2_POWER)
		{
			powerLevel=3;
		}
		
		if(powervalue>=(C_FULLPOWER))
		{
			powerisfull=1;        
		}else{
			powerisfull=0;
		}
  }
	
	if(Bat_IsCharging() == 0){                //放电
		if(powervalue<F_LOWPOWER)
		{
			powerLevel=0;
		}else if(powervalue >= F_LOWPOWER && powervalue<F_LEVEL1_POWER)
		{
			powerLevel=1;
		}else if(powervalue>=F_LEVEL1_POWER && powervalue<F_LEVEL2_POWER)
		{
			powerLevel=2;
		}else if(powervalue >= F_LEVEL2_POWER)
		{
			powerLevel=3;
		}
		
//		if(powervalue>=(C_FULLPOWER))
//		{
//			powerisfull=1;        
//		}else{
//			powerisfull=0;
//		}
		
	}
	
	
}

static void SW_Check()		
{
	u8 recv[20]={0x00,0x00};
	u8 command;
	u32 sid;
	u32 recvsid;
	u8 *p;
	if (SWS_BytesToRead() >= 2) {
		SWS_getBytes(recv, 2);
		if(recv[0]!=0x5a)
		{
			SWS_ReadBytes(recv, 1);
		}else
		{
			command=recv[1];
			switch(command)
			{
				case 0x01:
					_send_buf[1]=0x01;
					nRF_getAddr(&_send_buf[2]);
					_send_buf[7]=0x5a;
					SWS_SendBytes(_send_buf, 8);
					SWS_ReadBytes(recv, 2);
					break;
				case 0x02:
					if (SWS_BytesToRead() >= 8)
					{
						SWS_ReadBytes(recv, 8);
						if(recv[7]==0x5a)
						{
							_send_buf[1]=0x02;
							SWS_SendBytes(_send_buf, 2);
							nRF_setAddr(&recv[2]);
						}
					}
					break;
				case 0x03:
					_send_buf[1]=0x03;
					sid= readSlaverID();
					p=(u8 *)&sid;
					_send_buf[2]=*p++;
					_send_buf[3]=*p++;
					_send_buf[4]=*p++;
					_send_buf[5]=*p;
					_send_buf[6]=0x5a;
					SWS_SendBytes(_send_buf, 7);
					SWS_ReadBytes(recv, 2);
					break;
				case 0x04:
					if (SWS_BytesToRead() >= 7)
					{
						SWS_ReadBytes(recv, 7);
						if(recv[6]==0x5a)
						{
							_send_buf[1]=0x04;
							SWS_SendBytes(_send_buf, 2);
							
							p=(u8 *)&recvsid;
							*p++=recv[2];
							*p++=recv[3];
							*p++=recv[4];
							*p=recv[5];
							WriteSlaverID(recvsid);
						}
					}
					break;
				case 0x05:
					if (SWS_BytesToRead() >= 7)
					{
						SWS_ReadBytes(recv, 7);
						if(recv[6]==0x5a)
						{
							_send_buf[1]=0x05;
							SWS_SendBytes(_send_buf, 2);
							
							if(('u'==recv[2])&&('p'==recv[3])&&('d'==recv[4])&&('a'==recv[5]))
							{
								mDelay(1);
								Resetproisok();
								SystemReset();
							}
						}
					}
					break;
				case 0x06:
					if (SWS_BytesToRead() >= 12)
					{
						SWS_ReadBytes(recv, 12);
						if(recv[11]==0x5a)
						{												
							_send_buf[1]=0x06;
							SWS_SendBytes(_send_buf, 2);
							u32 tcnt=CMOS_GetTimer();
							while((SWS_BytesToRead() < 2)&&(tcnt-CMOS_GetTimer()<3000))
								;
							ONELINE_Init();
							nRF_TxSetRP1P2(&recv[2]);
						}
					}
					//配置RX_ADDR_P1,RX_ADDR_P2
					break;
				default:
					SWS_ReadBytes(recv, 2);
				break;
			}
		}
		}
}

void updategame(void);
u8 isBatCharging=0;

extern volatile u32 AutoPowerOffCnt;
extern u32 BkAutoPowerOff;

void initionNRF(void)
{
	int i;
	nRF_Initialization();
	i=nRF_updateAddr();	
	if(1==i)	
		hardware_main();//enter test mode
	//这里配置 TX_ADDR,RX_ADDR_P0及其他
	nRF_InitReg();
	nRF_EXTIcfg();
	nRF_RecieveMode();
}

void slaver_main(void)
{
	Animation_Arg ani;
	Motor_Arg motor;
	Com_Res ret;
	
	//fortest start
	Speech_Arg speech;
	speech.voiceID=0x0000;
	//fortest end
		
//	u8 power=0;
	int i;
	u32 timecnt;
	u32 _msg_buf[C_nRF_CtrlCmdSize/4+1];
	u8 *msg_buf=(u8 *)&_msg_buf;
	msg_buf+=3;
	poweron();
	CMOS_Initialization();
	
//	RS_Init(115200);
//	printf("-------------------\n");
	
	statusreg = RCC->CSR;
#if 0 //20170408	
	power=RCC_GetFlagStatus(RCC_FLAG_IWDGRST);//RCC_FLAG_PINRST);
	power|=RCC_GetFlagStatus(RCC_FLAG_LPWRRST);	//20170327
	RCC_ClearFlag();
	if(power )
	{
		shutdown();	//20170327
/*20170327
		MMA7660FC_Init();
		MMA7660FC_Standby();	//20170327 MMA7660FC_Standy();
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		PWR_WakeUpPinCmd(ENABLE);
		MMA7660FC_ClearIntFlag();
		
		#if 1	//关机关电
			poweroff();
		#endif
		PWR_EnterSTANDBYMode();
		while(1)
			;
*/		
	}
#endif	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_128);
	IWDG_SetReload(1565);   // 3.2ms * 1565 = 5.008s
	IWDG_Enable();
	DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE);
	IWDG_ReloadCounter();
		
	Bat_Init();
	
	powersum=0;
	powervalue=0;
	
	
	for(i=0;i<C_POWERSAMPLE;i++)
	{
		powersum+=Bat_GetAD();
	}
	powervalue=powersum/C_POWERSAMPLE;
	checkpower();
	
	
	
	//模块初始化	
	Flash_Init();
	File_SetBaseAddr(FLASH_SYSTEM_ADDR,FLASH_GAME_ADDR);
	if(0){
		int i;
		unsigned char aaaa[28] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                         0xff,0xff,0xaa,0x55,0x55,0xaa,0xa7,0x55,0x55,0xaa,
                         0xaa,0xaa,0x00,0x00,0x55,0x55,0x00,0x00};
		unsigned char bbbb[28];										 
//		Flash_4K_Erase(FLASH_CONFIG_ADDR);
//		Flash_Write(FLASH_CONFIG_ADDR, aaaa, 28);
		Flash_Read(FLASH_CONFIG_ADDR,bbbb,28);										 
		for(i = 0;i < 28;i++)	
    {
			if(aaaa[i] != bbbb[i])
				break;
    }												 
	}
	
	InitConfig();

	sndInit();
		
	SWS_Init();

	Motor_Init();

	gSensor_Init();
	
	GD62429_GPIO_Init();
	
	

	
#ifdef _RECEV_
	recev_Init();	//20170523
#endif

#ifdef _SEND_
	send_Init();	//20170523
#endif	
	
	Display_Initialization();
  
  //while(1);
	initionNRF();

	hdr_Init();
	
	timecnt = CMOS_GetTimer();
	
	if(0==powerLevel)
	{
		Console_Open((u8 *)"Power Low");//充电画面
		mDelay(1000);
		IWDG_ReloadCounter();
	}

	//opening
	i=readSlaverID();
	if(i<0 || i>=2)
		i=0;
	ani.animationID=0x806b-i;
	ani.layer=C_HDRLayer_1;
	ani.x=ani.y=0;
	if(0x0c000000==statusreg)
		GEA_AddAnimation(&ani);
#ifdef HANGUP	
	else
	{
			Console_Open("RSS");
			mDelay(2000);       // 2
	}
#endif	
	motor.mode=1;
	GEA_MotorCtrl(&motor);
	
	isBatCharging=Bat_IsChargForSlave();
	
	//fortest start
//	GEA_LoopSpeech(&speech);
	//fortest end
	#ifdef HANGUP
	trace01=0;
#endif
	while (1) {
		
		//fortest start
//		GEA_MotorCtrl(&motor);
//		battery=Bat_GetAD();
//		PutNum(0,0,battery);
//		RefreshScreen();		
		//fortest end
		
#ifdef HANGUP
	trace01=1;
#endif
		if (timecnt-CMOS_GetTimer() >= 200) {
			timecnt = CMOS_GetTimer();
			gSensor_Update();		
		}		
#ifdef HANGUP
	trace01=2;
#endif
		
#ifdef _RECEV_
	recevcheck();//20170523		NearSensorServer();
#endif		
	
#ifdef HANGUP
	trace01=3;
#endif
		play_check();
#ifdef HANGUP
	trace01=4;
#endif
		Display_CheckUpdate();                      //
#ifdef HANGUP
	trace01=5;
#endif
		stCheck();
#ifdef HANGUP
	trace01=6;
#endif
		IWDG_ReloadCounter();	//20170407
		
		SW_Check();	//		Com_SPI_CheckMessage();
#ifdef HANGUP
	trace01=7;
#endif

		if (nRF_GetMessage(msg_buf) == TRUE) {
			G_CallFuntion(msg_buf);
		}
#ifdef HANGUP
	trace01=8;
#endif
		updategame();
#ifdef HANGUP
	trace01=9;
#endif
		i=CheckAutoPowerOff();                        //检测子机关机
#ifdef HANGUP
	trace01=10;
#endif
		if(1==i)
		{
			GEA_PowerOff(0);
		}else if(2==i)
		{
			#ifdef HANGUP
			Console_Open("RINITNRF");
			#endif
			initionNRF();
			mDelay(2000);
//			nRF_InitReg();
//			nRF_EXTIcfg();
//			nRF_RecieveMode();
		}else if(0!=BkAutoPowerOff){
			if(CMOS_GetTimerCount(BkAutoPowerOff)>=1000){
				BkAutoPowerOff=CMOS_GetTimer();
				initionNRF();
//				nRF_InitReg();
//				nRF_EXTIcfg();
//				nRF_RecieveMode();
			}
		}

#ifdef HANGUP
	trace01=11;
#endif
		//check power
		GEA_GetSpeechStatus(0,&ret);
#ifdef HANGUP
	trace01=12;
#endif
		if((ret.result == C_SpeechEnd)&&(0==getMotorFlag()))
			checkpower();
#ifdef HANGUP
	trace01=13;
#endif
		if(isBatCharging!=Bat_IsChargForSlave())                 /////       0    1 
		{
			isBatCharging=Bat_IsChargForSlave();
			if(1==isBatCharging)                                  //// 为1时显示闪电
				RefreshScreen();
			else
				RefreshGraphs();
		}
#ifdef HANGUP
	trace01=14;
#endif
		IWDG_ReloadCounter();
	}
}

extern volatile STRUCT_FIFO_REG R_nRF_Fifo;
extern volatile STRUCT_nRF_CMD R_nRF_Buffer[C_nRF_Fifo_Size];
int static forcnt=0;
static u8 ppid=0x80;

void nRF_IRQHandler()
{
	int i;
	u8 idx;
	u8 funname;
	u8 retsize;
	u32 outbuf[C_nRF_CtrlCmdSize/4];
	void *output=outbuf;
	Callfunreturn funret;
	u8 newppid;
	
	ClrAutoPowerOff();
	
	u32 _msg_buf[C_nRF_CtrlCmdSize/4+1];
	u8 *msg_buf=(u8 *)&_msg_buf;
	msg_buf+=3;
	
	nRF_ReadRxBuffer(msg_buf,C_nRF_CtrlCmdSize);

////////////////////////////add lijun
	if(get_NRF_RSSI() == 0)
	{
		ClearGraphLayer(7);
		AddGraph(RSSI_low, 0, 0,7);
		IWDG_ReloadCounter();
	}
	else
	{
		ClearGraphLayer(7);
	}
	
///////////////////////////	
	
	
	
	
	funname=*msg_buf;
	retsize=TB_GE_MsgApiInfo[funname].outputSize;
	newppid=*(msg_buf+31);
	
	//如果是新的包就执行新的动作，否则不执行动作
		
	if((funname<Enum_GE_Msg_Sum)&&(retsize>0))
	{
		msg_buf++;
		funret=(Callfunreturn)TB_GE_MsgApiInfo[funname].function;              //执行动作
		funret(msg_buf,output);		
	}else
	{
		if(newppid!=ppid)
		{			
//			forcnt++;
			ppid=newppid;
			if(0==FIFO_GetPushIndex((PSTRUCT_FIFO_REG)&R_nRF_Fifo, &idx))
			{
				for(i=0;i<C_nRF_CtrlCmdSize;i++)
				{
					R_nRF_Buffer[idx].para[i]=*msg_buf++;
				}		
			}
		}
		outbuf[0]=forcnt;
	}
	SendMessage((u8 *)output,32);
	 
}

#include "Flash.h"
#define C_DATABUFSIZE 2048	
#define C_DATABUFCNT 4
u8 dataRbuf[C_DATABUFSIZE*C_DATABUFCNT];
volatile u32 readIndex=0;
volatile u32 writeIndex=0;
volatile u32 dataIndex=0;
u32 downloadgamedataAddr=FLASH_GAME_ADDR;
u32 Eraseed=FLASH_GAME_ADDR;
static u8 data_pid=0xff;

void resetupdate(void)
{
	if(0!=forcnt)
	{
		forcnt=0;
	}
	writeIndex=0;
	readIndex=0;
	dataIndex=0;
	data_pid=0xff;
	downloadgamedataAddr=FLASH_GAME_ADDR;
	Eraseed=FLASH_GAME_ADDR;
}

void updateend(void)
{
	while(writeIndex-readIndex>=1)
	{
		updategame();
	}
	
	resetupdate();
	File_SetBaseAddr(FLASH_SYSTEM_ADDR, FLASH_GAME_ADDR);
}


void updategame(void)
{
//	int cnt=writeIndex-readIndex;
//	int size;
//	if(cnt>=1)
//	{
//		size=cnt*C_DATABUFSIZE;
//		if((downloadgamedataAddr+size)>Eraseed)
//		{
//			Flash_4K_Erase_Start(Eraseed);
//			Eraseed+=(4096);
//		}
//		Flash_Write(downloadgamedataAddr, (u8*) &(dataRbuf[(readIndex&(C_DATABUFCNT-1))*C_DATABUFSIZE]), size);
//		downloadgamedataAddr+=size;		
//		readIndex+=cnt;
//	}
	
	if(writeIndex-readIndex>=1)
	{
		if(downloadgamedataAddr==FLASH_GAME_ADDR)
			Flash_4K_Erase_Start(downloadgamedataAddr);
		Flash_Write(downloadgamedataAddr, (u8*) &(dataRbuf[(readIndex&(C_DATABUFCNT-1))*C_DATABUFSIZE]), C_DATABUFSIZE);
		downloadgamedataAddr+=C_DATABUFSIZE;		
		if(0==((downloadgamedataAddr-FLASH_GAME_ADDR)%4096))
			Flash_4K_Erase_Start(downloadgamedataAddr);
		readIndex++;
	}
	
}

#define C_PACKGEize 31
void nRF_ReceiveDataHandler(u8 status)
{
	u8 newppid;
	u32 _msg_buf[C_nRF_CtrlCmdSize/4];
	u8 *msg_buf=(u8 *)&_msg_buf;
	ClrAutoPowerOff();
	nRF_ReadRxBuffer(msg_buf,C_nRF_CtrlCmdSize);
	newppid=*(msg_buf+31);
		
////	setLine((BitAction)1);	//IO回应
////	uDelay(2);
////	setLine((BitAction)0);
////	
////	if(newppid!=data_pid)
////	{			
//////		if((newppid+1)==data_pid)
////			forcnt++;
////		data_pid=newppid;
////		
//////		if(writeIndex>=C_DATABUFCNT)
//////		{
//////			int test=0;
//////			test++;
//////		}		
////		//存储新包
////		u32 tmp=	C_DATABUFSIZE-dataIndex;
////		if(tmp>=C_PACKGEize)
////		{
////			ccopy((u8*) msg_buf,(u8*)&dataRbuf[dataIndex+(writeIndex&(C_DATABUFCNT-1))*C_DATABUFSIZE],C_PACKGEize);
////			dataIndex+=C_PACKGEize;
////		}
////		else
////		{
////			ccopy((u8*) msg_buf,(u8*)&dataRbuf[dataIndex+(writeIndex&(C_DATABUFCNT-1))*C_DATABUFSIZE],tmp);
////			dataIndex=0;
////			writeIndex++;
////			msg_buf+=tmp;
////			tmp=C_PACKGEize-tmp;
////			ccopy((u8*) msg_buf,(u8*)&dataRbuf[(writeIndex&(C_DATABUFCNT-1))*C_DATABUFSIZE],tmp);
////			dataIndex+=tmp;
////		}
////		if(dataIndex>=C_DATABUFSIZE)
////		{
////			dataIndex=0;
////			writeIndex++;
////		}
////		
//////		ccopy((u8*) msg_buf,(u8*)&dataRbuf[(writeIndex&(C_DATABUFCNT-1))*C_DATABUFSIZE],C_DATABUFSIZE);
//////		writeIndex++;
////		
////	}	

	
	
	u8 nextid=data_pid+1;
	if((newppid==nextid)||(newppid==data_pid))
	{			
		setLine((BitAction)1);	//IO回应
		uDelay(2);
		setLine((BitAction)0);
	}
	if(newppid==nextid)
	{			
		data_pid=newppid;
//		if((newppid+1)==data_pid)
			forcnt++;
			
		//存储新包
		u32 tmp=	C_DATABUFSIZE-dataIndex;
		if(tmp>=C_PACKGEize)
		{
			ccopy((u8*) msg_buf,(u8*)&dataRbuf[dataIndex+(writeIndex&(C_DATABUFCNT-1))*C_DATABUFSIZE],C_PACKGEize);
			dataIndex+=C_PACKGEize;
		}
		else
		{
			ccopy((u8*) msg_buf,(u8*)&dataRbuf[dataIndex+(writeIndex&(C_DATABUFCNT-1))*C_DATABUFSIZE],tmp);
			dataIndex=0;
			writeIndex++;
			msg_buf+=tmp;
			tmp=C_PACKGEize-tmp;
			ccopy((u8*) msg_buf,(u8*)&dataRbuf[(writeIndex&(C_DATABUFCNT-1))*C_DATABUFSIZE],tmp);
			dataIndex+=tmp;
		}
		if(dataIndex>=C_DATABUFSIZE)
		{
			dataIndex=0;
			writeIndex++;
		}
		
//		ccopy((u8*) msg_buf,(u8*)&dataRbuf[(writeIndex&(C_DATABUFCNT-1))*C_DATABUFSIZE],C_DATABUFSIZE);
//		writeIndex++;
		
	}	
	
	EXTI_ClearITPendingBit(EXTI_Line6);
	nRF_ClearStatusRegister();
	nRF_RecieveMode();
	 
}




