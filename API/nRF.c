/*
 * Filename:	nRF.c
 * Author:		ZouChao, C.M.16
 * Version	Date		
 * 	v0.1 	2015-06-17		New built
 *	v0.2 	2015-06-22		Add task fifo 
 * Discription:	nRF api
 */

#include "nRF.h"
#include "CMOS.h"
#include "CMLIB.h"
#include "File.h"
#include "GEngineCfg.h"
#include "display.h"

#define C_NRF_IrqCheck_Polling			0
#define C_NRF_IrqCheck_Interrupt		1

volatile u8 R_nRF_IrqCheck;

volatile STRUCT_FIFO_REG R_nRF_Fifo;
volatile STRUCT_nRF_CMD R_nRF_Buffer[C_nRF_Fifo_Size];

/*nRF Initialization*/
void nRF_Initialization(void)
{
	FIFO_Init((PSTRUCT_FIFO_REG)&R_nRF_Fifo, C_nRF_Fifo_Size);
	nRF_Init();
	if (nRF_Check() != 0)		
	{
		Console_Open((u8 *)"2.4g mode fail");
		mDelay(2000);
		SystemReset();
	}
}

/*Get nRF Message*/
BOOL nRF_GetMessage(u8 *buf)
{
	u8 i, idx;
	BOOL flg;

	flg = FIFO_GetPopIndex((PSTRUCT_FIFO_REG)&R_nRF_Fifo, &idx);
	if(flg == TRUE)
	{
		for(i = 0; i < C_nRF_CtrlCmdSize; i++)
		{
			buf[i] = R_nRF_Buffer[idx].para[i];
		}
	}

	return flg;
}

/*Set nRF Address*/
extern u8 R_nRF_Addr[5];
void nRF_SetAddress(u8 *pAddr)
{
	u8 i;

	for(i = 0; i < 5; i++)
	{
		R_nRF_Addr[i] = pAddr[i];
	}
}

///*Check nRF Message*/
//BOOL nRF_CheckMessage(u8 *buf, u8 size)
//{
//	BOOL flg;

//	if(nRF_GetIRQStatus() == 0)
//	{
//		nRF_StandbyMode();
//		nRF_RxGetData(buf, size);
//		nRF_ClearStatusRegister();
//		flg = TRUE;
//	}
//	else
//	{
//		flg = FALSE;
//	}

//	return flg;
//}


//void nRF_ClearMessage(void)
//{
//	FIFO_Init((PSTRUCT_FIFO_REG)&R_nRF_Fifo, C_nRF_Fifo_Size);
//}

////BOOL nRF_SendGetMSG(u8 *send,u8 *recev)
////{
////	BOOL ret;
////	u32 tcnt=CMOS_GetTimer();
////	while(1)
////	{
////		ret=nRF_SendMessage(send, 32);
////		if(TRUE==ret)
////		{
////			nRF_ClearStatusRegister();
////			nRF_RecieveMode();
////			while(1)
////			{
////				if(0==nRF_GetIRQStatus())
////				{
////					nRF_StandbyMode();
////					nRF_RxGetData((u8 *)recev, 32);
////					nRF_ClearStatusRegister();
////					return TRUE;
////				}
////				if(tcnt-CMOS_GetTimer()>=20)
////					break;
////			}			
////		}else
////		{
////			mDelay(2);
////		}
////		if(tcnt-CMOS_GetTimer()>=100)
////			return FALSE;
////	}
////}

////int nRF_GetUID(u32 *AddrL,u32 *AddrH)
////{
////	BOOL ret;
////	u32 sendmsg[8];
////	volatile u32 recmsg[8];
////	u8 addr[5] = {0x00, 0x00, 0x00, 0x00, 0x01};
////	int i;
////	u32 tcnt;
////	u8 *p;

////	sendmsg[0]=0xaabbccdd;
////	sendmsg[1]=*(vu32*)(0x1ffff7e8);
////	sendmsg[2]=*(vu32*)(0x1ffff7ec);
////	sendmsg[3]=*(vu32*)(0x1ffff7f0);
////	nRF_SetAddress(addr);
////	nRF_TxSetReg(addr, C_nRF_CtrlCmdSize);
////	tcnt=CMOS_GetTimer();	
////	while (tcnt-CMOS_GetTimer()<=1000) {
////		ret=nRF_SendGetMSG((u8 *)&sendmsg, (u8 *)recmsg);	//-->0xaabbccdd ID  <--0xaabb00dd ID ADDR
////		if(FALSE==ret)
////			continue;
////		for(i=0;i<4;i++)
////		{
////			if(sendmsg[i]!=recmsg[i])
////				break;
////		}
////		if(i>=4)	
////		{
////			p=(u8 *)&recmsg[4];
////			p+=3;
////			nRF_SetAddress(p);
////			nRF_TxSetReg(p, C_nRF_CtrlCmdSize);
////			sendmsg[0]=0x41434445;
////			sendmsg[4]=recmsg[4];	//ADDRH
////			sendmsg[5]=recmsg[5];	//ADDRL
////		}else
////		{
////			continue;
////		}
////		mDelay(2);
////		ret=nRF_SendGetMSG((u8 *)&sendmsg, (u8 *)recmsg);	//-->0x41434445 ID ADDR <--0x41434445 ID ADDR
////		if(FALSE==ret)
////			ret=nRF_SendGetMSG((u8 *)&sendmsg, (u8 *)recmsg);	//-->0x41434445 ID ADDR <--0x41434445 ID ADDR
////		if(FALSE==ret)
////		{
////			nRF_SetAddress(addr);
////			nRF_TxSetReg(addr, C_nRF_CtrlCmdSize);
////			continue;
////		}
////		for(i=0;i<6;i++)
////		{
////			if(sendmsg[i]!=recmsg[i])
////				break;
////		}
////		if(i>=6)	
////		{
////			p=(u8 *)&recmsg[4];
////			p+=3;
////			*AddrH=recmsg[4];	//ADDRH
////			*AddrL=recmsg[5];	//ADDRL
////			return 0;
////		}else
////		{
////			nRF_SetAddress(addr);
////			nRF_TxSetReg(addr, C_nRF_CtrlCmdSize);
////		}
////		
////	}
////	return 1;
////}



