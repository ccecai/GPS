#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"     
#include "usmart.h"
#include "gps.h"
#include "string.h"		
#include "usart2.h"
#include "oled.h"
#include "usart3.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//ALIENTEK mini开发板扩展实验16
//ATK-S1216F8 GPS模块测试实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
u8 USART1_TX_BUF[USART2_MAX_RECV_LEN]; 					//串口1,发送缓存区
nmea_msg gpsx; 											//GPS信息
__align(4) u8 dtbuf[50];   								//打印缓存器
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode字符串 
int TP;

//显示GPS定位信息 
void Gps_Msg_Show(void)
{
 	float tp;		   
	long Tp;
	
	tp=gpsx.longitude;	   
	
	OLED_Float(1,1,tp/100000,8);
	OLED_ShowChar(70,1,'E',8);
	if(tp/100000>100) 
	{
		OLED_Float(1,1,tp/100000,8);
		usart3_send('a');
		usart3_send('b'); //大于一百标志位
		Tp = (long)tp;
		usart3_send(Tp/10000000);
		Tp%=10000000;
		usart3_send(Tp/1000000);
		Tp%=1000000;
		usart3_send(Tp/100000);
		Tp%=100000;
		usart3_send(Tp/10000);
		Tp%=10000;
		usart3_send(Tp/1000);
		Tp%=1000;
		usart3_send(Tp/100);
		Tp%=100;
		usart3_send(Tp/10);
		Tp%=10;
		usart3_send(Tp);
		delay_ms(10);
		usart3_send('E');
		delay_ms(10);
		usart3_send('e');
	}
	else
	{	
		
		usart3_send('a');
		usart3_send('c'); //大于一百标志位
		Tp = (long)tp;
		usart3_send(Tp/1000000);
		Tp%=1000000;
		usart3_send(Tp/100000);
		Tp%=100000;
		usart3_send(Tp/10000);
		Tp%=10000;
		usart3_send(Tp/1000);
		Tp%=1000;
		usart3_send(Tp/100);
		Tp%=100;
		usart3_send(Tp/10);
		Tp%=10;
		usart3_send(Tp);
		delay_ms(10);
		usart3_send('E');
		delay_ms(10);
		usart3_send('e');
	}
	sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//得到经度字符串
 
	tp=gpsx.latitude;	   
	
	OLED_Float(2,1,tp/100000,8);
	OLED_ShowChar(70,2,'N',8);
	
	usart3_send('d'); //帧头
	if(tp/100000>100) 
	{
		
		usart3_send('b'); //大于一百标志位
		Tp =(long) tp;
		usart3_send(Tp/10000000);
		Tp%=10000000;
		usart3_send(Tp/1000000);
		Tp%=1000000;
		usart3_send(Tp/100000);
		Tp%=100000;
		usart3_send(Tp/10000);
		Tp%=10000;
		usart3_send(Tp/1000);
		Tp%=1000;
		usart3_send(Tp/100);
		Tp%=100;
		usart3_send(Tp/10);
		Tp%=10;
		usart3_send(Tp);
		delay_ms(10);
		usart3_send('N');
		delay_ms(10);
		usart3_send('e');
	}
	else
	{
		usart3_send('c'); //大于一百标志位
		Tp =(long) tp;
		usart3_send(Tp/1000000);
		Tp%=1000000;
		usart3_send(Tp/100000);
		Tp%=100000;
		usart3_send(Tp/10000);
		Tp%=10000;
		usart3_send(Tp/1000);
		Tp%=1000;
		usart3_send(Tp/100);
		Tp%=100;
		usart3_send(Tp/10);
		Tp%=10;
		usart3_send(Tp);
		delay_ms(10);
		usart3_send('N');
		delay_ms(10);
		usart3_send('e');
	}
	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//得到纬度字符串
	 
	
}
 int main(void)
 { 
	u16 i,rxlen;
	u16 lenx;
	u8 key=0XFF;
	u8 upload=0; 	  
	delay_init();	    	 //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级	  
	uart_init(115200);	 	//串口初始化为9600	
	USART2_Init(38400);  //初始化串口2波特率为115200
//	LCD_Init();				//初始化液晶 
	OLED_Init();
	 OLED_Clear();
//	LED_Init();         	//LED初始化	 
//	KEY_Init();				//按键初始化
	 USART3_Init();
	usmart_dev.init(72);	//usmart初始化	
	if(SkyTra_Cfg_Rate(5)!=0)	//设置定位信息更新速度为5Hz,顺便判断GPS模块是否在位. 
	{
   
		do
		{
			USART2_Init(9600);			//初始化串口3波特率为9600
	  	SkyTra_Cfg_Prt(3);			//重新设置模块的波特率为38400
			USART2_Init(38400);			//初始化串口3波特率为38400
      key=SkyTra_Cfg_Tp(100000);	//脉冲宽度为100ms
		}while(SkyTra_Cfg_Rate(5)!=0&&key!=0);//配置SkyTraF8-BD的更新速率为5Hz
	 
		delay_ms(500);
		
	}
	while(1) 
	{		
		delay_ms(1);
		Gps_Msg_Show();
		if(USART2_RX_STA&0X8000)		//接收到一次数据了
		{
			rxlen=USART2_RX_STA&0X7FFF;	//得到数据长度
			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART2_RX_BUF[i];	   
 			USART2_RX_STA=0;		   	//启动下一次接收
			USART1_TX_BUF[i]=0;			//自动添加结束符
			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//分析字符串
			Gps_Msg_Show();				//显示信息	
			if(upload)printf("\r\n%s\r\n",USART1_TX_BUF);//发送接收到的数据到串口1
 		}
		
	}
}
 
























