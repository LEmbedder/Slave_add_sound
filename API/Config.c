#include "Config.h"
#include "CMOS.h"
#include "Flash.h"
#include "FLASHROM.h"
#include "nRF.h"
#include "display.h"
#include "NRF24L01.h"
#include "stm32f10x_iwdg.h"
#include "GD62429.h"
#include "USART.h"

static u32 configchecksum(PCONFIG conf)
{
	int i,cnt;
	u32 *p;
	u32 checksum=0;
	p=(u32 *)conf;
	cnt=sizeof(CONFIG)/4 -3;
	for(i=0;i<cnt;i++)
	{
		checksum+=*p;
		p++;
	}
	return checksum;
}

u32 writeconfig(PCONFIG conf)
{
	conf->aaaa=0xaaaa;
	conf->x5555=0x5555;
	conf->checksum=configchecksum(conf);
	Flash_4K_Erase(FLASH_CONFIG_ADDR);
	Flash_Write_Page(FLASH_CONFIG_ADDR,(u8 *)conf,sizeof(CONFIG));
	return 0;
}

static CONFIG conf;


PCONFIG getConfig()
{
	return &conf;
}

void InitConfig()
{
	int i,cnt;
	u32 *p;
	u32 checksum=0;
	int isok=1;

	Flash_Read(FLASH_CONFIG_ADDR,(u8 *)&conf,sizeof(CONFIG));
	if((0xaaaa!=conf.aaaa)||(0x5555!=conf.x5555))
	{
		isok=0;
	}else
	{
		checksum=configchecksum(&conf);
		if(checksum!=conf.checksum)
		{
			isok=0;
		}
	}
	if(!isok)
	{
		cnt=sizeof(CONFIG)/4;
		p=(u32 *)&conf;
		for(i=0;i<cnt;i++)
		{
			*p=0xffffffff;
			p++;
		}		
		writeconfig(&conf);
	}
	
	//for test
	
//	while(1)
//	{
//		u8 setn[16]="hell wors";
//		u8 getn[16];
//		setgname(setn);
//		getgname(getn);
//		IWDG_ReloadCounter();
//	}
	//test end
	
}

int nRF_updateAddr()
{
	int ret;	
	u8 addr[5]={0x39,0xff,0xd9,0x05,0x48};
	u32 conf_2_4GaddrL=conf.addr2_4GL;
	u32  conf_2_4GaddrH=conf.addr2_4GH;
	ret=0;
	if(0xffffffff==conf_2_4GaddrL && 0xffffffff==conf_2_4GaddrH)
	{	
		conf_2_4GaddrL=*(vu32*)(0x1ffff7e8);
		conf_2_4GaddrH=*(vu32*)(0x1ffff7ec);
		conf_2_4GaddrH&=0x00ff;
		if((0==conf_2_4GaddrL)&&(0==conf_2_4GaddrH))
		{
			conf_2_4GaddrL=0x12345678;
			conf_2_4GaddrH=0x00aa;
		}

		conf.addr2_4GL=conf_2_4GaddrL;
		conf.addr2_4GH=conf_2_4GaddrH;
		//20161101	writeconfig(&conf);
		ret=1;
	}	
	nRF_getAddr(addr);
	nRF_SetAddress(addr);
	nRF_TxSetTXRP0(addr);
	return ret;
}

u8 nRF_getAddr(u8 *_addr)
{
	u8 *p;
	u32 conf_2_4GaddrL=conf.addr2_4GL;
	u32  conf_2_4GaddrH=conf.addr2_4GH;
	if(0xffffffff==conf_2_4GaddrL && 0xffffffff==conf_2_4GaddrH)
		return 1;
	p=(u8 *)& conf.addr2_4GL;
	*_addr++=*p++;
	*_addr++=*p++;
	*_addr++=*p++;
	*_addr++=*p++;
	p=(u8 *)& conf.addr2_4GH;
	*_addr=*p;
	return 0;
}

u8 nRF_setAddr(u8 *_addr)
{
	u8 addr[5];
	u32 addl,addh;
	u8 *p=(u8 *)&addl;
	*p++=*(_addr+0);
	*p++=*(_addr+1);
	*p++=*(_addr+2);
	*p=*(_addr+3);
	
	p=(u8 *)&addh;
	*p=*(_addr+4);
	addh&=0x00ff;
	
	conf.addr2_4GL=addl;
	conf.addr2_4GH=addh;
	writeconfig(&conf);
	
	nRF_getAddr(addr);
	nRF_SetAddress(addr);
	nRF_TxSetTXRP0(addr);
	return 0;
}

u32 readSlaverID()
{
	return conf.ID;
}

void WriteSlaverID(u32 _id)
{
	conf.ID=_id;
	writeconfig(&conf);
}

static void witeproisok(u32 _isok)
{
	conf.proIsok=_isok;
	writeconfig(&conf);
}

void Resetproisok()
{
	witeproisok(0xffffffff);
}

void setgname(u8 *name)
{
	int i;
	u8 buf[256];
	u8 *p=buf;
	u8 page,index;
	page=0x00;
	index=0x00;
	Flash_Read(FLASH_GNAME_ADDR, buf, 256);
	for(i=0;i<120;i++)
	{
		if((*p==0xff)&&(*(p+1)==0xff))
		{
			page=i/8;
			index=i%8;
			break;
		}
		p+=2;
	}
	if((index==0x00)&&(page==0x00))
	{
		Flash_4K_Erase_Start(FLASH_GNAME_ADDR);
	}
	buf[0]=0x55;
	buf[1]=0xaa;
	Flash_Write(FLASH_GNAME_ADDR+(page*8+index)*2, buf, 2);
	Flash_Write(FLASH_GNAME_ADDR+(page+1)*256+index*32, name, 16);
}

void getgname(u8 *name)
{
	int i;
	u8 buf[256];
	u8 *p=&buf[239];
	u8 page,index;
	page=0xff;
	index=0xff;
	Flash_Read(FLASH_GNAME_ADDR, buf, 256);
	for(i=119;i>=0;i--)
	{
		if((*(p-1)==0x55)&&(*p==0xaa))
		{
			page=i/8;
			index=i%8;
			break;
		}
		p-=2;
	}
	if((index!=0xff)&&(page!=0xff))
	{
		Flash_Read(FLASH_GNAME_ADDR+(page+1)*256+index*32, name, 16);
	}else{
		*name=0x00;
	}
}

#include "GEngineCfg.h"
	
int GEA_SetGName(PGName in)
{
	setgname(in->gname);
	return 0;
}

int GEA_GetGName(PCom_Arg in,PGName out)
{
	getgname(out->gname);
	return 0;
}


int GEA_Setvolume(Pvolume_Arg in)
{
   GD_I2C_W(in->data);
}


