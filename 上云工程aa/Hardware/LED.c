#include "stm32f10x.h"                  // Device header

void LED_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC ,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz ;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}

void LED_ON()
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
}
void LED_OFF()
{
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
}
void LED_Turn()
{
	if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_5)== 0)
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
	else
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
}
