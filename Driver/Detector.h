
#ifndef __DETECTOR_H
#define __DETECTOR_H

#include "stm32f10x.h"

enum Box_Side {
	box_up = 0,
	box_left,
	box_right,
	box_down,
	box_invaliad
};

void ADC2_GPIO_Cmd(FunctionalState newState);
void Detector_Init(enum Box_Side side);
void Detector_Close(void);
u16 Detector_GetValue(void);
u8 Detector_GetAverageValue(void);
void NearSensorServer(void);

void Detector_Analyse(void);
u8 Detector_Up_Status(void);
u8 Detector_Down_Status(void);
u8 Detector_Left_Status(void);
u8 Detector_Right_Status(void);


#endif

