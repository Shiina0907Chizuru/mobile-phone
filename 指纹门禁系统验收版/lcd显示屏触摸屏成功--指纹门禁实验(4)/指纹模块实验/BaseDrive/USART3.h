#ifndef USART3_H
#define USART3_H

//  ����USART�������ݰ�
struct UsartData                                                        
{		
	unsigned char *Rxbuf;
    unsigned int   RXlenth;
    unsigned char  Time;
    unsigned char  ReceiveFinish;
};
typedef  struct UsartData USARTDATA;
typedef  USARTDATA       *PUSARTDATA;

extern USARTDATA   Uart3;
extern USARTDATA   Uart6;

// ��������
void UART3_Configuration(void);
void USART3_Senddata(unsigned char *Data, unsigned int length);
void UART6_Configuration(unsigned int baud);
void USART6_Senddata(unsigned char *Data, unsigned int length);

#endif
