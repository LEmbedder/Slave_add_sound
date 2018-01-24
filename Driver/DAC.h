/*
 * Filename:	DAC.h
 * Author:		ZouChao, C.M.16
 * Date:		2015-07-15
 * Version:		v0.2
 * Discription:	dac driver
 */

#ifndef __DAC_H_
#define __DAC_H_

#include "stm32f10x.h"

/*DAC Initialization*/
void DAC_Initialization(void);
/*DAC Output*/
void DAC_OutPut(u16 Data);


#endif/*__DAC_H_*/

