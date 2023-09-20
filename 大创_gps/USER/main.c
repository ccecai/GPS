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

//ALIENTEK mini��������չʵ��16
//ATK-S1216F8 GPSģ�����ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
u8 USART1_TX_BUF[USART2_MAX_RECV_LEN]; 					//����1,���ͻ�����
nmea_msg gpsx; 											//GPS��Ϣ
__align(4) u8 dtbuf[50];   								//��ӡ������
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode�ַ��� 
int TP;

//��ʾGPS��λ��Ϣ 
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
		usart3_send('b'); //����һ�ٱ�־λ
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
		usart3_send('c'); //����һ�ٱ�־λ
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
	sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//�õ������ַ���
 
	tp=gpsx.latitude;	   
	
	OLED_Float(2,1,tp/100000,8);
	OLED_ShowChar(70,2,'N',8);
	
	usart3_send('d'); //֡ͷ
	if(tp/100000>100) 
	{
		
		usart3_send('b'); //����һ�ٱ�־λ
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
		usart3_send('c'); //����һ�ٱ�־λ
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
	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//�õ�γ���ַ���
	 
	
}
 int main(void)
 { 
	u16 i,rxlen;
	u16 lenx;
	u8 key=0XFF;
	u8 upload=0; 	  
	delay_init();	    	 //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�	  
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600	
	USART2_Init(38400);  //��ʼ������2������Ϊ115200
//	LCD_Init();				//��ʼ��Һ�� 
	OLED_Init();
	 OLED_Clear();
//	LED_Init();         	//LED��ʼ��	 
//	KEY_Init();				//������ʼ��
	 USART3_Init();
	usmart_dev.init(72);	//usmart��ʼ��	
	if(SkyTra_Cfg_Rate(5)!=0)	//���ö�λ��Ϣ�����ٶ�Ϊ5Hz,˳���ж�GPSģ���Ƿ���λ. 
	{
   
		do
		{
			USART2_Init(9600);			//��ʼ������3������Ϊ9600
	  	SkyTra_Cfg_Prt(3);			//��������ģ��Ĳ�����Ϊ38400
			USART2_Init(38400);			//��ʼ������3������Ϊ38400
      key=SkyTra_Cfg_Tp(100000);	//������Ϊ100ms
		}while(SkyTra_Cfg_Rate(5)!=0&&key!=0);//����SkyTraF8-BD�ĸ�������Ϊ5Hz
	 
		delay_ms(500);
		
	}
	while(1) 
	{		
		delay_ms(1);
		Gps_Msg_Show();
		if(USART2_RX_STA&0X8000)		//���յ�һ��������
		{
			rxlen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART2_RX_BUF[i];	   
 			USART2_RX_STA=0;		   	//������һ�ν���
			USART1_TX_BUF[i]=0;			//�Զ���ӽ�����
			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//�����ַ���
			Gps_Msg_Show();				//��ʾ��Ϣ	
			if(upload)printf("\r\n%s\r\n",USART1_TX_BUF);//���ͽ��յ������ݵ�����1
 		}
		
	}
}
 
























