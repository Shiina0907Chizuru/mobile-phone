/****************************************Copyright (c)***************************************
**-------------------------------------------------------------------------------------------
** Created by:          qinyx
** Last modified Date:  2014-02-28
** Last Version:        V1.00
** Descriptions:        STM32F407Ƕ��ʽʵ����
**-------------------------------------------------------------------------------------------*/
//********************************************************************************************/
#include "stm32f4xx.h"
#include "USART3.h"
#include "delay.h"
#include "stdio.h"
#include "KEY.h"
#include "mp3.h"
#include "LED.h"
#include "WIFI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "GPS.h"	 
#include "lcd.h"	 
#include "touch.h"	 
#include "EXIT.h"
#include "TIME.h"
#include "ADC.h"
#include "StepMotor.h"
#include "PWM.h"
#include "DCMotor.h"
#include "CH455I2C.H"
#include "I2C.h"
#include "stm32f4xx_it.h"
int main(void)
{
	int enter=0;
    // float t       emp;
    SysTick_Init();		//��ʱ��ʼ��
    LEDGpio_Init();			
    BEEPGpio_Init();
    KEYGpio_Init();  	
	  LCD_Init();			//LCD��ʼ��
	  Adc_Init();
	  UART3_Configuration();   
	  UART5_Configuration(9600);  
    UART6_Configuration(9600);  
	  setMp3Dev(FLASH0);                                                  
    delay_ms(200); DCMotorGpio_Init();
	 // setMp3Vol(15);
//    TIM2_Configuration();
//	  EXTI_Config();  //  �����ⲿ�ж�0
    tp_dev.init();				//��������ʼ��

   POINT_COLOR = BLUE;          
   enter=In();
    
	  while(enter)
		{
			menuset();
			apps();
		}

}
