
#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

enum Motor_Type {
	motor_on_1s = 0,
};
void Motor_Init(void);
void Motor_On(void);
void Motor_Off(void);
u8 getMotorFlag(void);
void Motor_Action(enum Motor_Type type);
void Motor_Check(void);


#endif

