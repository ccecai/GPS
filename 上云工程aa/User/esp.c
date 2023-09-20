#include "stm32f10x.h"                  // Device header
#include "MyUSART.h"
#include <stdio.h>
#include <string.h>
#include "Delay.h"
#include "oled.h"
int turn_flag = 0,bk = 1;
extern void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size);
extern void OLED_Num2(unsigned char x,unsigned char y, int number);
extern int LightLux;
extern int soil_humidity;
extern int soil_temperature;
extern int Humidity;
extern int temperature;
extern float fspeed;
extern int fangle;
extern int level;
extern char RECS[250];
extern int status;
const char* WIFI ="OnePlus 9";
const char* WIFIASSWORD="88888888";
const char* ClintID="public";
const char* username="admin";
const char* passwd="123456";
const char* Url="47.109.133.87";
const char* pubtopic="cattest";
const char* func1="Longitude";
//double Latitude = 104.123456;
const char* func2="Latitude";	
const char* ntp = "ntp1.aliyun.com";
extern float longit,latit;
extern void esp_connect(void);
uint8_t flag = 1;
//const char* func8="level";
int fputc(int ch,FILE *f )   //printf重定向  
{
	USART_SendData(USART1,(uint8_t)ch);
	while(USART_GetFlagStatus (USART1,USART_FLAG_TC) == RESET);
	return ch;
}
char esp_Init(void)
{
	memset(RECS,0,sizeof(RECS));
	printf("AT+RST\r\n");  //重启
	Delay_ms(2000);
	
	memset(RECS,0,sizeof(RECS));
	printf("AT+CWMODE=1\r\n"); //Station模式
	Delay_ms(1000);
	if(strcmp(RECS,"OK"))
		return 1;
//	memset(RECS,0,sizeof(RECS));
//	printf("AT+CIPSNTPCFG=1,8,\"%s\"\r\n",ntp);    //关闭回显
//	Delay_ms(1000);
//	if(strcmp(RECS,"OK"))
//		return 2;
	memset(RECS,0,sizeof(RECS));
	printf("AT+CWJAP=\"%s\",\"%s\"\r\n",WIFI,WIFIASSWORD); //连接热点
	
	Delay_ms(2000);
	if(strcmp(RECS,"OK"))
		return 3;
	
	memset(RECS,0,sizeof(RECS));
	printf("AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",ClintID,username,passwd);//用户信息配置
	Delay_ms(100);
	if(strcmp(RECS,"OK"))
		return 4;
	while(bk)
	{
		memset(RECS,0,sizeof(RECS));
		printf("AT+MQTTCONN=0,\"%s\",1883,0\r\n",Url); //连接服务器
		Delay_ms(2000);
		if(strcmp(RECS,"OK"))
		{
			bk = 1;
		}
		else
		{
			bk = 0;
		}
	}
		
	flag = 0;
	return 0;
}


char Esp_PUB(void)
{
	
		status=1;
		memset(RECS,0,sizeof(RECS));
	printf("AT+MQTTPUB=0,\"%s\",\"{\\\"%s\\\":\\\"%0.4f\\\"\\,\\\"%s\\\":\\\"%0.4f\\\"}\",0,0\r\n",pubtopic,func1,longit,func2,latit);
		Delay_ms(200);
		if(strcmp(RECS,"OK"))
		{
			flag = 1;
		}
//		
//		status=1;
//		memset(RECS,0,sizeof(RECS));
//		printf("AT+MQTTPUB=0,\"%s\",\"{\\\"%s\\\":\\\"%0.4f\\\"}\",0,0\r\n",pubtopic,func2,latit);
//		Delay_ms(200);
//		if(strcmp(RECS,"OK"))
//		{
//			flag = 1;
//		}
		//printf("AT+MQTTPUB=0,\"%s\",\"{\\\"method\\\":\\\"thing.event.property.post\\\"\\,\\\"params\\\":{\\\"%s\\\":%f\\,\\\"%s\\\":%d}}\",0,0\r\n",pubtopic,func6,fspeed,func8,level);
		//while(RECS[0]);
		//Delay_ms(200);//延时等待数据接收完成
		//if(strcmp(RECS,"ERROR")==0)
			//return 1;
		return 0;
	
	
}
