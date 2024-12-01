#include "StepMotor.h"
#include "delay.h"

/******************************************************************************************
*�������ƣ�void RelayGpio_Init(void)
*
*��ڲ�������
*
*���ڲ�������
*
*����˵����LED��ʼ��
//MOTOR_A      PE3
//MOTOR_B      PE2
//MOTOR_C      PE5
//MOTOR_D      PE4
*******************************************************************************************/
void StepMotorGpio_Init(void)
{    
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    MOTOR_A_L;
    MOTOR_B_L;
    MOTOR_C_L;
    MOTOR_D_L;
}

/**********************************************************************************************************
�������ƣ�����������Ժ���
���������ģʽ��0Ϊ��ת��1Ϊ��ת
�����������
**********************************************************************************************************/
void StepMotorTest(unsigned char mode)
{
    unsigned int i = 0;
    
    if (mode == 0)
    {
        for ( i =0; i < 500; i++)
        {
            MOTOR_A_L;
            delay_ms(5);
            MOTOR_A_H;
            
            MOTOR_B_L;
            delay_ms(5);
            MOTOR_B_H;
            
            MOTOR_C_L;
            delay_ms(5);
            MOTOR_C_H;
            
            MOTOR_D_L;
            delay_ms(5);
            MOTOR_D_H; 
        }
    }
    
    else
    {
        for ( i =0; i < 500; i++)
        {
            MOTOR_D_L;
            delay_ms(5);
            MOTOR_D_H;
            
            MOTOR_C_L;
            delay_ms(5);
            MOTOR_C_H;
            
            MOTOR_B_L;
            delay_ms(5);
            MOTOR_B_H;
            
            MOTOR_A_L;
            delay_ms(5);
            MOTOR_A_H; 
        }
    }
    
    MOTOR_A_L;
    MOTOR_B_L;
    MOTOR_C_L;
    MOTOR_D_L;
}
