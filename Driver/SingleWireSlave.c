
#include "SingleWireSlave.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include "CMOS.h"


enum SWS_Mode {
	SWS_mode_input,
	SWS_mode_output
};

static void set_mode(enum SWS_Mode mode);


#define SWS_GPIOx       GPIOA
#define SWS_GPIO_Pin    GPIO_Pin_15

#define Out_High()   GPIO_SetBits(SWS_GPIOx, SWS_GPIO_Pin)
#define Out_Low()    GPIO_ResetBits(SWS_GPIOx, SWS_GPIO_Pin)
#define Read_Bit()   GPIO_ReadInputDataBit(SWS_GPIOx, SWS_GPIO_Pin)


struct SWS_FIFO {
	u8 *pbuf;
	u32 size;
	u32 in;
	u32 out;
	u32 length;
};

static void fifo_set_buf(struct SWS_FIFO *fifo, u8 *buf, u32 size);
static void fifo_reset(struct SWS_FIFO *fifo);
static void fifo_write_buf(struct SWS_FIFO *fifo, u8 *buf, u32 size);
static u32 fifo_read_buf(struct SWS_FIFO *fifo, u8 *buf, u32 size);
static void fifo_write_byte(struct SWS_FIFO *fifo, u8 byte);
static u32 fifo_read_byte(struct SWS_FIFO *fifo, u8 *byte);


#define RECV_BUF_SIZE      20

static struct SWS_FIFO _recv, _send;
u8 _send_buf[SEND_BUF_SIZE]={0x5a};
static u8 _recv_buf[RECV_BUF_SIZE];
static u8 _send_byte, _recv_byte;
static u32 _last_time, _tmp;
static u8 _count_of_rising_edge;
static u8 _count_of_recv_bit;


#define _t_start          7
#define _t_bit            3
//#define _t_bit_end        2
#define delay_tsend()     uDelay(90)


int SWS_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	set_mode(SWS_mode_input);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//bqli EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	SWS_Priority();
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	fifo_set_buf(&_send, _send_buf, SEND_BUF_SIZE);
	fifo_reset(&_send);
	fifo_set_buf(&_recv, _recv_buf, RECV_BUF_SIZE);
	fifo_reset(&_recv);
	_count_of_rising_edge = 0;
	_count_of_recv_bit = 0;
	_last_time = CMOS_GetTimer();
	
	return 0;
}

//int irqcnt=0;

void EXTI15_10_IRQHandler(void)
{
	int cnt;
	if((EXTI_GetITStatus(EXTI_Line15) != RESET))
	{
//		irqcnt++;
//		cnt=irqcnt;
//		EXTI_ClearITPendingBit(EXTI_Line15);
//		return;
		
		
	_tmp = _last_time - CMOS_GetTimer();
	_last_time = _last_time - _tmp;
	
	if (_tmp >= _t_start) {
		_count_of_recv_bit = 0;
		_count_of_rising_edge = 1;
	}else if (_tmp >= _t_bit) {

			switch (_count_of_rising_edge) {
			case 3:
			case 4:
				_recv_byte = (_recv_byte << 1);
				break;
			case 6:
			case 7:
			case 8:
				_recv_byte = (_recv_byte << 1) + 1;
				break;
			default:
				_count_of_rising_edge=0;
			_count_of_recv_bit=0;
				EXTI_ClearITPendingBit(EXTI_Line15);
				return;
			}
			
			_count_of_recv_bit++;

			set_mode(SWS_mode_output);
			
			if (_send_byte & 0x80)
				Out_High();
			else
				Out_Low();
			_send_byte = _send_byte << 1;
			
			delay_tsend();
			set_mode(SWS_mode_input);
			
			_count_of_rising_edge = 0;
			
			if (_count_of_recv_bit==8) {
				fifo_write_byte(&_recv, _recv_byte);
				if (fifo_read_byte(&_send, &_send_byte) == 0) {
					_send_byte = 0x00;
				}
				_count_of_recv_bit = 0;
			}
	} else {
		_count_of_rising_edge++;
	}
	EXTI_ClearITPendingBit(EXTI_Line15);
	}
}

