
#ifndef __SINGLEWIRESLAVE_H
#define __SINGLEWIRESLAVE_H

#include "stm32f10x.h"

#define SEND_BUF_SIZE      20
extern u8 _send_buf[SEND_BUF_SIZE];

int SWS_Init(void);

u32 SWS_RecvBufSize(void);
u32 SWS_BytesToRead(void);
/* ����ʵ�ʶ�ȡ�����ֽ��� */
u32 SWS_ReadBytes(u8 *buf, u32 size);

u32 SWS_SendBufSize(void);
u32 SWS_BytesToSend(void);
/* ������� buf �� SWS_SetSendBuf() �е� send_buf ����ͬһ����ַ��
   ����� buf �е����ݻᱻ������ send_buf ��ַ�� */
void SWS_SendBytes(u8 *buf, u32 size);

/* ������� recv_buf==0, �����ڲ��� buf */
u32 SWS_SetRecvBuf(u8 *recv_buf, u32 size);
/* ������� send_buf==0, �����ڲ��� buf */
u32 SWS_SetSendBuf(u8 *send_buf, u32 size);
void SWS_getBytes(u8 *buf, u32 size);
#endif
