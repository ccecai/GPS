/***********************************************
公司：轮趣科技(东莞)有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：V1.0
修改时间：2022-10-13

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version: V1.0
Update：2022-10-13

All rights reserved
***********************************************/
#include "usart3.h"

/**************************************************************************
函数功能：串口3初始化
入口参数：无
返 回 值：无
**************************************************************************/
void USART3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStrue; //定义一个引脚初始化的结构体
	USART_InitTypeDef USART_InitStrue; //定义一个串口初始化的结构体
	NVIC_InitTypeDef NVIC_InitStrue; //定义一个中断优先级初始化的结构体
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //GPIOB时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //串口3时钟使能
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP; //B10引脚作为串口3发送数据引脚，推挽复用输出
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_10; //引脚10
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz; //作为串口发送数据引脚时该速度可以为任意
  GPIO_Init(GPIOB,&GPIO_InitStrue); //根据上面设置好的GPIO_InitStructure参数进行初始化
	
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING; //A10引脚作为串口1接收数据引脚，浮空输入或带上拉输入
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_11; //引脚11
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz; //作为串口接收数据引脚时该速度可以为任意
  GPIO_Init(GPIOB,&GPIO_InitStrue); //根据上面设置好的GPIO_InitStructure参数进行初始化
	
	USART_InitStrue.USART_BaudRate=9600; //定义串口波特率为9600bit/s
	USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx; //发送接收兼容模式
	USART_InitStrue.USART_Parity=USART_Parity_No; //无奇偶校验位
	USART_InitStrue.USART_StopBits=USART_StopBits_1; //一个停止位
	USART_InitStrue.USART_WordLength=USART_WordLength_8b; //字长为8位数据格式
	USART_Init(USART3,&USART_InitStrue);//根据上面设置USART_InitStrue参数初始化串口1
	
	USART_Cmd(USART3,ENABLE); //使能串口1
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE); //使能接收中断void USART3_IRQHandler(void)
	
	NVIC_InitStrue.NVIC_IRQChannel=USART3_IRQn; //属于串口3中断
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE; //中断使能
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=1; //抢占优先级为1级，值越小优先级越高，0级优先级最高
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=1; //响应优先级为1级，值越小优先级越高，0级优先级最高
	NVIC_Init(&NVIC_InitStrue); ////根据NVIC_InitStrue的参数初始化VIC寄存器，设置串口1中断优先级
}


/**************************************************************************
函数功能：串口3发送数据
入口参数：无
返 回 值：无
**************************************************************************/
void usart3_send(u8 data)
{
	USART3->DR = data;
	while((USART3->SR&0x40)==0);	
}

/**************************************************************************
函数功能：串口2中断服务函数
入口参数：无
返回  值：无
**************************************************************************/
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE)) //接收到数据
	{	         	
	 
   }
}

/**************************************************************************
函数功能：串口3发送数据函数
入口参数：舵机A角度，舵机B角度
返回  值：无
**************************************************************************/
void usart3_sendAngleBlock(int Angle_A, int Angle_B)
{
	int  BlockCheck=0;

	BlockCheck=Angle_A^BlockCheck;
  BlockCheck=Angle_B^BlockCheck; //异或求校验位
	
	usart3_send(0xff);       //帧头
	usart3_send(0xfe);       //帧头
	usart3_send(Angle_A);    //舵机A角度
	usart3_send(Angle_B);    //舵机B角度
	usart3_send(0);    
	usart3_send(0);    
	usart3_send(0);    
	usart3_send(0);
	usart3_send(0);
	usart3_send(BlockCheck);    //BBC校验位
}