u32 SWS_RecvBufSize(void)
{
	return _recv.size;
}

u32 SWS_BytesToRead(void)
{
	return _recv.length;
}

//struct SWS_FIFO {
//	u8 *pbuf;
//	u32 size;
//	u32 in;
//	u32 out;
//	u32 length;
//};
void SWS_getBytes(u8 *buf, u32 size)
{
	u32 out=_recv.out;
	u8 *pbuf=_recv.pbuf;	
	u32 length=_recv.length;
	for(int i=0;((length)&&(i<size));i++)
	{
		*buf++=*(pbuf+out+i);
		length--;
		if(out+i>=_recv.size)
			pbuf-=_recv.size;
	}
}

u32 SWS_ReadBytes(u8 *buf, u32 size)
{
	return fifo_read_buf(&_recv, buf, size);
}

u32 SWS_SendBufSize(void)
{
	return _send.size;
}

u32 SWS_BytesToSend(void)
{
	return _send.length;
}

void SWS_SendBytes(u8 *buf, u32 size)
{
	if (_send.pbuf == buf) {
		_send.in = size;
		_send.out = 0;
		_send.length = size;
	} else {
		fifo_write_buf(&_send, buf, size);
	}
	if (fifo_read_byte(&_send, &_send_byte) == 0) {
		_send_byte = 0x00;
	}
}

u32 SWS_SetRecvBuf(u8 *recv_buf, u32 size)
{
	if (recv_buf == 0) {
		fifo_set_buf(&_recv, _recv_buf, RECV_BUF_SIZE);
	} else {
		fifo_set_buf(&_recv, recv_buf, size);
	}
	
	fifo_reset(&_recv);
	
	return _recv.size;
}

u32 SWS_SetSendBuf(u8 *send_buf, u32 size)
{
	if (send_buf == 0) {
		fifo_set_buf(&_send, _send_buf, SEND_BUF_SIZE);
	} else {
		fifo_set_buf(&_send, send_buf, size);
	}
	
	fifo_reset(&_send);
	
	return _send.size;
}

static void set_mode(enum SWS_Mode mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if (mode == SWS_mode_input)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	else
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = SWS_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(SWS_GPIOx, &GPIO_InitStructure);
}


static void fifo_set_buf(struct SWS_FIFO *fifo, u8 *buf, u32 size)
{
	fifo->pbuf = buf;
	fifo->size = size;
}

static void fifo_reset(struct SWS_FIFO *fifo)
{
	fifo->in = 0;
	fifo->length = 0;
	fifo->out = 0;
}

static void fifo_write_buf(struct SWS_FIFO *fifo, u8 *buf, u32 size)
{
	u32 i;
	
	for (i = 0; i < size; i++) {
		fifo_write_byte(fifo, buf[i]);
	}
}

static u32 fifo_read_buf(struct SWS_FIFO *fifo, u8 *buf, u32 size)
{
	u32 i;
	
	for (i = 0; i < size; i++) {
		if (fifo_read_byte(fifo, &buf[i]) == 0) {
			break;
		}
	}
	
	return i;
}

static void fifo_write_byte(struct SWS_FIFO *fifo, u8 byte)
{
	fifo->pbuf[fifo->in++] = byte;
	
	if (fifo->in >= fifo->size)
		fifo->in = 0;
	
	if ((fifo->size - fifo->length) >= 1) {
		fifo->length++;
	} else {
		fifo->out = fifo->in;
	}
}

static u32 fifo_read_byte(struct SWS_FIFO *fifo, u8 *byte)
{
	if (fifo->length > 0) {
		*byte = fifo->pbuf[fifo->out++];
		
		if (fifo->out >= fifo->size)
			fifo->out = 0;
		
		fifo->length--;
		
		return 1;
	} else {
		return 0;
	}
}
