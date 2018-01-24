#ifndef __FLASHROM_H
#define __FLASHROM_H	 

#ifdef __cplusplus
extern "C" {
#endif

enum FLASHID
{
	FLASHID_CONFIG=0,
	FLASHID_SYSTEM,
	FLASHID_GAME,
};


#define FLASH_CONFIG_ADDR		0x000000	//配置文件
#define FLASH_GNAME_ADDR		0x1000
#define FLASH_CONFIG_SIZE		0x10000		//64k

#define FLASH_SYSTEM_ADDR	 	(FLASH_CONFIG_ADDR+FLASH_CONFIG_SIZE)		//内置游戏数据             0x000000 + 0x10000
#define FLASH_SYSTEM_SIZE    0x800000       // 8 M 

#define FLASH_GAME_ADDR 	 (FLASH_SYSTEM_ADDR+FLASH_SYSTEM_SIZE)       // 
#define FLASH_GAME_SIZE      0x800000       // 8 M 

#ifdef __cplusplus
}
#endif


#endif


