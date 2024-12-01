/**-------------------------------------------------------------------------------------------
** Created by:          qinyx
** Last modified Date:  2014-02-28
** Last Version:        V1.00
** Descriptions:        STM32F407Ƕ��ʽʵ����
**
**-------------------------------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "EXIT.h"

/*************************************************************************
*�������ƣ�void EXTI_Config(void)
*
*��ڲ�������
*
*���ڲ�������
*
*����˵�����ⲿ�жϳ�ʼ��
����S1����Ӧ������PF8���ⲿ�ж�8
*************************************************************************/
void EXTI_Config(void)
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);   //  ʹ��GPIOFʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  //  ʹ��SYSCFGʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;            //  ����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //  ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;               //  S1��Ӧ����
    GPIO_Init(GPIOF, &GPIO_InitStructure);                  //  ��ʼ��GPIO 

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource8);   //  ��ʼ���ж���0

    EXTI_InitStructure.EXTI_Line = EXTI_Line8;              //  �����ж���Ϊ�ж���8
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     //  �����ж�ģʽ
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //  ����Ϊ�½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;               //  �����ж���ʹ��
    EXTI_Init(&EXTI_InitStructure);                         //  ����

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;      //�ⲿ�ж� 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;    //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;   //��Ӧ���ȼ�    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);                         //���� NVIC
}


void EXTI1_Config(void)
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);   //  ʹ��GPIOFʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  //  ʹ��SYSCFGʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;            //  ����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //  ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;               //  S1��Ӧ����
    GPIO_Init(GPIOF, &GPIO_InitStructure);                  //  ��ʼ��GPIO 

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource10);   //  ��ʼ���ж���0

    EXTI_InitStructure.EXTI_Line = EXTI_Line10;              //  �����ж���Ϊ�ж���8
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     //  �����ж�ģʽ
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //  ����Ϊ�½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;               //  �����ж���ʹ��
    EXTI_Init(&EXTI_InitStructure);                         //  ����

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;      //�ⲿ�ж� 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;    //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;   //��Ӧ���ȼ�    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);                         //���� NVIC
}
