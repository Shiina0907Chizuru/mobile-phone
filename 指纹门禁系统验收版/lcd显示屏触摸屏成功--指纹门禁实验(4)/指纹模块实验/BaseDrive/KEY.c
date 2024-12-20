/**-------------------------------------------------------------------------------------------
** Created by:          qinyx
** Last modified Date:  2014-02-28
** Last Version:        V1.00
** Descriptions:        STM32F407嵌入式实验箱
**	  Gpio配置文件
**
**-------------------------------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "KEY.h"
#include "delay.h"
#include "LCD.h"
u8 key;//按键值

/******************************************************************************************
*函数名称：void KEYGpio_Init(void)
*
*入口参数：无
*
*出口参数：无
*
*功能说明：KEY初始化
//KEY_S1      PF8
//KEY_S2      PF9
//KEY_S3      PF10
*******************************************************************************************/
void KEYGpio_Init(void)
{    
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    //  KEY_S1  	PF8
	//  KEY_S2      PF9
	//  KEY_S3      PF10
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
}

/**********************************************************************************************************
函数名称：按键扫描函数
输入参数：无
输出参数：无
函数返回：按键值
**********************************************************************************************************/
unsigned char KeyScan(void)
{		 
    unsigned char buf[4] = {0};
    unsigned char temp = 0;
    static u8 key_up=1;
    
    buf[0] = KEY_S1_READ();
    buf[1] = KEY_S2_READ();
	buf[2] = KEY_S3_READ();
    
    if(key_up && (buf[0] == 0 || buf[1] == 0 || buf[2] == 0))
    {
        key_up = 0;
        
        delay_ms(100);
    
        buf[0] = KEY_S1_READ();
		buf[1] = KEY_S2_READ();
		buf[2] = KEY_S3_READ();
        
        //  KEY_S1
        if ( (buf[0] == 0) && (buf[1] == 1) && (buf[2] == 1))
        {
            temp = 1;
        }
        
        //  KEY_S2
        if ( (buf[0] == 1) && (buf[1] == 0) && (buf[2] == 1))
        {
            temp = 2;
        }
		
		//  KEY_S3
        if ( (buf[0] == 1) && (buf[1] == 1) && (buf[2] == 0))
        {
            temp = 3;
        }
    }
    else if (buf[0] == 1 && buf[1] == 1 && buf[2] == 1) key_up = 1;
    
    return temp;
}
/*************************************************
Function: my_key()
Description: 按键扫描(按键按下才能继续程序，没有按下按键时候等待)
Input: null
Output: null
Return: 按键值
Others: printf需要改成屏幕显示
Author: UU酱
*************************************************/
unsigned char my_key(void){
    int key_flag=1,MYkey=0;
    char keys[5];
    LCD_Clear(WHITE);
    LCD_ShowString(100,280, (u8 *)"请输入按键",BLACK,WHITE);
//    printf("\r\n");
    while(key_flag){
        key=KeyScan();
        if(key!=0){
            MYkey=key;
//            sprintf(keys,"%d",key);
//            LCD_Clear(WHITE);
//            LCD_ShowString(100,280, (u8 *)"key："+key,BLACK,WHITE);
//            LCD_ShowString(220,480, (u8 *)keys,BLACK,WHITE);
//            printf("key：%d\r\n",key);
            key_flag=0;
        }
    }
    return MYkey;
}