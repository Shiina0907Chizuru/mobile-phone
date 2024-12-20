/****************************************Copyright (c)***************************************
**-------------------------------------------------------------------------------------------
** Created by:          qinyx
** Last modified Date:  2014-02-28
** Last Version:        V1.00
** Descriptions:        STM32F407嵌入式实验箱
**-------------------------------------------------------------------------------------------*/
//********************************************************************************************/
#include "stm32f4xx.h"
#include "USART.h"
#include "delay.h"
#include "LED.h"
#include "ADC.h"
#include "DAC.h"
#include "KEY.h"
#include "CH455I2C.h"
#include "bsp_as608.h"
#include "as608_test.h"
#include "LCD.h"
#include "w25qxx.h" 
#include "spi.h"
#include "mp3.h"
// u8 flag = 0;
u8 num_a=0;//上位机输入指令
const u8 BCD_decode_tab[0x10] = { 0X3F, 0X06, 0X5B, 0X4F, 0X66, 0X6D, 0X7D, 0X07, 0X7F, 0X6F, 
									 0X77, 0X7C, 0X58, 0X5E, 0X79, 0X71 };//BCD
u8 ch455_key;//CH455的按键键值
	
int main(void)
{

  SysTick_Init();										//  系统滴答定时器初始化
	LEDGpio_Init();
	KEYGpio_Init();
  W25QXX_Init();
	UART3_Configuration();
  volatile unsigned short temp = 0; 
  temp = W25QXX_ReadID();
  printf("SPI test\r\n");
  Servo_init();
	UART6_Configuration(9600);
  // W25Q16:0XEF14 XT25F32B:0X0B15
  
  if(temp == 0XEF14 || temp == 0X0B15)
  {
      LED8_ON;
  }
//  W25QXX_TEST();
//    Adc_Init();
//    Dac1_Init();	
      /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
//    Debug_USART_Config();
  
  
    /*初始化环形缓冲区*/
    rx_queue_init();
//   
//    /*初始化指纹模块配置*/
    AS608_Config();
//  
//	/*测试STM32与指纹模块的通信*/
    Connect_Test();
    LCD_Init();
    LCD_Clear(WHITE); 
  
  unsigned char vol = 0;
    //播放开门音乐同时执行开门舵机转动，几秒后舵机复位代表锁门
    // OpenDoor(vol);
  while(1)
  {
//      if (Uart3.ReceiveFinish)
//        {
            FR_Task();//指纹模块操作
//            printf("成功输入的指令为：%d",num_a);//num_a通过蓝牙串口输入
//            printf("\r\n");
//            Uart3.ReceiveFinish = 0;
//        }
//      printf("%d",num_a);
//      delay_ms(500);
  } 
    

}





