#ifndef WIFI_H
#define WIFI_H

#include "stm32f4xx.h"

extern unsigned char id;

//  º¯ÊıÉùÃ÷
void WIFI_Configuration(void);
void WIFI_Send(unsigned char *ptr, unsigned int length);
void WIFI_Receive(void);

#endif
