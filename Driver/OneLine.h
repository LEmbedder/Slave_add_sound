
#ifndef __ONELINE_H_
#define __ONELINE_H_

#include "stm32f10x.h"
#include "CMLIB.h"
#include "stm32f10x_gpio.h"

void ONELINE_Init(void);
u8 getLine(void);
void setLine(BitAction BitVal);

#endif /*__IC_SPI_H_*/
