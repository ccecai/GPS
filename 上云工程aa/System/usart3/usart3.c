#include "Delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
#include "LED.h"
#include "oled.h"

extern u8 Lora_mode;
//串口接收缓存区 	
int longitude_flag = 0,latitude_flag = 0,san_flag = 0,er_flag = 0;
float longitude[10] = {0},latitude[10] = {0};
char longit_,latit_;
float longit = 0,latit = 0;
int l_i = 0;
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			//发送缓冲,最大USART3_MAX_SEND_LEN字节
u8 Temp;
//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过timer,则认为不是1次连续数据.也就是超过timer没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART3_RX_STA=0;   	
int j = 0,start1_flag = 0,two_flag = 0,three_flag = 0,start2_flag;
float jindu,weidu;
int JINDU[20],WEIDU[20];
extern void OLED_Num2(unsigned char x,unsigned char y, int number);
extern void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size);
USART_InitTypeDef USART_InitStructure;
//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void usart3_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //串口3时钟使能

 	USART_DeInit(USART3);                           //复位串口3
   //USART3_TX   PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;      //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);          //初始化PB10
   
	//USART3_RX	  PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;           //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);                          //初始化PB11
	
	USART_InitStructure.USART_BaudRate = bound;                     //波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;     //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;             //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
 
	USART_Cmd(USART3, ENABLE);                  //使能串口 
	
	//使能接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断   
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	TIM3_Int_Init(99,7199);	//10ms中断
	USART3_RX_STA=0;		//清零
	TIM_Cmd(TIM3,DISABLE);	//关闭定时器7
}

//串口3,printf 函数 发送端LORA模块发送数据
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);  //使用参数列表发送格式化输出到字符串
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	  USART_SendData(USART3,USART3_TX_BUF[j]); 
	} 
}


 
//串口接收使能控制
//enable:0,关闭 1,打开
void usart3_rx(u8 enable)
{
	 USART_Cmd(USART3, DISABLE); //失能串口 
	
	 if(enable)
	 {
		 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式
	 }else
	 {
		 USART_InitStructure.USART_Mode = USART_Mode_Tx;//只发送 
	 }
	 
	 USART_Init(USART3, &USART_InitStructure); //初始化串口3
   USART_Cmd(USART3, ENABLE); //使能串口 
	
}
uint8_t Serial_TxPacket[8];				//FF 01 02 03 04 FE
uint8_t Serial_RxPacket[8];
uint8_t Serial_RxFlag;
uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}
void USART3_IRQHandler(void)
{
	static int Res;
	
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		Res = USART_ReceiveData(USART3);
		if(longitude_flag == 1 && san_flag == 1)
		{
			longitude[l_i] = Res;
			if(l_i == 8)
			{
			
				longit = longitude[0]*100+longitude[1]*10+longitude[2]*1+longitude[3]/10+longitude[4]/100+longitude[5]/1000+longitude[6]/10000;
				longit_ = longitude[8];
			}
			l_i++;
		}
		if(longitude_flag == 1 && er_flag == 1)
		{
			longitude[l_i] = Res;
			if(l_i == 7)
			{
				longit = longitude[0]*10+longitude[1]*1+longitude[2]/10+longitude[3]/100+longitude[4]/1000+longitude[5]/10000;
				longit_ = longitude[7];
			}
			l_i++;
		}
		if(latitude_flag == 1 && san_flag == 1)
		{
			latitude[l_i] = Res;
			if(l_i == 8)
			{
				latit = latitude[0]*100+latitude[1]*10+latitude[2]*1+latitude[3]/10+latitude[4]/100+latitude[5]/1000+latitude[6]/10000;
				latit_ = latitude[8];
			}
			l_i++;
		}
		if(latitude_flag == 1 && er_flag == 1)
		{
			latitude[l_i] = Res;
			if(l_i == 7)
			{
				latit = latitude[0]*10+latitude[1]*1+latitude[2]/10+latitude[3]/100+latitude[4]/1000+latitude[5]/10000;
				latit_ = latitude[7];
			}
			l_i++;
		}
		if(Res == 'b')
		{
			san_flag = 1;
		}
		if(Res == 'c')
		{
			er_flag = 1;
		}
		if(Res == 'e')
		{
			san_flag = 0;
			er_flag = 0;
			longitude_flag = 0;
			latitude_flag = 0;
			l_i = 0;
		}
		if(Res == 'a')
		{
			longitude_flag = 1;
		}
		if(Res == 'd')
		{
			latitude_flag = 1;
		}
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}
