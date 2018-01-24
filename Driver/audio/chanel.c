/*
chanel
*/
#include "chanel.h"
#include "File.h"
#include "pcm.h"
#include "CMLIB.h"
#include "decode.h"

#ifdef REALDATAFROM
int chanel_fillbuf(pchanelData chanel) 
{
	PCM_CODE_DATA tmpbuf[C_pcm_bufsize];
	u8 *pbuf=(u8 *)tmpbuf;
	int rlength=0;
	int sampleNum;
	
	if(0==(chanel->flags&B_chanel_IsPlaying))
		return 0;
	
	if(chanel->filelength>C_pcm_bufsize)
	{
		rlength=GetResourceData(pbuf,chanel->dataAddr,C_pcm_bufsize);
		chanel->dataAddr+=rlength;
		chanel->filelength-=rlength;
		decode((PCM_CODE_DATA *)pbuf,rlength, chanel->buffer,chanel);
	}else
	{
		rlength=GetResourceData(pbuf,chanel->dataAddr,chanel->filelength);
		decode((PCM_CODE_DATA *)pbuf,rlength, chanel->buffer,chanel);
		pbuf+=rlength;
		chanel->filelength=0;
	
		if(B_chanel_loop==(chanel->flags&B_chanel_loop))
		{
			chanel->state.index=0;
			chanel->state.valprev=chanel->backstate.valprev;
			while(rlength<C_pcm_bufsize)
			{
				chanel->dataAddr=chanel->backAddr;
				chanel->filelength =chanel->backlength;
				int bufsize=C_pcm_bufsize-rlength<chanel->filelength?C_pcm_bufsize-rlength:chanel->filelength;
				bufsize=GetResourceData(pbuf,chanel->dataAddr,bufsize);
				decode((PCM_CODE_DATA *)pbuf,bufsize, chanel->buffer,chanel);
				rlength+=bufsize;
				pbuf+=bufsize;
				chanel->filelength-=bufsize;
				chanel->dataAddr+=bufsize;
			}
		}else
		{
			chanel->flags ^= B_chanel_IsPlaying;
		}		
	}
	
	sampleNum=rlength*2;
	
	return sampleNum;
}
#else
int GettestData(u8 *pbuf,int addr,int length)
{
	ccopy((u8 *)addr,pbuf,length);
	return length;
}

int chanel_fillbuf(pchanelData chanel) 
{
	u8 *pbuf=(u8 *)chanel->buffer;
	int rlength=0;
	int sampleNum;
	
	if(0==(chanel->flags&B_chanel_IsPlaying))
		return 0;
	
	if(chanel->filelength>C_pcm_bufsize)
	{
		rlength=GettestData(pbuf,chanel->dataAddr,C_pcm_bufsize);
		chanel->dataAddr+=rlength;
		chanel->filelength-=rlength;
	}else
	{
		rlength=GettestData(pbuf,chanel->dataAddr,chanel->filelength);
		pbuf+=rlength;
		chanel->filelength=0;
	
		if(B_chanel_loop==(chanel->flags&B_chanel_loop))
		{
			while(rlength<C_pcm_bufsize)
			{
				chanel->dataAddr=chanel->backAddr;
				chanel->filelength =chanel->backlength;
				int bufsize=C_pcm_bufsize-rlength<chanel->filelength?C_pcm_bufsize-rlength:chanel->filelength;
				rlength+=GettestData(pbuf,chanel->dataAddr,bufsize);
				pbuf+=bufsize;
				chanel->filelength-=bufsize;
				chanel->dataAddr+=bufsize;
			}
		}else
		{
			chanel->flags ^= B_chanel_IsPlaying;
		}		
	}
	sampleNum=rlength/2;
	return sampleNum;
}
#endif
