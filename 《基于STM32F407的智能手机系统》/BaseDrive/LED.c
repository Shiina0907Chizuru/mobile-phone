/**-------------------------------------------------------------------------------------------
** Created by:          qinyx
** Last modified Date:  2014-02-28
** Last Version:        V1.00
** Descriptions:        STM32F407Ƕ��ʽʵ����
**	  Gpio�����ļ�
**
**-------------------------------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "LED.h"
#include "delay.h"

/*************************************************************************
*�������ƣ�void LEDGpio_Init(void)
*
*��ڲ�������
*
*���ڲ�������
*
*����˵����LED��ʼ��
//LED1      PF0
//LED2      PF1
//LED3      PF2
//LED4      PF3
//LED5      PF4
//LED6      PF5
//LED7      PF6
//LED8      PF7
*************************************************************************/
void LEDGpio_Init(void)
{    
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    //  LED
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);   //ʹ��GPIOBʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//LED��Ӧ����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //ͨ�����ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //������������
    GPIO_Init(GPIOF, &GPIO_InitStructure);                  //��ʼ�� GPIO
    
    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
    LED4_OFF;
    LED5_OFF;
    LED6_OFF;
    LED7_OFF;
    LED8_OFF;
}
void BEEPGpio_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    //  BEEP
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);   //ʹ��GPIOCʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//BEEP��Ӧ����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //ͨ�����ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //������������
    GPIO_Init(GPIOC, &GPIO_InitStructure);                  //��ʼ�� GPIO
    BEEP_OFF;
}
void BEEP1(void)
{
    int n;
    for(n=0; n<3; n++)
    {
        BEEP_ON;
        delay_ms(200);
        BEEP_OFF;
        delay_ms(200);
    }
}
// ���������ʾ
void BEEP2(void)
{
    BEEP_ON;
    delay_ms(500);
    BEEP_OFF;
}
void BEEP3(void)
{
        BEEP_ON;
        delay_ms(200);
        BEEP_OFF;


}
