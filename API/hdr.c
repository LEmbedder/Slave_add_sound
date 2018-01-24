#include "hdr.h"
#include "GEngineCfg.h"
#include "source.h"
#include "File.h"
#include "CMOS.h"
#include "display.h"
#include "CMLIB.h"

#define C_HDREND 0x01
#define C_HDRPAUSE 0x02

#define C_PLAYERNUM	4
#define C_FIFONUM	4

#define C_DEFAULTSPEED	240

typedef struct hdrtag
{
	u32 delay;
	u32 addr;
	u16 id;
	s16 x;
	s16 y;
	u8 layer;
	u32 tcnt;
	u8 flag;	
}HDR,*PHDR;

typedef struct hdrplayertag
{
	HDR hdrFIFO[C_FIFONUM];
	PHDR play;
}HDRPLAY,*PHDRPLAY;

static HDRPLAY players[C_PLAYERNUM];

static int layer2index(u8 layer)
{
	if(C_HDRLayer_1==layer)
	{
		return 0;
	}
	else if(C_HDRLayer_2==layer)
	{
		return 1;
	}
	else if(C_HDRLayer_3==layer)
	{
		return 2;
	}
	else if(C_HDRLayer_4==layer)
	{
		return 3;
	}
	return C_HDRLayer_Max;
}

int GEA_AddAnimation(PAnimation_Arg args)
{
	int index,i;
	u8 layer=args->layer;
	index=layer2index(layer);
	if(index>=C_HDRLayer_Max)
		return 0;
	for(i=0;i<C_HDRLayer_Max;i++)
	{
		if(players[index].hdrFIFO[i].id==0xffff)
		{
			players[index].hdrFIFO[i].id=args->animationID;
			players[index].hdrFIFO[i].layer=args->layer;
			players[index].hdrFIFO[i].x=args->x;
			players[index].hdrFIFO[i].y=args->y;
			break;
		}
	}
	return 0;
}

int hdraction(u8 layer,u8 mode)
{
	int index;
	index=layer2index(layer);
	if(index>=C_HDRLayer_Max)
		return 1;
	if(0==mode)
	{
		ClearGraphLayer(index*2+1);
		players[index].play=0;
		players[index].hdrFIFO[0].id=0xffff;
	}else if(1==mode)
	{
		players[index].play=0;
		players[index].hdrFIFO[0].id=0xffff;
	}
	players[index].hdrFIFO[1].id=0xffff;
	players[index].hdrFIFO[2].id=0xffff;
	players[index].hdrFIFO[3].id=0xffff;
	return 0;
}


int GEA_ClearAnimationFifo(PHDRLayer_Arg args)
{
	if(0!=(args->layers&C_HDRLayer_1))
	{
		hdraction(C_HDRLayer_1,2);		
	}
	if(0!=(args->layers&C_HDRLayer_2))
	{
		hdraction(C_HDRLayer_2,2);	
	}
	if(0!=(args->layers&C_HDRLayer_3))
	{
		hdraction(C_HDRLayer_3,2);	
	}
	if(0!=(args->layers&C_HDRLayer_4))
	{
		hdraction(C_HDRLayer_4,2);	
	}
	return 0;
}

int GEA_DeleteAnimation(PHDRLayer_Arg args)
{
	if(0!=(args->layers&C_HDRLayer_1))
	{
		hdraction(C_HDRLayer_1,0);		
	}
	if(0!=(args->layers&C_HDRLayer_2))
	{
		hdraction(C_HDRLayer_2,0);	
	}
	if(0!=(args->layers&C_HDRLayer_3))
	{
		hdraction(C_HDRLayer_3,0);	
	}
	if(0!=(args->layers&C_HDRLayer_4))
	{
		hdraction(C_HDRLayer_4,0);	
	}
	return 0;
}

int GEA_StopAnimation(PHDRLayer_Arg args)
{
	if(0!=(args->layers&C_HDRLayer_1))
	{
		hdraction(C_HDRLayer_1,1);		
	}
	if(0!=(args->layers&C_HDRLayer_2))
	{
		hdraction(C_HDRLayer_2,1);	
	}
	if(0!=(args->layers&C_HDRLayer_3))
	{
		hdraction(C_HDRLayer_3,1);	
	}
	if(0!=(args->layers&C_HDRLayer_4))
	{
		hdraction(C_HDRLayer_4,1);	
	}
	return 0;
}

