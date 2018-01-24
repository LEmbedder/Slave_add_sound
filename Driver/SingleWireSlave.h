
#ifndef __SINGLEWIRESLAVE_H
#define __SINGLEWIRESLAVE_H

#include "stm32f10x.h"

#define SEND_BUF_SIZE      20
extern u8 _send_buf[SEND_BUF_SIZE];

int SWS_Init(void);

u32 SWS_RecvBufSize(void);
u32 SWS_BytesToRead(void);
/* 返回实际读取到的字节数 */
u32 SWS_ReadBytes(u8 *buf, u32 size);

u32 SWS_SendBufSize(void);
u32 SWS_BytesToSend(void);
/* 如果参数 buf 与 SWS_SetSendBuf() 中的 send_buf 不是同一个地址，
   则参数 buf 中的内容会被拷贝到 send_buf 地址。 */
void SWS_SendBytes(u8 *buf, u32 size);

/* 如果参数 recv_buf==0, 则用内部的 buf */
u32 SWS_SetRecvBuf(u8 *recv_buf, u32 size);
/* 如果参数 send_buf==0, 则用内部的 buf */
u32 SWS_SetSendBuf(u8 *send_buf, u32 size);
void SWS_getBytes(u8 *buf, u32 size);
#endif
