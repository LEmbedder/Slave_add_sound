
#include "MMA7660FC.h"
#include "misc.h"
#include "CMOS.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"


enum I2C_ACK {
	i2c_ak = 0,
	i2c_nak
};

#define SDA_HIGH()    GPIOB->BSRR = GPIO_Pin_0
#define SDA_LOW()     GPIOB->BRR = GPIO_Pin_0
#define SCL_HIGH()    GPIOB->BSRR = GPIO_Pin_1
#define SCL_LOW()     GPIOB->BRR = GPIO_Pin_1
#define SDA_READ()    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)


// MMA7660FC registers
#define WRITE_ADDR      0x98
#define READ_ADDR       0x99
#define XOUT       0x00
#define YOUT       0x01
#define ZOUT       0x02
#define TILT       0x03
#define SRST       0x04
#define SPCNT      0x05
#define INTSU      0x06
#define MODE       0x07
#define SR         0x08
#define PDET       0x09
#define PD         0x0a


// Tilt status register
#define BaFro_Mask     0x03
#define PoLa_Mask      0x1c
#define Tap_Mask       0x20
#define Shake_Mask     0x80
#define BaFro_Unknown  0x00
#define BaFro_Front    0x01
#define BaFro_Back     0x02
#define PoLa_Unknown   0x00
#define PoLa_Left      0x04
#define PoLa_Right     0x08
#define PoLa_Down      0x14
#define PoLa_Up        0x18
#define Tap_Set        0x20
#define Tap_Reset      0x00
#define Shake_Set      0x80
#define Shake_Reset    0x00


// Interrupt setup register
#define FBINT    0x01
#define PLINT    0x02
#define PDINT    0x04
#define ASINT    0x08
#define GINT     0x10
#define SHINTX   0x80
#define SHINTY   0x40
#define SHINTZ   0x20


// Mode register
#define Mode_StandBy               0x00
#define Mode_Test                  0x04
#define Mode_Active                0x01
#define AutoWake                   0x08
#define AutoSleep                  0x10
#define SleepCounterPrescaler_1    0x00
#define SleepCounterPrescaler_16   0x20
#define INT_OD                     0x00
#define INT_PP                     0x40
#define INT_ActiveLow              0x00
#define INT_ActiveHigh             0x80


// Sample rate register
#define RunSample_120      0x00
#define RunSample_64       0x01
#define RunSample_32       0x02
#define RunSample_16       0x03
#define RunSample_8        0x04
#define RunSample_4        0x05
#define RunSample_2        0x06
#define RunSample_1        0x07
#define SleepSample_32     0x00
#define SleepSample_16     0x08
#define SleepSample_8      0x10
#define SleepSample_1      0x18
/* Dobounce sample for Portrait/Landscape. */
#define DebounceSample_1   0x00
#define DebounceSample_2   0x20
#define DebounceSample_3   0x40
#define DebounceSample_4   0x60
#define DebounceSample_5   0x80
#define DebounceSample_6   0xa0
#define DebounceSample_7   0xc0
#define DebounceSample_8   0xe0


// Tap/Pulse detection register
#define Tap_X_Enable    0x00
#define Tap_X_Disable   0x20
#define Tap_Y_Enable    0x00
#define Tap_Y_Disable   0x40
#define Tap_Z_Enable    0x00
#define Tap_Z_Disable   0x80


#define G_VALUE_COUNT    33

#define S_UPDATE     1
#define S_READ       0

static volatile s8 _x = 0, _y = 0, _z = 0;
static volatile u8 _xyz_state = S_READ;
#define SHAKE_SAMPLE_SIZE    30
static s8 _x_buf[SHAKE_SAMPLE_SIZE];
static s8 _y_buf[SHAKE_SAMPLE_SIZE];
static s8 _z_buf[SHAKE_SAMPLE_SIZE];
static int _index = 0;
static float _g_value[G_VALUE_COUNT] = {0.000, 
		0.047, 0.094, 0.141, 0.188, 0.234, 0.281, 0.328, 0.375, 0.422, 0.469,
		0.516, 0.563, 0.609, 0.656, 0.703, 0.750, 0.797, 0.844, 0.891, 0.938,
		0.984, 1.031, 1.078, 1.125, 1.172, 1.219, 1.266, 1.313, 1.359, 1.406,
		1.453, 1.500};
static s8 _angle[G_VALUE_COUNT] = {0,
		3,  5,  8,  11, 14, 16, 19, 22, 25, 28,
		31, 34, 38, 41, 45, 49, 53, 58, 63, 70,
		80, 90, 90, 90, 90, 90, 90, 90, 90, 90,
		90, 90};

#define SHAKE_THRESHOLD    28     // 1.3g
#define SHAKE_RATE         4


static void MMA7660FC_GPIO_Init(void);
static void MMA7660FC_INT_Init(void);

static void I2C_Start(void);
static void I2C_Stop(void);
static u8 I2C_W(u8 val);
static u8 I2C_R(enum I2C_ACK ack);