int GEA_PlayAnimation(PAnimation_Arg args)
{
	int ret;
	ret=hdraction(args->layer,0);
	if(ret)
		return 0;
	return GEA_AddAnimation(args);
}

void hdr_Init()
{
	HDRLayer_Arg arg;
	arg.layers=(C_HDRLayer_1 |C_HDRLayer_2 |C_HDRLayer_3 |C_HDRLayer_4);
	GEA_DeleteAnimation(&arg);
}

void hdr_check(PHDR myhdr)
{
	u8 label;
	u16 _exit=1;
	u32 tcnt;
	u32 data32;
	u16 data16;
	Speech_Arg speech;
	Com_Res ret;
//	u8 graphscnt;
	u8 layer;
	
	struct{
		u16 id;
		s8 x;
		s8 y;
	}graph;
	if(myhdr->flag&(C_HDREND | C_HDRPAUSE))
		return;
	tcnt=myhdr->tcnt-CMOS_GetTimer();
	if(tcnt<myhdr->delay)
		return;
	myhdr->tcnt=CMOS_GetTimer();
	while(_exit)
	{		
		GetResourceData((u8 *)&label,myhdr->addr,1);
		myhdr->addr+=1;
		switch(label)
		{
			case 0xff:
				myhdr->flag|=C_HDREND;
				_exit=0;
				break;
			case 0xfa:
				GetResourceData((u8 *)&data32,myhdr->addr,4);
				myhdr->addr+=4;
				myhdr->delay=data32;
				break;
			case 0xfe:
				GetResourceData((u8 *)&data16,myhdr->addr,2);
				myhdr->addr+=2;
				speech.voiceID=data16;
				GEA_PlaySpeech(&speech);
				break;
			case 0xf7:
				layer=layer2index(myhdr->layer);
				layer<<=1;
				layer+=1;
				ClearGraphLayer(layer);
				SetDisplayUpdate();
				break;
			case 0xf9:
				GEA_GetSpeechStatus(0,&ret);
				if(C_SpeechEnd==ret.result)
				{
					
				}else
				{
					myhdr->addr-=1;
					_exit=0;
				}
				break;
			default:
				if(label<=8 && label>=1)
				{
					layer=layer2index(myhdr->layer);
					layer<<=1;
					layer+=1;
					ClearGraphLayer(layer);
					for(;label>0;label--)
					{
						GetResourceDataP((u8 *)&graph,(int *)&myhdr->addr,sizeof(graph));
						
						AddGraph(graph.id, graph.x+myhdr->x, graph.y+myhdr->y, layer);
					}
					SetDisplayUpdate();
					_exit=0;
				}
				break;
		}
	}
	
}

void hdr_open(PHDR myhdr)
{
	myhdr->flag=0x00;
	myhdr->addr=GetResourceItemAddr(Resource_anim,myhdr->id);
	myhdr->delay=0;
	hdr_check(myhdr);
	if(0==myhdr->delay)
		myhdr->delay=C_DEFAULTSPEED;
}

void play_check()
{
	PHDRPLAY pp;
	int i,j;
	for(i=0;i<C_PLAYERNUM;i++)
	{
		pp=&players[i];
		if(0==pp->play )
		{
			if(0xffff!=pp->hdrFIFO[0].id)
			{
				hdr_open(pp->hdrFIFO);
				pp->play=pp->hdrFIFO;
				continue;
			}
		}else if((pp->play)->flag&C_HDREND)
		{
			for(j=1;j<C_FIFONUM;j++)
			{
				ccopy((u8 *)&pp->hdrFIFO[j],(u8 *)&pp->hdrFIFO[j-1],sizeof(HDR));
			}
			pp->hdrFIFO[C_FIFONUM-1].id=0xffff;
			if(0xffff!=pp->hdrFIFO[0].id)
			{
				hdr_open(pp->hdrFIFO);
				pp->play=pp->hdrFIFO;
				continue;
			}else
			{
				pp->play=0;
			}
		}
		if(0!=pp->play ){
			hdr_check(pp->play);
		}
	}
}

u8 play_status(void)
{
	PHDRPLAY pp;
	int i;
	for(i=0;i<C_PLAYERNUM;i++)
	{
		pp=&players[i];
		if(0==pp->play )
		{
			continue;
		}else if((pp->play)->flag&C_HDREND)
		{
			continue;
		}
		if(0!=pp->play ){
			return 1;
		}
	}
	return 0;
}


