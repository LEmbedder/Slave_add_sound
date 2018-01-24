#include "GD62429.h"
#include "misc.h"
#include "CMOS.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "USART.h"


//这里的命名中的GD是指声音调节芯片是GD62492,跟GDmcu没关系
#define GD_SDA_HIGH()    GPIOB->BSRR = GPIO_Pin_5
#define GD_SDA_LOW()     GPIOB->BRR = GPIO_Pin_5
#define GD_SCL_HIGH()    GPIOB->BSRR = GPIO_Pin_4
#define GD_SCL_LOW()     GPIOB->BRR = GPIO_Pin_4
#define GD_SDA_READ()    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)

static void i2c_Delay(char tick);


enum GD_I2C_ACK {
	GD_i2c_ak = 0,
	GD_i2c_nak
};


void GD62429_GPIO_Init(void)                                     //I2C引脚初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;     // SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;     // SCL
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GD_SCL_LOW(); 
	//GD_I2C_W(_40dB);
	
}

u8 GD_I2C_W(u16 val)                            // 写二两个字节
{
	u8 i;
	u8 ack = 0;
	
	GD_SCL_LOW();                                 //时钟拉低
	
	for (i = 0; i < 11; i++) {  	
		if (val & 0x400)                            //取它的高位
			GD_SDA_HIGH();
		else
			GD_SDA_LOW();                          //数据位的操作，此时数据位保持不变
		i2c_Delay(50);       

		i2c_Delay(50);
		GD_SCL_HIGH();
		
		i2c_Delay(50);                          //此时是上升沿
		
		GD_SDA_LOW(); 
		
		i2c_Delay(50);   
		
		if(i == 10)
			GD_SDA_HIGH();
		
		GD_SCL_LOW();  
		val = val << 1;
	}
	
	return ack;
}

u8 gd_I2C_W()
{

  int i;
	for(i = 0;i < 16;i++){
    GD_SDA_HIGH();
		i2c_Delay(10);
	  GD_SDA_LOW(); 
		i2c_Delay(10);
		
	}
		return 0;
}

static void i2c_Delay(char tick)
{
	u16 i;
	
	i = tick;                 //10
	while(i--);
}






