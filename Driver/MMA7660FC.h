
#ifndef __MMA7660FC_H_
#define __MMA7660FC_H_

#include "stm32f10x.h"
//
//#define MMA7660FC_MAX_X    (31)
//#define MMA7660FC_MIN_X    (-32)
//#define MMA7660FC_MAX_Y    (31)
//#define MMA7660FC_MIN_Y    (-32)
//#define MMA7660FC_MAX_Z    (31)
//#define MMA7660FC_MIN_Z    (-32)
//
//#define MMA7660FC_MAX_ACCELERATION     (1.500)
//#deifne MMA7660FC_MIN_ACCELERATION     (-1.500)
//
//#define MMA7660FC_MAX_ANGLE      (90)
//#define MMA7660FC_MIN_ANGLE      (-90)

#define MMA7660FC_STILL       1
#define MMA7660FC_SHAKE       2
#define MMA7660FC_SHAKE2      3

struct MMA7660FC_ShakeState {
	u8 state;
	u32 time;
};


void MMA7660FC_Init(void);
void MMA7660FC_Standy(void);

void EXTI0_IRQHandler(void);

u8 MMA7660FC_IsRun(void);
u8 MMA7660FC_IsSleep(void);

void MMA7660FC_GetShakeState(struct MMA7660FC_ShakeState *ss);

void MMA7660FC_XYZ(s8 *x, s8 *y, s8 *z);

float MMA7660FC_To_Acceleration(s8 value);
s8 MMA7660FC_To_Angle(s8 value);

void MMA7660FC_ClearIntFlag(void);
void MMA7660FC_Standby(void);

#endif

