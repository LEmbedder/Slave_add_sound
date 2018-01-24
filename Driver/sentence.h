#ifndef SENTENCE_H_
#define SENTENCE_H_
#include "stm32f10x.h"
#include "GEngineCfg.h"


typedef enum {
  	SND_STATUS_STOP = 0,
  	SND_STATUS_PLAY,
  	SND_STATUS_PAUSE,
  	SND_STATUS_PauseNextVoice,
   }SND_STATUS;

#define C_stvoiceidBufSize	8u
#define C_stVarBufSize	8u
#define C_stDepth	3u
#define C_stListBufSize			C_stVarBufSize
#define C_stEndFlag	0xffff

#define C_stCtrCheck	0x01000000
#define C_stCtrPlay		0x02000000
#define C_stCtrStop		0x03000000

#define C_stCtrMask		0x0f000000


void sndInit(void);
void stCheck(void);
int stPlaySentence(u16 phraseIndex);
void stStopSentence(void);
u16 stGetSentenceStatus(void);
void stSetVarVoice(u16 *p,u16 length);
void SentenceISR(void);
static void sentenceListPlay(u16 index);
static void sentencePlay(u16 index);
//void GEA_SetSpeechList(PSpeechList_Arg in);
//void GEA_PlaySpeech(PSpeech_Arg in);
//void GEA_StopSpeech(PCom_Arg in);
//void GEA_GetSpeechStatus(PCom_Arg in,PCom_Res out);
//static void sentenceStop(void);

#endif /*SENTENCE_H_*/