static void Write_Reg(u8 reg, u8 val);
static u8 Read_Reg(u8 reg);
static void Write_Reg_Queue(u8 reg, u8 *buf, int size);
static void Read_Reg_Queue(u8 reg, u8 *buf, int size);

static void Delay(void);

#include "Display.h"
void MMA7660FC_Init(void)
{
	u8 mode, spcnt, sr, intsu, pdet;
	
	MMA7660FC_GPIO_Init();
	
	mode = Mode_Active | SleepCounterPrescaler_16 | INT_PP | INT_ActiveHigh;
	spcnt = 20 * 16 / 16;   // sleep count = N(s) * SampleRate / Prescaler_16
	sr = RunSample_64 | SleepSample_8;
	intsu = GINT;
	pdet = Tap_X_Disable | Tap_Y_Disable | Tap_Z_Disable;

	Write_Reg(MODE, Mode_StandBy);
	Write_Reg(SPCNT, spcnt);
	Write_Reg(SR, sr);
	Write_Reg(INTSU, intsu);
	Write_Reg(PDET, pdet);
	Write_Reg(MODE, mode);
	
	if ((Read_Reg(MODE) != mode) ||
	    (Read_Reg(SPCNT) != spcnt) ||
		(Read_Reg(SR) != sr) ||
		(Read_Reg(INTSU) != intsu) ||
		(Read_Reg(PDET) != pdet)) {
			Display_Initialization();
			PutString(0, 0, (u8 *)"gsensor fail");
			RefreshScreen();
			mDelay(2000);
	}
	
	MMA7660FC_INT_Init();
	
	Read_Reg(TILT);
	_index = 0;
}

void MMA7660FC_Standy(void)
{
	u8 mode, sr, intsu;
	
	mode = Mode_Active | SleepCounterPrescaler_16 | INT_PP | INT_ActiveHigh;
	sr = RunSample_1 | SleepSample_1;
	intsu = SHINTX | SHINTY | SHINTZ;

	Write_Reg(MODE, Mode_StandBy);
	Write_Reg(SR, sr);
	Write_Reg(INTSU, intsu);
	Write_Reg(MODE, mode);
	
	Read_Reg(TILT);
}

void MMA7660FC_ClearIntFlag(void)
{
	Read_Reg(MODE);
}

void MMA7660FC_Standby(void)
{
	Write_Reg(MODE, Mode_StandBy);
	Write_Reg(INTSU, 0);
	Read_Reg(MODE);
}

u32 rockcnt=0;

void EXTI0_IRQHandler(void)
{
	u8 buf[3];
	int i;
	int last;
	
	EXTI_ClearITPendingBit(EXTI_Line0);
	
	for (i = 0; i < 5; i++) {
		Read_Reg_Queue(XOUT, buf, 3);
		if (((buf[0] & 0x40) == 0) &&
		    ((buf[1] & 0x40) == 0) &&
			((buf[2] & 0x40) == 0)) {
			break;
		}
	}
	
	_x = (s8)(buf[0] & 0x1f);
	if (buf[0] & 0x20)
		_x = _x - 0x20;
		
	_y = (s8)(buf[1] & 0x1f);
	if (buf[1] & 0x20)
		_y = _y - 0x20;
		
	_z = (s8)(buf[2] & 0x1f);
	if (buf[2] & 0x20)
		_z = _z - 0x20;
		
	_xyz_state = S_UPDATE;
	
	_index = _index + 1;
	if (_index >= SHAKE_SAMPLE_SIZE)
		_index = 0;
	_x_buf[_index] = _x;
	_y_buf[_index] = _y;
	_z_buf[_index] = _z;
	
	// add by bqli 2016/01/09
	if(0==_index)
		last=SHAKE_SAMPLE_SIZE;
	else
		last=_index;
	last-=1;
	
	if((_x_buf[_index]*_x_buf[last]<0) || (_y_buf[_index]*_y_buf[last]<0) || (_z_buf[_index]*_z_buf[last]<0))
		rockcnt++;
	else
	{
		
	}
	//end
	
}


u8 MMA7660FC_IsRun(void)
{
	u8 srst;
	
	srst = Read_Reg(SRST);
	
	if (srst & 0x01)
		return 1;
	else
		return 0;
}

u8 MMA7660FC_IsSleep(void)
{
	u8 srst;
	
	srst = Read_Reg(SRST);
	
	if (srst & 0x02)
		return 1;
	else
		return 0;
}

static int GetShakeRate(s8 *buf, int size, u8 *shake)
{
	int i;
	int high = 0, low = 0;
	
	for (i = 0; i < size; i++) {
		if (buf[i] >= SHAKE_THRESHOLD)
			high = high + 1;
		else if (buf[i] <= -SHAKE_THRESHOLD)
			low = low + 1;
		else
			;
	}
	
	if ((high >= 1) && (low >= 1))
		*shake = 1;
	else
		*shake = 0;
	
	return ((high > low) ? low : high);
}

