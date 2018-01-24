
#include "File.h"
#include "Flash.h"
#include "Commu.h"
#include "Resource.h"
#include "FLASHROM.h"

static str_Resource str_data_base;
static str_Resource str_game_base;

#define graph_base_addr (str_data_base.graph_base_addr)
#define audio_base_addr (str_data_base.audio_base_addr)

void File_SetBaseAddr(u32 addr,u32 gameaddr)
{
	int i;
	u32 *p;
	Flash_Read(addr, (u8 *)&str_data_base, sizeof(str_data_base));
	p=(u32 *)&str_data_base;
	p++;
	for(i=0;i<(sizeof(str_data_base)/4);i++)
	{
		*p+=addr;
		p++;
	}

	Flash_Read(gameaddr, (u8 *)&str_game_base, sizeof(str_game_base));
	p=(u32 *)&str_game_base;
	p++;
	for(i=0;i<(sizeof(str_game_base)/4);i++)
	{
		*p+=gameaddr;
		p++;
	}
}

void File_Read(u32 addr, u8 *buf, u32 size)
{
	Flash_Read(addr, buf, size);
}

void File_GetGraphInfo(u16 GraphID, struct Graph_Info *graph_struct)
{
	u32 segoff,width,height,biBitCount,bisize;
	u32 dataAddr=GetResourceItemAddr(Resource_graph,GraphID);              //得到bmp图片的地址
	segoff=width=height=biBitCount=bisize=dataAddr;
	segoff+=0x0a;
	GetResourceData((u8 *)&segoff,segoff,4);
	
	bisize+=14;
	GetResourceData((u8 *)&bisize,bisize,4);
	width+=0x12;
	GetResourceData((u8 *)&width,width,4);
	
	height+=0x16;
	GetResourceData((u8 *)&height,height,4);
	
	graph_struct->addr = dataAddr+segoff;
	biBitCount+=28;
	GetResourceData((u8 *)&biBitCount,biBitCount,2);
	
	graph_struct->paletteaddr = dataAddr+bisize+14;         //调色板的位置
	graph_struct->addr = dataAddr+segoff;   	              //位图数据的位置
	graph_struct->bisize = bisize;
	graph_struct->segoff = segoff;
	graph_struct->width = (u8)width;
	graph_struct->height = (u8)height; 
	graph_struct->biBitCount = (u16)biBitCount;
	if(graph_struct->segoff == 14 + bisize)
		graph_struct->is_palette = 0;
	if(graph_struct->segoff > 14 + bisize)
		graph_struct->is_palette = 1;
}

u32 GetResourceAddr(enum ResourceID ObjID,int subid)
{
	u32 baseaddr;
	u32 *pbaseaddr;
	int itemsum;
	
	u32 cacheaddr,confaddr;
	
	if(ObjID<1)
		return 0;
	itemsum=sizeof(str_data_base)>>2;
	if(0!=(subid&0x8000))
	{
		cacheaddr=(u32)&str_data_base;
		confaddr=FLASH_SYSTEM_ADDR+ObjID*4;
	}else
	{
		cacheaddr=(u32)&str_game_base;
		confaddr=FLASH_GAME_ADDR+ObjID*4;
	}
	if(ObjID<=itemsum)
	{	
		pbaseaddr=((u32 *)cacheaddr);
		pbaseaddr+=ObjID;
		baseaddr=*pbaseaddr;
	}else
	{
		pbaseaddr=(u32 *)confaddr;
		Flash_Read((u32)pbaseaddr, (u8 *)baseaddr, sizeof(u32));
		baseaddr+=FLASH_SYSTEM_ADDR;
	}	
	return baseaddr;
}

u32 GetResourceItemAddr(enum ResourceID ObjID,int subID)
{
	u32 baseaddr;
	u32 segoff;
	baseaddr=GetResourceAddr(ObjID,subID);
	subID&=0x7fff;
	Flash_Read(baseaddr + 4 + subID * 4, (u8 *)&segoff, 4);
	baseaddr += segoff;	
	return baseaddr;
}

int GetResourceItemNum(enum ResourceID ObjID,int subid)
{
	u32 baseaddr;
	int itemsum;
	baseaddr=GetResourceAddr(ObjID,subid);
	Flash_Read(baseaddr, (u8 *)&itemsum, 4);
	return itemsum;
}

u16 File_GetAudioSum(int subid)
{
	return GetResourceItemNum(Resource_audio,subid);
	
//	u32 baseaddr;
//	int itemsum;
//	baseaddr=GetResourceAddr(Resource_audio,subid);
//	Flash_Read(baseaddr, (u8 *)&itemsum, 4);
//	return (u16)itemsum;
}

u16 File_GetGraphSum(int subid)
{
	return GetResourceItemNum(Resource_graph,subid);
//	u16 sum;
//	Flash_Read(graph_base_addr, (u8 *)&sum, 2);
//	return sum;
}

int GetResourceData(u8 *pbuf,int addr,int length)
{
	File_Read(addr, pbuf, length);
	//should adjust return length
	return length;
}

int GetResourceDataP(u8 *pbuf,int *addr,int _length)
{
	int length;
	u32 add=*addr;
	length=GetResourceData(pbuf,add,_length);
	*addr=add+length;
	return length;
}



