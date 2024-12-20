#ifndef USART_H
#define USART_H

#include "stdio.h"

//  用于USART接收数据包
struct UsartData                                                        
{		
	unsigned char *Rxbuf;
    unsigned int   RXlenth;
    unsigned char  Time;
    unsigned char  ReceiveFinish;
};
typedef  struct UsartData USARTDATA;
typedef  USARTDATA       *PUSARTDATA;

// 变量声明
extern USARTDATA   Uart3;
extern USARTDATA   Uart6;
// 函数声明
void UART3_Configuration(void);
void USART3_Senddata(unsigned char *Data, unsigned int length);

#endif
