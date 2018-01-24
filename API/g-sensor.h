
#ifndef __G_SENSOR_H
#define __G_SENSOR_H

#include "stm32f10x.h"

enum gSensorDirection {
	g_x_positive = 0,
	g_x_negative,
	g_y_positive,
	g_y_negative,
	g_z_positive,
	g_z_negative
};

#define G_BOX_TOP      g_z_negative
#define G_BOX_FRONT    g_y_negative


enum gWaveRank {
	g_wave_unknown = 0,
	g_wave_static,
	g_wave_1,
	g_wave_2,
};

enum gBoxSurface {
	g_box_unknown = 0,
	g_box_up,
	g_box_down,
	g_box_left,
	g_box_right,
	g_box_front,
	g_box_back
};


void gSensor_Init(void);

void gSensor_Update(void);

enum gWaveRank gSensor_WaveRank(void);
u32 gSensor_LastShakeTime(void);

enum gBoxSurface gSensor_TopSurface(void);

s8 gSensor_X(void);
s8 gSensor_Y(void);
s8 gSnesor_Z(void);
s8 gSensor_X_Angle(void);
s8 gSensor_Y_Angle(void);
s8 gSensor_Z_Angle(void);
float gSensor_X_Acceleration(void);
float gSensor_Y_Acceleration(void);
float gSensor_Z_Acceleration(void);

s8 gSensor_Box_X(void);
s8 gSensor_Box_Y(void);
s8 gSensor_Box_Z(void);
s8 gSensor_Box_X_Angle(void);
s8 gSensor_Box_Y_Angle(void);
s8 gSensor_Box_Z_Angle(void);
float gSensor_Box_X_Acceleration(void);
float gSensor_Box_Y_Acceleration(void);
float gSensor_Box_Z_Acceleration(void);

void Get_Box_XYZ(enum gSensorDirection top, enum gSensorDirection front, 
                 s8 x, s8 y, s8 z, s8 *box_x, s8 *box_y, s8 *box_z);


#endif
