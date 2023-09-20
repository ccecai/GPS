/***********************************************
��˾����Ȥ�Ƽ�(��ݸ)���޹�˾
Ʒ�ƣ�WHEELTEC
������wheeltec.net
�Ա����̣�shop114407458.taobao.com 
����ͨ: https://minibalance.aliexpress.com/store/4455017
�汾��V1.0
�޸�ʱ�䣺2022-10-13

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version: V1.0
Update��2022-10-13

All rights reserved
***********************************************/
#include "usart3.h"

/**************************************************************************
�������ܣ�����3��ʼ��
��ڲ�������
�� �� ֵ����
**************************************************************************/
void USART3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStrue; //����һ�����ų�ʼ���Ľṹ��
	USART_InitTypeDef USART_InitStrue; //����һ�����ڳ�ʼ���Ľṹ��
	NVIC_InitTypeDef NVIC_InitStrue; //����һ���ж����ȼ���ʼ���Ľṹ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //GPIOBʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //����3ʱ��ʹ��
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP; //B10������Ϊ����3�����������ţ����츴�����
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_10; //����10
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz; //��Ϊ���ڷ�����������ʱ���ٶȿ���Ϊ����
  GPIO_Init(GPIOB,&GPIO_InitStrue); //�����������úõ�GPIO_InitStructure�������г�ʼ��
	
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING; //A10������Ϊ����1�����������ţ�������������������
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_11; //����11
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz; //��Ϊ���ڽ�����������ʱ���ٶȿ���Ϊ����
  GPIO_Init(GPIOB,&GPIO_InitStrue); //�����������úõ�GPIO_InitStructure�������г�ʼ��
	
	USART_InitStrue.USART_BaudRate=9600; //���崮�ڲ�����Ϊ9600bit/s
	USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None; //��Ӳ������������
	USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx; //���ͽ��ռ���ģʽ
	USART_InitStrue.USART_Parity=USART_Parity_No; //����żУ��λ
	USART_InitStrue.USART_StopBits=USART_StopBits_1; //һ��ֹͣλ
	USART_InitStrue.USART_WordLength=USART_WordLength_8b; //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_Init(USART3,&USART_InitStrue);//������������USART_InitStrue������ʼ������1
	
	USART_Cmd(USART3,ENABLE); //ʹ�ܴ���1
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE); //ʹ�ܽ����ж�void USART3_IRQHandler(void)
	
	NVIC_InitStrue.NVIC_IRQChannel=USART3_IRQn; //���ڴ���3�ж�
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE; //�ж�ʹ��
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=1; //��ռ���ȼ�Ϊ1����ֵԽС���ȼ�Խ�ߣ�0�����ȼ����
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=1; //��Ӧ���ȼ�Ϊ1����ֵԽС���ȼ�Խ�ߣ�0�����ȼ����
	NVIC_Init(&NVIC_InitStrue); ////����NVIC_InitStrue�Ĳ�����ʼ��VIC�Ĵ��������ô���1�ж����ȼ�
}


/**************************************************************************
�������ܣ�����3��������
��ڲ�������
�� �� ֵ����
**************************************************************************/
void usart3_send(u8 data)
{
	USART3->DR = data;
	while((USART3->SR&0x40)==0);	
}

/**************************************************************************
�������ܣ�����2�жϷ�����
��ڲ�������
����  ֵ����
**************************************************************************/
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE)) //���յ�����
	{	         	
	 
   }
}

/**************************************************************************
�������ܣ�����3�������ݺ���
��ڲ��������A�Ƕȣ����B�Ƕ�
����  ֵ����
**************************************************************************/
void usart3_sendAngleBlock(int Angle_A, int Angle_B)
{
	int  BlockCheck=0;

	BlockCheck=Angle_A^BlockCheck;
  BlockCheck=Angle_B^BlockCheck; //�����У��λ
	
	usart3_send(0xff);       //֡ͷ
	usart3_send(0xfe);       //֡ͷ
	usart3_send(Angle_A);    //���A�Ƕ�
	usart3_send(Angle_B);    //���B�Ƕ�
	usart3_send(0);    
	usart3_send(0);    
	usart3_send(0);    
	usart3_send(0);
	usart3_send(0);
	usart3_send(BlockCheck);    //BBCУ��λ
}