void MMA7660FC_GetShakeState(struct MMA7660FC_ShakeState *ss)
{
	int x_rate, y_rate, z_rate;
	int max_rate;
	u8 x_shake, y_shake, z_shake;
	
	x_rate = GetShakeRate(_x_buf, SHAKE_SAMPLE_SIZE, &x_shake);
	y_rate = GetShakeRate(_y_buf, SHAKE_SAMPLE_SIZE, &y_shake);
	z_rate = GetShakeRate(_z_buf, SHAKE_SAMPLE_SIZE, &z_shake);
	
	max_rate = (x_rate > y_rate) ? x_rate : y_rate;
	max_rate = (max_rate > z_rate) ? max_rate : z_rate;
	
	ss->time = CMOS_GetTimer();
	
	if (max_rate >= SHAKE_RATE) {
		ss->state = MMA7660FC_SHAKE2;
	} else if (x_shake | y_shake | z_shake) {
		ss->state = MMA7660FC_SHAKE;
	} else {
		ss->state = MMA7660FC_STILL;
	}
}

void MMA7660FC_XYZ(s8 *x, s8 *y, s8 *z)
{
	_xyz_state = S_READ;
	
	*x = _x;
	*y = _y;
	*z = _z;
	
	if (_xyz_state == S_UPDATE) {
		*x = _x;
		*y = _y;
		*z = _z;
	}
}

float MMA7660FC_To_Acceleration(s8 value)
{
	if (value >= 0)
		return _g_value[value];
	else
		return -_g_value[-value];
}

s8 MMA7660FC_To_Angle(s8 value)
{
	if (value >= 0)
		return _angle[value];
	else
		return -_angle[-value];
}


static void MMA7660FC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;     // SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;     // SCL
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;     // INT
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SDA_HIGH();
	SCL_HIGH();
}

void poweron(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;     // power contrl
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIOC->BSRR = GPIO_Pin_15;	//open
	
}

void poweroff(void)
{
	GPIOC->BRR = GPIO_Pin_15;
}

static void SDA_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;     // SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void SDA_In(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;     // SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void MMA7660FC_INT_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	MMA7660FC_Priority();
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void Write_Reg(u8 reg, u8 val)
{
	I2C_Start();
	I2C_W(WRITE_ADDR);
	I2C_W(reg);
	I2C_W(val);
	I2C_Stop();
}

static u8 Read_Reg(u8 reg)
{
	u8 val;
	
	I2C_Start();
	I2C_W(WRITE_ADDR);
	I2C_W(reg);
	I2C_Start();
	I2C_W(READ_ADDR);
	val = I2C_R(i2c_nak);
	I2C_Stop();
	
	return val;
}

static void Write_Reg_Queue(u8 reg, u8 *buf, int size)
{
	int i;
	
	I2C_Start();
	I2C_W(WRITE_ADDR);
	I2C_W(reg);
	for (i = 0; i < size; i++)
		I2C_W(buf[i]);
	I2C_Stop();
}

static void Read_Reg_Queue(u8 reg, u8 *buf, int size)
{
	int i;
	
	I2C_Start();
	I2C_W(WRITE_ADDR);
	I2C_W(reg);
	I2C_Start();
	I2C_W(READ_ADDR);
	for (i = 0; i < size - 1; i++)
		buf[i] = I2C_R(i2c_ak);
	buf[i] = I2C_R(i2c_nak);
	I2C_Stop();
}

static void I2C_Start(void)
{
	SDA_HIGH();
	SCL_HIGH();
	Delay();
	SDA_LOW();
	Delay();
	SCL_LOW();
}

static void I2C_Stop(void)
{
	SDA_LOW();
	SCL_HIGH();
	Delay();
	SDA_HIGH();
}

static u8 I2C_W(u8 val)
{
	u8 i;
	u8 ack;
	
	SCL_LOW();
	
	for (i = 0; i < 8; i++) {
		if (val & 0x80)
			SDA_HIGH();
		else
			SDA_LOW();
			
		Delay();
		SCL_HIGH();
		Delay();
		SCL_LOW();
		
		val = val << 1;
	}
	
	SDA_In();
	
	SDA_HIGH();
	Delay();

	SCL_HIGH();
	Delay();
	ack = SDA_READ();
	SCL_LOW();
	
	SDA_Out();
	
	SDA_LOW();
	Delay();
	
	return ack;
}

static u8 I2C_R(enum I2C_ACK ack)
{
	u8 i;
	u8 val;
	
	SDA_In();
	
	SDA_HIGH();
	
	for (i = 0; i < 8; i++) {
		val = val << 1;
		
		SCL_LOW();
		Delay();
		SCL_HIGH();
		Delay();
		
		val = val + SDA_READ();
	}
	
	SCL_LOW();
	
	SDA_Out();
	
	if (ack == i2c_ak)	
		SDA_LOW();    // ack
	else
		SDA_HIGH();   // no ack
	Delay();
	
	SCL_HIGH();
	Delay();
	SCL_LOW();
	
	SDA_LOW();
	Delay();
	
	return val;
}

static void Delay(void)
{
	u16 i;
	
	i = 10;
	while(i--);
}

