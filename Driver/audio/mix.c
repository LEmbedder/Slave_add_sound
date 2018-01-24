/*
mix
*/
#include "mix.h"
#include "chanel.h"

#define V_MAX 32767
#define V_MIN -32768
static double f=1.0;
#define C_PCM_S (0x00)

void mixchanel(PCM_DATA* mixbuf,PCM_DATA* chanel1,int length1,PCM_DATA* chanel2,int length2)
{
	int min,max,i;
	int mix,tmp;
	PCM_DATA* chanel;
	double dtmp;
	PCM_DATA data;
	PCM_DATA v1,v2,value;

	if(length1>=length2)
	{
		max=length1;
		min=length2;
	}else
	{
		min=length1;
		max=length2;
	}
	
	for(i=0;i<min;i++)
	{
		v1=*chanel1++;
		v2=*chanel2++;
#if 1		
		if((v1<0)&&(v2<0))
			value=v1+v2-v1*v2/-32767;
		else
			value=v1+v2-v1*v2/32768;
		*mixbuf++=value;
#else		
		tmp=v1+v2;
		dtmp=tmp*f;
		mix=(int)dtmp;
		if(mix>V_MAX)
		{
			f=(double)V_MAX/(double)(mix);
			mix=V_MAX;
		}else if(mix<V_MIN)
		{
			f=(double)V_MIN/(double)(mix);
			mix=V_MIN;
		}
		if(f<1)
			f+=((double)1-f)/(double)32;
		if(f>1)
			f=1.0;
		*mixbuf++=mix;
#endif		
	}
	
	if(length1>=length2)
		chanel=chanel1;
	else
		chanel=chanel2;
	
	for(;i<max;i++)
	{
		*mixbuf++=*chanel++;
	}
	
	#ifdef REALDATAFROM
	for(;i<C_pcm_bufsize*2;i++)
	{
		*mixbuf++=C_PCM_S;	//20160823	C_SILENT;
	}
	#else
	for(;i<C_pcm_bufsize/2;i++)
	{
		*mixbuf++=C_PCM_S;
	}
	#endif
	
}

//void mix(PCM_DATA* mixbuf,PCM_DATA* chanel1,int length1,PCM_DATA* chanel2,int length2)
//{
//	int min,max,i;
//	int mix,tmp;
//	PCM_DATA* chanel;
//	double dtmp;
//	PCM_DATA data;
//	PCM_DATA v1,v2;

//	if(length1>=length2)
//	{
//		max=length1;
//		min=length2;
//	}else
//	{
//		min=length1;
//		max=length2;
//	}
//	
//	for(i=0;i<min;i++)
//	{
//		v1=*chanel1++;
//		v1-=0x80;
//		v2=*chanel2++;
//		v2-=0x80;
//		tmp=v1+v2;
//		dtmp=tmp*f;
//		mix=(int)dtmp;
//		if(mix>V_MAX)
//		{
//			f=(double)V_MAX/(double)(mix);
//			mix=V_MAX;
//		}else if(mix<V_MIN)
//		{
//			f=(double)V_MIN/(double)(mix);
//			mix=V_MIN;
//		}
//		if(f<1)
//			f+=((double)1-f)/(double)16;
//		if(f>1)
//			f=1.0;
//		data=mix;
//		data+=0x80;
//		*mixbuf++=data;
//	}
//	
//	if(length1>=length2)
//		chanel=chanel1;
//	else
//		chanel=chanel2;
//	
//	for(;i<max;i++)
//	{
//		*mixbuf++=*chanel++;
//	}
//	
//	for(;i<C_pcm_bufsize*2;i++)
//	{
//		*mixbuf++=C_SILENT;
//	}
//	
//}


