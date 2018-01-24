/*
decode
*/
#include "decode.h"
#include "chanel.h"
#include "pcm.h"

int decode(PCM_CODE_DATA* cdata,int num, PCM_DATA* pcm,pchanelData chanel)
{
	adpcm_decoder((char *)cdata, (s16 *)pcm, num, &(chanel->state));
	return 0;
}
