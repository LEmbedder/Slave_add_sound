
#ifndef __BATTERY_H
#define __BATTERY_H

#include "stm32f10x.h"

#define C_POWER_LOW	0x880
#define C_POWER_WAR	0x920

void Bat_Init(void);
u8 Bat_GetChargeState(void);
u8 Bat_GetStandbyState(void);
float Bat_GetVoltage(void);
u8 Bat_GetVoltagePercentage(void);
u8 Bat_IsChargeDone(void);
u8 Bat_IsCharging(void);
u16 Bat_GetAD(void);
u8 Bat_GetLevel(void);



//#define C_SHUTDOWNPOWER 2000	//20161211
//#define C_LOWPOWER 2240
//#define C_LEVEL1_POWER 2360
//#define C_LEVEL2_POWER 2460      //3.81
//#define C_FULLPOWER 2560
//#define C_POWERSAMPLE 20


/////放电电压值      4个
#define F_SHUTDOWNPOWER 2060	   //20161211
#define F_LOWPOWER 2147
#define F_LEVEL1_POWER 2209      //2280
#define F_LEVEL2_POWER 2296      //2451
//#define F_FULLPOWER 2530       //2560                 比这个值大，电充满，结束充电
#define F_POWERSAMPLE 20



//////充电电压值      4个
#define C_SHUTDOWNPOWER 2040	    //20161211
#define C_LOWPOWER 2296
#define C_LEVEL1_POWER 2358      //2280
#define C_LEVEL2_POWER 2401    //2451
#define C_FULLPOWER 2416      //2560                 比这个值大，电充满，结束充电
#define C_POWERSAMPLE 20

#endif

