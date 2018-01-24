#include "sentence.h"
#include "File.h"
#include "audio.h"

typedef struct
{
	u32 segoff;
	u16 num;	
}SENTENCE;

#define	C_stEnd		(1<<0)				//0--not end,1--end	
#define	C_stPause	(1<<1)				//0--not pause,1--pause	
#define	C_stPauseNextVoice	(1<<2)		//0--not pause,1--pause	
#define	C_stLoopVoice	(1<<3)			//0--not loop,1--loop	

#define C_stStatus (C_stEnd+C_stPause+C_stPauseNextVoice)

// 
#define C_stInvalid 0xffff

static u16 st_voiceidBuf[C_stvoiceidBufSize];
static u8 st_voiceIndex;
static u16 st_varBuf[C_stVarBufSize];
static u8 st_varIndex;
static u16 st_listBuf[C_stListBufSize];
static u8 st_listIndex;
static SENTENCE st_sentenceBuf[C_stDepth];
static u8 st_SentenceIndex;               
static u16 st_stStatus=C_stEnd;

static s16 stFillBuf(void);
static s16 stOpenSentence(u16 phraseIndex);

#if	_DEBUG_VERSION_
static void XstTrace();
#define stTrace XstTrace();
#else
#define stTrace (void)0;
#endif
//void GEA_StopSpeech(PCom_Arg in);
void sndInit(void)
{
	u32 i;
	audio_Init();
	for(i=0;i<C_stvoiceidBufSize;i++)
	{
		st_voiceidBuf[i]=C_stInvalid;
	}
	st_voiceIndex=C_stvoiceidBufSize;
	for(i=0;i<C_stVarBufSize;i++)
	{
		st_varBuf[i]=C_stInvalid;
	}
	st_varIndex=C_stVarBufSize;
	for(i=0;i<C_stListBufSize;i++)
	{
		st_listBuf[i]=C_stEndFlag;
	}
	st_listIndex=C_stListBufSize;
	for(i=0;i<C_stDepth;i++)
	{
		st_sentenceBuf[i].num=0;
	}
	st_SentenceIndex=C_stDepth;
	
	st_stStatus=C_stEnd;
}

void stCheck()
{
	u16 Index;
	audio_checkmsg();	
	if((st_stStatus&C_stStatus)||IsVoiceBusy())
		return;
  if(st_voiceIndex>=C_stvoiceidBufSize)
	{
		stFillBuf();
	}
	if(st_sentenceBuf[st_SentenceIndex].num<=0)
	{
		if(0<st_SentenceIndex)
		{
			st_SentenceIndex--;
			st_voiceIndex=C_stvoiceidBufSize;
			stCheck();
			return;
		}
		else if((C_stEndFlag==st_listBuf[st_listIndex])||(st_listIndex>=C_stListBufSize))
		{
			st_stStatus|=(C_stEnd);
			return;
		}else{
			sentencePlay(st_listBuf[st_listIndex++]);
			return;
		}
	}
	
	Index=st_voiceidBuf[st_voiceIndex];

	if(Index<=0xfff0)
	{
			PlayVoice(Index);
			st_sentenceBuf[st_SentenceIndex].segoff+=sizeof(u16);
			st_sentenceBuf[st_SentenceIndex].num-=1;
			st_voiceIndex++;
	}else	//0xfff1~0xfffe
	{
		if(st_varIndex>=C_stVarBufSize||C_stInvalid==st_varBuf[st_varIndex]||st_SentenceIndex>=(C_stDepth-1))
		{
			stTrace;
			st_voiceIndex++;
			return;
		}
		else
		{	
			st_SentenceIndex++;	
			stOpenSentence(st_varBuf[st_varIndex]);
			st_voiceIndex = C_stvoiceidBufSize;
			st_varBuf[st_varIndex++]=C_stInvalid;
			stCheck();
			return;
		}
	}
}

