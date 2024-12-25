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
#include "touch.h"
// #include "Servo.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

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
  // Servo_init();
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
    tp_dev.init(); 
    lv_init();                             // LVGL 初始化
    lv_port_disp_init();                   // 注册LVGL的显示任务
    lv_port_indev_init();                  // 注册LVGL的触屏检测任务
    //  // 按钮
    // lv_obj_t *myBtn = lv_btn_create(lv_scr_act());                               // 创建按钮; 父对象：当前活动屏幕
    // lv_obj_set_pos(myBtn, 10, 10);                                               // 设置坐标
    // lv_obj_set_size(myBtn, 120, 50);                                             // 设置大小
   
    // // 按钮上的文本
    // lv_obj_t *label_btn = lv_label_create(myBtn);                                // 创建文本标签，父对象：上面的btn按钮
    // lv_obj_align(label_btn, LV_ALIGN_CENTER, 0, 0);                              // 对齐于：父对象
    // lv_label_set_text(label_btn, "Test");                                        // 设置标签的文本
 
    // // 独立的标签
    // lv_obj_t *myLabel = lv_label_create(lv_scr_act());                           // 创建文本标签; 父对象：当前活动屏幕
    // lv_label_set_text(myLabel, "Hello world!");                                  // 设置标签的文本
    // lv_obj_align(myLabel, LV_ALIGN_CENTER, 0, 0);                                // 对齐于：父对象
    // lv_obj_align_to(myBtn, myLabel, LV_ALIGN_OUT_TOP_MID, 0, -20);               // 对齐于：某对象
  
  unsigned char vol = 0;
    //播放开门音乐同时执行开门舵机转动，几秒后舵机复位代表锁门
    // OpenDoor(vol);
  while(1)
  {
//      if (Uart3.ReceiveFinish)
//        {
            FR_Task();//指纹模块操作
//            printf("成功输入的指令为<|pad|>d",num_a);//num_a通过蓝牙串口输入
//            printf("\r\n");
//            Uart3.ReceiveFinish = 0;
//        }
//      printf("%d",num_a);
//      delay_ms(500);
       
      tp_dev.scan(0);
				delay_ms(1);

  //       static uint8_t msLVGL = 0;   // 用于控制LVGL任务的执行频率 = 0;  
  //       if (msLVGL++ == 5) 
  //       {
  //           lv_timer_handler(); // LVGL任务
  //           msLVGL = 0;
  //       }
  } 
    

}