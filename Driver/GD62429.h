#ifndef __GD62429_H_
#define __GD62429_H_
#include "CMOS.h"

#define   _0dB  0X35F
#define   _20dB  0X21F
#define   _32dB  0X36F


#define   _40dB  0X3AF



void GD62429_GPIO_Init(void);

u8 GD_I2C_W(u16 val);

u8 gd_I2C_W(void);
#endif