int GEA_SetSpeechList(PSpeechList_Arg in)	//(u16 *pList)
{
	u32 i=0;
	u16 *p=in->voiceID;
	u16 pl;
	PCom_Arg parg=0;
	GEA_StopSpeech(parg);//sentenceStop();
	for(i=0;i<C_stVarBufSize;i++)
	{
		pl=*p++;
		st_varBuf[i]=pl;
		if(C_stEndFlag==pl)
			break;
	}
	p=in->voiceID;	//pList;
	for(i=0;i<C_stListBufSize;i++)
	{
		pl=*p++;
		st_listBuf[i]=pl;
		if(C_stEndFlag==pl)
			break;
	}
	return 0;
}

int GEA_LoopSpeech(PSpeech_Arg in)
{
	if(0xffff==in->voiceID)
		StopBGM();
	else
		playbgm(in->voiceID);
	return 0;
}

int GEA_PlaySpeech(PSpeech_Arg in)	//sndPlaySpeech(u16 index)
//static void sentenceListPlay(u16 index)
{
	PCom_Arg parg=0;
	GEA_StopSpeech(parg);
	st_stStatus=st_stStatus&~C_stStatus;
	
	if(C_stEndFlag==in->voiceID)// index)
	{
		
	}
	else
	{
		st_listBuf[0]=in->voiceID;// index;
		st_listBuf[1]=C_stEndFlag;
	}
	st_listIndex=0;
	sentencePlay(st_listBuf[st_listIndex++]);
	return 0;
}

static void sentencePlay(u16 index)
{
	stOpenSentence(index);
	st_voiceIndex = C_stvoiceidBufSize;
	st_SentenceIndex=0;
//	st_stStatus=st_stStatus&~C_stStatus;
	stCheck();
}

int GEA_StopSpeech(PCom_Arg in)	//sndStopSpeech()
//static void sentenceStop()
{
	st_stStatus=C_stEnd;
	st_voiceIndex=C_stvoiceidBufSize;
	st_varIndex=0;
	st_listIndex=0;
	st_SentenceIndex=0;
	StopVoice();
	return 0;
}

int GEA_GetSpeechStatus(PCom_Arg in,PCom_Res out)	//sndGetSpeechStatus(void)
{
	u8 ret;
	if(st_stStatus&C_stEnd)
	{
		ret = C_SpeechEnd;
	}else if(st_stStatus&C_stPause)
	{
		ret = C_SpeechPause;
	}else if(st_stStatus&C_stPauseNextVoice)
	{
		ret = C_SpeechPauseNextVoice;
	}else
	{
		ret = 0;
	}
	out->result=ret;
	return 0;
}

static u16 playplindex;
static s16 stOpenSentence(u16 phraseIndex)
{
	u32 segoff;
	u32 total;
	PLHead headinfor;
	u32 baseaddr;
	u8 num;
	
	playplindex=phraseIndex;

	{	
	baseaddr=GetResourceAddr(Resource_phrase,phraseIndex);
	GetResourceData((u8 *)&total,baseaddr,  4);
	phraseIndex&=0x7fff;
	if(phraseIndex>=total)
	{
		stTrace;
		return -1;
	}

	segoff= phraseIndex*sizeof(PLHead);
	segoff+=4;
	
	GetResourceData((u8 *)&headinfor,baseaddr+segoff,sizeof(PLHead));	
	GetResourceData((u8 *)&num,baseaddr+headinfor.segoff,1);
	st_sentenceBuf[st_SentenceIndex].num=num;
	st_sentenceBuf[st_SentenceIndex].segoff=headinfor.segoff+1;

	}
	return 0;
}

static s16 stFillBuf() 
{
	u32 segoff;
	u32 baseaddr;
	int k;
	st_voiceIndex=0;
	{	
	baseaddr=GetResourceAddr(Resource_phrase,playplindex);

	segoff = st_sentenceBuf[st_SentenceIndex].segoff;	
	k = C_stvoiceidBufSize<=st_sentenceBuf[st_SentenceIndex].num ? C_stvoiceidBufSize:st_sentenceBuf[st_SentenceIndex].num;
	GetResourceData((u8 *)&st_voiceidBuf,baseaddr+segoff,k*sizeof(u16));
		
//		st_sentenceBuf[st_SentenceIndex].num=1;
//		st_voiceidBuf[0]=00;

	}
	return 0;
}



#if	_DEBUG_VERSION_
static void XstTrace()
{

}
#endif



