#include "Delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
#include "LED.h"
#include "oled.h"

extern u8 Lora_mode;
//���ڽ��ջ����� 	
int longitude_flag = 0,latitude_flag = 0,san_flag = 0,er_flag = 0;
float longitude[10] = {0},latitude[10] = {0};
char longit_,latit_;
float longit = 0,latit = 0;
int l_i = 0;
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
u8 Temp;
//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������timer,����Ϊ����1����������.Ҳ���ǳ���timerû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART3_RX_STA=0;   	
int j = 0,start1_flag = 0,two_flag = 0,three_flag = 0,start2_flag;
float jindu,weidu;
int JINDU[20],WEIDU[20];
extern void OLED_Num2(unsigned char x,unsigned char y, int number);
extern void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size);
USART_InitTypeDef USART_InitStructure;
//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void usart3_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //����3ʱ��ʹ��

 	USART_DeInit(USART3);                           //��λ����3
   //USART3_TX   PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;      //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);          //��ʼ��PB10
   
	//USART3_RX	  PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;           //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);                          //��ʼ��PB11
	
	USART_InitStructure.USART_BaudRate = bound;                     //������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;     //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;             //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	
	USART_Init(USART3, &USART_InitStructure); //��ʼ������3
 
	USART_Cmd(USART3, ENABLE);                  //ʹ�ܴ��� 
	
	//ʹ�ܽ����ж�
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�   
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM3_Int_Init(99,7199);	//10ms�ж�
	USART3_RX_STA=0;		//����
	TIM_Cmd(TIM3,DISABLE);	//�رն�ʱ��7
}

//����3,printf ���� ���Ͷ�LORAģ�鷢������
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u3_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);  //ʹ�ò����б��͸�ʽ��������ַ���
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
	  USART_SendData(USART3,USART3_TX_BUF[j]); 
	} 
}


 
//���ڽ���ʹ�ܿ���
//enable:0,�ر� 1,��
void usart3_rx(u8 enable)
{
	 USART_Cmd(USART3, DISABLE); //ʧ�ܴ��� 
	
	 if(enable)
	 {
		 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ
	 }else
	 {
		 USART_InitStructure.USART_Mode = USART_Mode_Tx;//ֻ���� 
	 }
	 
	 USART_Init(USART3, &USART_InitStructure); //��ʼ������3
   USART_Cmd(USART3, ENABLE); //ʹ�ܴ��� 
	
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
