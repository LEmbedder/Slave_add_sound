#ifndef _Resource_H
#define _Resource_H

#include "stm32f10x.h"

enum ResourceID
{
	Resource_graph=1,
	Resource_audio,
	Resource_phrase,
	Resource_anim,
	Resource_reserved01,
	Resource_reserved02,
};

typedef struct data_base_tag
{
	s32 itemsnum; 
	u32 graph_base_addr;
	u32 audio_base_addr;
	u32 phrase_base_addr;
	u32 anim_base_addr;
	u32 reserved01;
	u32 reserved02;
}str_Resource,pstr_Resource;

#endif
