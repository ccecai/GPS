#include "stm32f10x.h"                  // Device header

#include <stdio.h>
#include "Delay.h"
#include "MyUSART.H"
#include "esp.h"
#include "DHT11.H"
#include "MySPI.H"
#include "Timer.H"
#include "PWM.H"
#include "LED.H"
#include "usart3.h"
#include <stdio.h>
#include <string.h>
#include "Delay.h"
#include "oled.h"

int LightLux;
int soil_humidity;
int soil_temperature;
int Humidity;
int temperature;
int speed;
float fspeed;
int fangle=0;
int angle_H,angle_L;
int level;
int chonglian_flag = 0;
extern int bk;
extern char RECS[200];
extern char Data[5];
extern char rx_buffer[100];
extern uint16_t rx_index;
extern float jindu,weidu;
extern int start1_flag,three_flag,turn_flag;
uint16_t cnt;
extern void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
extern void OLED_Float(unsigned char Y,unsigned char X,double real,unsigned char N);
extern void OLED_Num2(unsigned char x,unsigned char y, int number);
extern void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size);
extern float longit,latit;
extern char longit_,latit_;
extern uint8_t flag;
int Count = 0;

void esp_connect(void)
{
	uint8_t Judge=1;
		while(Judge)
	{
		Judge = esp_Init();
		OLED_Num2(1,3,Judge);
	}  //连接阿里云直到成功
	
}
void Init(void)
{
	LED_Init();
	TIM3_Int_Init(100,7199);
	OLED_Init();
	OLED_Clear();
	MyUSART_Init(); //初始化串口
	usart3_init(9600);
	esp_connect();
}

int main(void)
{
	Init();
	
	
	while(1)
	{
		Count++;
		OLED_Float(1,1,longit,8);
		OLED_ShowChar(70,1,longit_,8);
		OLED_Float(2,1,latit,8);
		OLED_ShowChar(70,2,latit_,8);
		OLED_Float(4,1,flag,8);
		if(Count == 1000)
		{
			if(flag == 0) Esp_PUB();
			else esp_connect();
			Count = 0;
			
		}
		
//		if(flag == 0) Esp_PUB();
//		else esp_connect();
		
	}
	
}



