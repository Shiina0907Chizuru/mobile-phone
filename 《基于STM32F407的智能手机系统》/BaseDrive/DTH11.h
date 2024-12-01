#ifndef __DHT11_H
#define __DHT11_H 			   
#include "stm32f4xx.h"

//DHT11_PIN         PE6		
#define  	DHT11_L      	GPIO_ResetBits(GPIOE, GPIO_Pin_6)
#define  	DHT11_H      	GPIO_SetBits(GPIOE, GPIO_Pin_6)		

#define  	DHT11_READ()    GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6)

void DTH11_OUT(void);
void DTH11_IN(void);
void DTH11_Delay(unsigned int j);
void Delay_10us(void);
unsigned char DTH11_ReadByte(void);
void DTH11_ReadData(unsigned char *temp, unsigned char *humi);

#endif
