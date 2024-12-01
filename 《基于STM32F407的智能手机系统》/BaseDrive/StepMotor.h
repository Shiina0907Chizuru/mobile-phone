#ifndef _STEPMOTOR_H_
#define _STEPMOTOR_H_

#include "stm32f4xx.h"

/*硬件资源引脚定义
//MOTOR_A      PE3
//MOTOR_B      PE2
//MOTOR_C      PE5
//MOTOR_D      PE4
*/
//MOTOR_A   PE3
#define  	MOTOR_A_L       GPIO_ResetBits(GPIOE, GPIO_Pin_3)
#define  	MOTOR_A_H       GPIO_SetBits(GPIOE, GPIO_Pin_3)

//MOTOR_B   PE2
#define  	MOTOR_B_L       GPIO_ResetBits(GPIOE, GPIO_Pin_2)
#define  	MOTOR_B_H       GPIO_SetBits(GPIOE, GPIO_Pin_2)

//MOTOR_C   PE5
#define  	MOTOR_C_L       GPIO_ResetBits(GPIOE, GPIO_Pin_5)
#define  	MOTOR_C_H       GPIO_SetBits(GPIOE, GPIO_Pin_5)

//MOTOR_D   PE4
#define  	MOTOR_D_L       GPIO_ResetBits(GPIOE, GPIO_Pin_4)
#define  	MOTOR_D_H       GPIO_SetBits(GPIOE, GPIO_Pin_4)


//	函数声明
void StepMotorGpio_Init(void);
void StepMotorTest(unsigned char mode);
#endif
