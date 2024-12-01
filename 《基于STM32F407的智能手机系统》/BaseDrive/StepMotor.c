#include "StepMotor.h"
#include "delay.h"

/******************************************************************************************
*函数名称：void RelayGpio_Init(void)
*
*入口参数：无
*
*出口参数：无
*
*功能说明：LED初始化
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
函数名称：步进电机测试函数
输入参数：模式，0为正转，1为反转
输出参数：无
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
