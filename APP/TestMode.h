/*
 * Filename:	Test.h
 * Author:		ZouChao, C.M.16
 * Date:		2015-06-02
 * Version:		v0.1
 * Discription:	test app
 */

#ifndef __TEST_MODE_H_
#define __TEST_MODE_H_

#include "stm32f10x.h"

#define C_TstMode_Lcd		0
#define C_TstMode_GSensor	1
#define C_TstMode_DAC		2
#define C_TstMode_USART		3
#define C_TstMode_Motor     4
#define C_TstMode_Pcm       5
#define C_TstMode_Power     6
#define C_TstMode_nRF       7
#define C_TstMode_Detector  8
#define C_TstMode_GSensor2  9
#define C_TstMode_24G_Master   10
#define C_TstMode_24G_Slave    11
#define C_TstMode_Power_Time   12
#define C_TstMode_MS_Master    13
#define C_TstMode_GSensor_Key  14
#define C_TstMode_MMA7660FC    15



/*Test Mode Main Function*/
u8 TestMode(u8 para);


#endif /*__TEST_MODE_H_*/
