
#ifndef __FILE_H
#define __FILE_H

#include "stm32f10x.h"
#include "Resource.h"

struct Graph_Info {
	u32 addr;
	u32 paletteaddr;         //调色板地址
	u16 biBitCount;         //图片的位数
	u8 segoff;              //偏移量
	u8 bisize;              //位图信息图的大小（字节）
	u8 is_palette;
	u8 width;
	u8 height;
};



void File_SetBaseAddr(u32 addr,u32 gameaddr);

void File_Read(u32 addr, u8 *buf, u32 size);

void File_SetGraphBaseAddr(u32 base_addr);
u16 File_GetGraphSum(int subid);
void File_GetGraphInfo(u16 GraphID, struct Graph_Info *graph_struct);
//void File_GetGraphData(struct Graph_Info *graph_struct, u8 *buf);

void File_SetAudioBaseAddr(u32 base_addr);
u16 File_GetAudioSum(int subid);
u32 File_GetAudioAddr(u16 index);
u32 GetResourceItemAddr(enum ResourceID ObjID,int subID);
int GetResourceData(u8 *pbuf,int addr,int length);
int GetResourceDataP(u8 *pbuf,int *addr,int _length);
u32 GetResourceAddr(enum ResourceID ObjID,int subid);

#endif

