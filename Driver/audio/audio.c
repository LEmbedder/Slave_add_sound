/*
audio
*/
#include "audio.h"
#include "chanel.h"
#include "pcm.h"
#include "mix.h"
#include "File.h"

static mix_data mixer;
static chanelData voice={.buffer={1,2}};
static chanelData bgm={.buffer={1,2}};

void audio_Init()
{
	voice.flags=0;
	bgm.flags=0;
	pcm_Init();
}

void audio_checkmsg()
{
	int vlength,blength;
	if((voice.flags&B_chanel_IsPlaying)||(bgm.flags&B_chanel_IsPlaying))
	{
		while(pcm_checkbuf())
		{
			PCM_DATA* p=mixer.mix_buf[mixer.index];
			#ifdef REALDATAFROM
			for(int i=0;i<2;i++)
			{
				vlength=chanel_fillbuf(&voice);
				blength=chanel_fillbuf(&bgm);
				mixchanel(p,voice.buffer,vlength,bgm.buffer,blength);
				p+=C_pcm_bufsize*2;
			}
			#else
			for(int i=0;i<8;i++)
			{
				vlength=chanel_fillbuf(&voice);
				blength=chanel_fillbuf(&bgm);
				mixchanel(p,voice.buffer,vlength,bgm.buffer,blength);
				p+=C_pcm_bufsize/2;
			}
			#endif
			pcm_fillbuf(mixer.mix_buf[mixer.index]);
			mixer.index^=0x01;
		}
	}
}

extern void _binary_asc5x7_bin_start(void);

void PlayVoice(u16 index)
{
	#ifdef REALDATAFROM
		voice.dataAddr=GetResourceItemAddr(Resource_audio,index);
		GetResourceData((u8 *)&voice.filelength,voice.dataAddr,4);
		voice.dataAddr+=0x04;	
		GetResourceData((u8 *)&voice.state.valprev,voice.dataAddr,2);
		voice.dataAddr+=0x02;
		voice.state.index=0;
	#else
		voice.dataAddr=(u32)&_binary_asc5x7_bin_start;	
		voice.dataAddr-=1;
		voice.filelength=0x7d00;
	#endif
	
	voice.backAddr=voice.dataAddr;
	voice.backlength=voice.filelength;
	voice.backstate.valprev=voice.state.valprev;
	voice.flags=(B_chanel_IsPlaying);
	audio_checkmsg();
	pcm_open();
}

void _PlayBGM(u16 index)
{
	bgm.dataAddr=GetResourceItemAddr(Resource_audio,index);
//	bgm.dataAddr+=0x28;
	GetResourceData((u8 *)&bgm.filelength,bgm.dataAddr,4);
	bgm.dataAddr+=0x04;
	
	GetResourceData((u8 *)&bgm.state.valprev,bgm.dataAddr,2);
	bgm.dataAddr+=0x02;
	bgm.state.index=0;
	
	bgm.backAddr=bgm.dataAddr;
	bgm.backlength=bgm.filelength;
	bgm.flags=(B_chanel_IsPlaying);
	bgm.flags|=B_chanel_loop;
	audio_checkmsg();
	pcm_open();
}

int playbgm(u16 phraseIndex)
{
	u32 segoff;
	u32 total;
	PLHead headinfor;
	u32 baseaddr;
	u8 num;
	u16 midiid;

	baseaddr=GetResourceAddr(Resource_phrase,phraseIndex);
	GetResourceData((u8 *)&total,baseaddr,  4);
	phraseIndex&=0x7fff;
	if(phraseIndex>=total)
	{
		return -1;
	}

	segoff= phraseIndex*sizeof(PLHead);
	segoff+=4;
	
	GetResourceData((u8 *)&headinfor,baseaddr+segoff,sizeof(PLHead));	

	GetResourceData((u8 *)&midiid,baseaddr+headinfor.segoff+1,sizeof(u16));
	_PlayBGM(midiid);
	return 0;
}

void StopVoice()
{
	voice.flags=0x0000;
}

void StopBGM(void)
{
	bgm.flags=0x0000;
}

u16 IsVoiceBusy()
{
	return voice.flags&B_chanel_IsPlaying;
}
