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
#ifndef __USRAT3_H
#define __USRAT3_H 
#include "sys.h"

void USART3_Init(void);
void usart3_send(u8 data);
void USART3_IRQHandler(void);
void usart3_sendAngleBlock(int Angle_A, int Angle_B);
#endif
