#include "stm32f10x.h"                  // Device header
#include "MyUSART.h"
#include "esp.h"
#include <string.h>
#include "OLED.h"
//#include <stdbool.h> // 引入 bool 类型
#include <stdlib.h> // 引入 atoi 函数
#include <delay.h>
#define MAX_BUFFER_SIZE 128
#define BUFFER_SIZE 100

// 接收缓冲区
char rx_buffer[BUFFER_SIZE];
// 接收缓冲区的索引
uint16_t rx_index = 0;
#include "usart3.h"
uint8_t rxBuffer[MAX_BUFFER_SIZE];
uint8_t rxIndex = 0;
int isParsing = 0; // 用于标记是否处于解析状态
int ledValue = 0;

char RECS[250];
unsigned char i;
int status=0;


// 在源字符串中查找目标字符串
// 返回目标字符串在源字符串中的起始位置，如果未找到则返回 NULL
const char* custom_strstr(const char* src, const char* target)
{
    while (*src != '\0')
    {
        const char* s = src;
        const char* t = target;

        while (*t != '\0' && *s == *t)
        {
            s++;
            t++;
        }

        if (*t == '\0')
        {
            return src; // 找到目标字符串，返回起始位置
        }

        src++;
    }

    return NULL; // 未找到目标字符串
}
void MyUSART_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 ,ENABLE);//1.开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;                 //初始化GIPIO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;              //初始化串口
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity =USART_Parity_No ;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init (USART1,&USART_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel =USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);
	
}
char* MyUSART_GetString(void)
{
	return RECS;
}
void MyUSART_SendString(char* str)
{
	uint8_t stri=0;
	while(str[stri] != '\0')
		USART_SendData (USART1,str[stri++]);
}
int isDigit(char c)
{
  return (c >= '0' && c <= '9');
}
int status1=0;
int status2=0;
void USART1_IRQHandler()
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE) )
	{
		RECS[i++]=USART_ReceiveData(USART1);
		if((RECS[i-2]=='\r')|(RECS[i-1]=='\n'))  
		{
			RECS[i-2]='\0';
			i = 0;
		}
	}


        // 判断是否接收到完整的数据包，可以根据特定字符来判断结束标志
        // 这里以'\n'作为结束标志
        
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		/*
        uint8_t data = USART_ReceiveData(USART1);
	if (isParsing)
        {
			//OLED_ShowNum(3,1,1,1);
            // 仅处理数字字符
            if (data == '1' )
            {
                // 存储 "led": 后面的数字字符
                
				OLED_ShowNum(3,1,1,1);
            }
            if  (data == '2')
			{
				OLED_ShowNum(3,1,0,1);
			}
			isParsing = 0;
        }
        // 标记开始解析，如果接收到 '\"'
		
		if (data == 'd'&&status2 == 1)
			{
				status1=1;
			}
			if(status1>0)
			{
			status1++;
		}
        if (status1==3)
        {
			if(data == ':')
            {
				isParsing = 1;
				OLED_ShowNum(2,1,1,1);
			}
			OLED_ShowNum(4,1,1,1);
			u3_printf("1");
			status1=0;
			
		}
		status2=0;
		if (data == 'e')
		{
			status2=1;
		}
    }*/
}

//void USART1_IRQHandler(void)
//{
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//    {
//        uint8_t data = USART_ReceiveData(USART1);
//	if (isParsing)
//        {
//			//OLED_ShowNum(3,1,1,1);
//            // 仅处理数字字符
//            if (data == '1' )
//            {
//                // 存储 "led": 后面的数字字符
//                
//				OLED_ShowNum(3,1,1,1);
//            }
//            if  (data == '0')
//			{
//				OLED_ShowNum(3,1,0,1);
//			}	
//        }
//        // 标记开始解析，如果接收到 '\"'
//        if (data == ':')
//        {
//            isParsing = 1;
//            
//			OLED_ShowNum(2,1,1,1);
//        }

//        
//    }
//}

