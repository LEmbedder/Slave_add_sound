
#include "g-sensor.h"
#include "MMA7660FC.h"
#include "CMOS.h"


static s8 g_x = 0, g_y = 0, g_z = 0;
static s8 g_box_x = 0, g_box_y = 0, g_box_z = 0;
static struct MMA7660FC_ShakeState g_ss;
static u32 last_shake_time = 0;

void gSensor_Init(void)
{
	MMA7660FC_Init();
}


void gSensor_Update(void)
{
	MMA7660FC_XYZ(&g_x, &g_y, &g_z);
	MMA7660FC_GetShakeState(&g_ss);
	Get_Box_XYZ(G_BOX_TOP, G_BOX_FRONT,
	            g_x, g_y, g_z, &g_box_x, &g_box_y, &g_box_z);
}


enum gWaveRank gSensor_WaveRank(void)
{
	if (g_ss.state == MMA7660FC_SHAKE2) {
		last_shake_time = g_ss.time;
		return g_wave_2;
	} else if (g_ss.state == MMA7660FC_SHAKE) {
		last_shake_time = g_ss.time;
		return g_wave_1;
	} else {
		return g_wave_static;
	}
}

u32 gSensor_LastShakeTime(void)
{
	return last_shake_time;
}

enum gBoxSurface gSensor_TopSurface(void)
{
	u8 x_abs, y_abs, z_abs;
	
	x_abs = (g_box_x >= 0) ? g_box_x : -g_box_x;
	y_abs = (g_box_y >= 0) ? g_box_y : -g_box_y;
	z_abs = (g_box_z >= 0) ? g_box_z : -g_box_z;
	
	if ((z_abs >= x_abs) && (z_abs >= y_abs))	{
		if (g_box_z > 0)
			return g_box_up;
		else
			return g_box_down;
	} else if ((x_abs >= y_abs) && (x_abs >= z_abs)) {
		if (g_box_x > 0)
			return g_box_right;
		else
			return g_box_left;
	} else if ((y_abs >= x_abs) && (y_abs >= z_abs)) {
		if (g_box_y > 0)
			return g_box_back;
		else
			return g_box_front;
	} else {
		return g_box_unknown;
	}
}

s8 gSensor_X(void)
{
	return g_x;
}

s8 gSensor_Y(void)
{
	return g_y;
}

s8 gSnesor_Z(void)
{
	return g_z;
}

s8 gSensor_X_Angle(void)
{
	return MMA7660FC_To_Angle(g_x);
}

s8 gSensor_Y_Angle(void)
{
	return MMA7660FC_To_Angle(g_y);
}

s8 gSensor_Z_Angle(void)
{
	return MMA7660FC_To_Angle(g_z);
}

float gSensor_X_Acceleration(void)
{
	return MMA7660FC_To_Acceleration(g_x);
}

float gSensor_Y_Acceleration(void)
{
	return MMA7660FC_To_Acceleration(g_y);
}

float gSensor_Z_Acceleration(void)
{
	return MMA7660FC_To_Acceleration(g_z);
}

s8 gSensor_Box_X(void)
{
	return g_box_x;
}

s8 gSensor_Box_Y(void)
{
	return g_box_y;
}

s8 gSensor_Box_Z(void)
{
	return g_box_z;
}

s8 gSensor_Box_X_Angle(void)
{
	return MMA7660FC_To_Angle(g_box_x);
}

s8 gSensor_Box_Y_Angle(void)
{
	return MMA7660FC_To_Angle(g_box_y);
}

s8 gSensor_Box_Z_Angle(void)
{
	return MMA7660FC_To_Angle(g_box_z);
}

float gSensor_Box_X_Acceleration(void)
{
	return MMA7660FC_To_Acceleration(g_box_x);
}

float gSensor_Box_Y_Acceleration(void)
{
	return MMA7660FC_To_Acceleration(g_box_y);
}

float gSensor_Box_Z_Acceleration(void)
{
	return MMA7660FC_To_Acceleration(g_box_z);
}

void Get_Box_XYZ(enum gSensorDirection top, enum gSensorDirection front, 
                 s8 x, s8 y, s8 z, s8 *box_x, s8 *box_y, s8 *box_z)
{
	*box_x = x;
	*box_y = y;
	*box_z = z;
	
	switch (top) {
	
	case g_x_positive:
		switch (front) {
		case g_y_positive:
			*box_x = z;
			*box_y = -y;
			*box_z = x;
			break;
			
		case g_y_negative:
			*box_x = -z;
			*box_y = y;
			*box_z = x;
			break;
			
		case g_z_positive:
			*box_x = -y;
			*box_y = -z;
			*box_z = x;
			break;
			
		case g_z_negative:
			*box_x = y;
			*box_y = z;
			*box_z = x;
			break;
			
		default:;
		}
		break;
		
	case g_x_negative:
		switch (front) {
		case g_y_positive:
			*box_x = -z;
			*box_y = -y;
			*box_z = -x;
			break;
			
		case g_y_negative:
			*box_x = z;
			*box_y = y;
			*box_z = -x;
			break;
			
		case g_z_positive:
			*box_x = y;
			*box_y = -z;
			*box_z = -x;
			break;
			
		case g_z_negative:
			*box_x = -y;
			*box_y = z;
			*box_z = -x;
			break;
			
		default:;
		}
		break;
		
	case g_y_positive:
		switch (front) {
		case g_x_positive:
			*box_x = -z;
			*box_y = -x;
			*box_z = y;
			break;
			
		case g_x_negative:
			*box_x = z;
			*box_y = x;
			*box_z = y;
			break;
			
		case g_z_positive:
			*box_x = x;
			*box_y = -z;
			*box_z = y;
			break;
			
		case g_z_negative:
			*box_x = -x;
			*box_y = z;
			*box_z = y;
			break;
			
		default:;
		}
		break;
		
	case g_y_negative:
		switch (front) {
		case g_x_positive:
			*box_x = z;
			*box_y = -x;
			*box_z = -y;
			break;
			
		case g_x_negative:
			*box_x = -z;
			*box_y = x;
			*box_z = -y;
			break;
			
		case g_z_positive:
			*box_x = -x;
			*box_y = -z;
			*box_z = -y;
			break;
			
		case g_z_negative:
			*box_x = x;
			*box_y = z;
			*box_z = -y;
			break;
			
		default:;
		}
		break;
		
	case g_z_positive:
		switch (front) {
		case g_x_positive:
			*box_x = y;
			*box_y = -x;
			*box_z = z;
			break;
			
		case g_x_negative:
			*box_x = -y;
			*box_y = x;
			*box_z = z;
			break;
			
		case g_y_positive:
			*box_x = -x;
			*box_y = -y;
			*box_z = z;
			break;
			
		case g_y_negative:
			*box_x = x;
			*box_y = y;
			*box_z = z;
			break;
			
		default:;
		}
		break;
		
	case g_z_negative:
		switch (front) {
		case g_x_positive:
			*box_x = -y;
			*box_y = -x;
			*box_z = -z;
			break;
			
		case g_x_negative:
			*box_x = y;
			*box_y = x;
			*box_z = -z;
			break;
			
		case g_y_positive:
			*box_x = x;
			*box_y = -y;
			*box_z = -z;
			break;
			
		case g_y_negative:
			*box_x = -x;
			*box_y = y;
			*box_z = -z;
			break;
			
		default:;
		}
		break;
		
	default:;
	}
}

